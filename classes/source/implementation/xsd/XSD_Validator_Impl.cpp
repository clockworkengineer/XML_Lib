//
// Class: XSD_Impl
//
// Description: XSD_Validator class constructor / destructor and top-level dispatch.
//
// Dependencies: C++20 - Language standard features used.
//

#include "XSD_Impl.hpp"
#include "implementation/io/XML_FileSource.hpp"

#include <filesystem>

namespace XML_Lib {

/// @brief
/// XSD_Impl constructor.

XSD_Impl::XSD_Impl(Node &xNode) : xmlRoot(xNode) {}

/// @brief
/// XSD_Impl destructor.

XSD_Impl::~XSD_Impl() = default;

/// @brief
/// Parse XSD schema from source stream.

/// @param source XSD source stream.
namespace {
static constexpr std::size_t kMaxXsdSchemaNodeCount = 10000;

/// @brief
/// Implementation of countNodesInTree.

static std::size_t countNodesInTree(const Node &node)
{
  std::size_t count = 1;
  for (const auto &child : node.getChildren()) {
    count += countNodesInTree(child);
  }
  return count;
}
}

/// @brief
/// Implementation of XSD_Impl::parse.

void XSD_Impl::parse(ISource &source)
{
  // Determine base directory for schema imports/includes.
  schemaDirectory = std::filesystem::current_path();
  if (auto *fileSource = dynamic_cast<FileSource *>(&source)) {
    schemaDirectory = std::filesystem::path(fileSource->getFileName()).parent_path();
    const auto canonicalPath = std::filesystem::weakly_canonical(std::filesystem::path(fileSource->getFileName()));
    importedSchemas.insert(canonicalPath.string());
  }

  // Parse the XSD file as XML using the library itself
  XML xsdXml;
  xsdXml.parse(source);
  if (countNodesInTree(xsdXml.root()) > kMaxXsdSchemaNodeCount) {
    XML_LIB_THROW(SyntaxError("XSD schema complexity exceeds maximum allowed nodes."));
  }
  // Walk the resulting Node tree to populate the schema data model
  parseSchema(xsdXml.root());
}

void XSD_Impl::parseExternalSchema(const Node &includeNode)
{
  const auto schemaLocation = attrValue(includeNode, "schemaLocation");
  if (schemaLocation.empty()) {
    XML_LIB_THROW(IValidator::Error("Missing schemaLocation for xs:include or xs:import."));
  }

  std::filesystem::path schemaPath{ schemaLocation };
  if (!schemaPath.is_absolute()) { schemaPath = schemaDirectory / schemaPath; }
  const auto canonicalPath = std::filesystem::weakly_canonical(schemaPath);
  if (!std::filesystem::exists(canonicalPath)) {
    XML_LIB_THROW(IValidator::Error("XSD schema file not found: '" + canonicalPath.string() + "'."));
  }
  if (importedSchemas.contains(canonicalPath.string())) {
    XML_LIB_THROW(IValidator::Error("Circular schema include/import detected for '" + canonicalPath.string() + "'."));
  }

  XML xsdXml;
  FileSource fileSource(canonicalPath.string());
  xsdXml.parse(fileSource);
  XSD_Impl external(xsdXml.root());
  external.schemaDirectory = canonicalPath.parent_path();
  external.importedSchemas = importedSchemas;
  external.importedSchemas.insert(canonicalPath.string());
  external.parseSchema(xsdXml.root());
  mergeSchema(external);
}

void XSD_Impl::mergeSchema(const XSD_Impl &external)
{
  for (const auto &pair : external.simpleTypes) {
    const auto &name = pair.first;
    if (simpleTypes.contains(name)) {
      XML_LIB_THROW(IValidator::Error("Duplicate XSD simpleType definition '" + name + "'."));
    }
    simpleTypes.emplace(name, pair.second);
  }
  for (const auto &pair : external.complexTypes) {
    const auto &name = pair.first;
    if (complexTypes.contains(name)) {
      XML_LIB_THROW(IValidator::Error("Duplicate XSD complexType definition '" + name + "'."));
    }
    complexTypes.emplace(name, pair.second);
  }
  for (const auto &decl : external.rootElements) {
    const auto &name = decl.name;
    if (findTopLevelElement(name) != nullptr) {
      XML_LIB_THROW(IValidator::Error("Duplicate XSD element declaration '" + name + "'."));
    }
    rootElements.push_back(decl);
  }
}

/// @brief
/// Stringify the loaded schema back to XML text.

/// @param destination Destination stream.
void XSD_Impl::stringify(IDestination &destination) const
{
  destination.add("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  destination.add("<xs:schema");
  if (!targetNamespace.empty()) { destination.add(" targetNamespace=\"" + targetNamespace + "\""); }
  destination.add(" xmlns:xs=\"http://www.w3.org/2001/XMLSchema\">");
  destination.add("\n");

  for (const auto &[name, st] : simpleTypes) {
    destination.add("  <xs:simpleType name=\"" + name + "\">\n");
    destination.add("    <xs:restriction base=\"" + st.baseType + "\"/>\n");
    destination.add("  </xs:simpleType>\n");
  }

  for (const auto &elemDecl : rootElements) {
    destination.add("  <xs:element name=\"" + elemDecl.name + "\"");
    if (!elemDecl.typeRef.empty()) { destination.add(" type=\"" + elemDecl.typeRef + "\""); }
    destination.add("/>\n");
  }

  destination.add("</xs:schema>\n");
}

}// namespace XML_Lib