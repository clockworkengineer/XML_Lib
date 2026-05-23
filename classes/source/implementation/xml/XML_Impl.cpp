//
// Class: XML_Impl
//
// Description: XML class implementation api layer.
//
// Dependencies: C++20 - Language standard features used..
//

#include "XML_Impl.hpp"
#include <sstream>
#if defined(XML_LIB_ENABLE_XSD)
#include "XSD_Validator.hpp"
#endif
#if defined(XML_LIB_ENABLE_XPATH)
#include "XPath.hpp"
#endif

namespace XML_Lib {

/// @brief
/// Return the first child of xNode satisfying predicate, or nullptr.

template<typename Predicate>
/// @brief
/// Find the first child matching the supplied predicate.

static Node *findFirstChild(Node &xNode, Predicate &&predicate)
{
  for (auto &child : xNode.getChildren()) {
    if (predicate(child)) { return &child; }
  }
  return nullptr;
}

/// @brief
/// Initialize XML_Impl with the provided stringifier and parser.

XML_Impl::XML_Impl(IStringify *stringify, IParser *parser)
{
  entityMapper = std::make_unique<XML_EntityMapper>();
  if (parser == nullptr) {
    xmlParser = std::make_unique<Default_Parser>(*entityMapper);
  } else {
    xmlParser.reset(parser);
  }
#if defined(XML_LIB_ENABLE_STRINGIFY)
  if (stringify == nullptr) {
    xmlStringifier = std::make_unique<Default_Stringify>();
  } else {
    xmlStringifier.reset(stringify);
  }
#else
  (void)stringify;
#endif
}

/// @brief
/// Destroy XML_Impl and release any owned resources.

XML_Impl::~XML_Impl() = default;

/// @brief
/// Return the XML_Lib version string.

std::string XML_Impl::version()
{
  std::stringstream versionString;
  versionString << "XML_Lib Version " << XML_VERSION_MAJOR << "." << XML_VERSION_MINOR << "." << XML_VERSION_PATCH;
  return versionString.str();
}

#if defined(XML_LIB_ENABLE_DTD)
/// @brief
/// Return the DTD node used for document validation.

Node &XML_Impl::dtd()
{
  if (xmlParser->canValidate()) {
    if (Node *found = findFirstChild(prolog(), [](const Node &n) { return isA<DTD>(n); })) {
      return *found;
    }
  }
  XML_LIB_THROW(Error("No DTD found."));
}
#endif

/// @brief
/// Return the XML prolog node.

Node &XML_Impl::prolog()
{
  if (!xmlRoot.isEmpty()) { return xmlRoot; }
  XML_LIB_THROW(Error("No XML has been parsed."));
}

/// @brief
/// Return the XML declaration node.

Node &XML_Impl::declaration() { return prolog().getChildren()[0]; }

/// @brief
/// Return the document root node.

Node &XML_Impl::root()
{
  if (Node *found = findFirstChild(prolog(), [](const Node &n) { return isA<Root>(n) || isA<Self>(n); })) {
    return *found;
  }
  XML_LIB_THROW(Error("No root element found."));
}

#if defined(XML_LIB_ENABLE_DTD)
/// @brief
/// Validate the parsed XML against the attached DTD.

void XML_Impl::validate() { xmlParser->validate(prolog()); }
#endif

#if defined(XML_LIB_ENABLE_XSD)
/// @brief
/// Validate the parsed XML against an XSD schema string.

void XML_Impl::validate(const std::string_view &xsdSource)
{
  XSD_Validator xsdValidator(root());
  BufferSource source(xsdSource);
  xsdValidator.parse(source);
  xsdValidator.validate(root());
}
#endif

#if defined(XML_LIB_ENABLE_XPATH)
/// @brief
/// Evaluate an XPath expression against the current document.

std::vector<const Node *> XML_Impl::xpath(const std::string_view expression)
{
  XPath xp(root());
  return xp.evaluate(expression);
}
#endif

/// @brief
/// Implementation of XML_Impl::parse.

void XML_Impl::parse(ISource &source, const ParseOptions &options)
{
  XML_Arena::ScopedCurrentArena scopedCurrentArena(arena);
  XML_Arena::ScopedDefaultResource scopedDefaultResource(arena);
  xmlRoot = xmlParser->parse(source, options);
}
#if defined(XML_LIB_ENABLE_STRINGIFY)
/// @brief
/// Serialize the current XML document to the destination.

void XML_Impl::stringify(IDestination &destination) { xmlStringifier->stringify(prolog(), destination, 0); }
#endif

/// @brief
/// Traverse the XML tree and invoke the supplied action.

void XML_Impl::traverse(IAction &action)
{
  if (xmlRoot.isEmpty()) { XML_LIB_THROW(Error("No XML to traverse.")); }
  traverseNodes(xmlRoot, action);
}

/// @brief
/// Traverse the XML tree with a const action handler.

void XML_Impl::traverse(IAction &action) const
{
  if (xmlRoot.isEmpty()) { XML_LIB_THROW(Error("No XML to traverse.")); }
  traverseNodes(xmlRoot, action);
}
}// namespace XML_Lib