//
// Class: XSD_Schema
//
// Description: Pre-compiled XSD schema implementation.
//
// Dependencies: C++20 - Language standard features used.
//

#include "XSD_Schema.hpp"
#include "XSD_Impl.hpp"
#include "XSD_Validator.hpp"
#include "implementation/io/XML_BufferSource.hpp"
#include "implementation/io/XML_FileSource.hpp"

namespace XML_Lib {

XSD_Schema::XSD_Schema(ISource &source)
{
  Node dummyRoot = Node::make<Root>();
  XSD_Impl impl(dummyRoot);
  impl.parse(source);
  schemaDef = impl.getSchemaDefinition();
}

XSD_Schema::XSD_Schema(const std::string_view schemaSource)
{
  std::error_code ec;
  if (std::filesystem::exists(std::filesystem::path(schemaSource), ec)) {
    FileSource source{ std::string(schemaSource) };
    Node dummyRoot = Node::make<Root>();
    XSD_Impl impl(dummyRoot);
    impl.parse(source);
    schemaDef = impl.getSchemaDefinition();
  } else {
    BufferSource source{ schemaSource };
    Node dummyRoot = Node::make<Root>();
    XSD_Impl impl(dummyRoot);
    impl.parse(source);
    schemaDef = impl.getSchemaDefinition();
  }
}

XSD_Schema XSD_Schema::fromFile(const std::filesystem::path &filePath)
{
  FileSource source{ filePath.string() };
  return XSD_Schema(source);
}

XSD_Schema::~XSD_Schema() = default;

void XSD_Schema::validate(const Node &xNode) const
{
  Node &mutableNode = const_cast<Node &>(xNode);
  XSD_Validator validator(mutableNode, *this);
  validator.validate(xNode);
}

} // namespace XML_Lib
