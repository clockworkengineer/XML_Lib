#include "XML_Lib_Tests.hpp"
#include "io/XML_BufferSource.hpp"
#include <string>

static std::string makeLargeXML(const size_t itemCount)
{
  std::string xml;
  xml.reserve(itemCount * 20 + 64);
  xml += "<root>";
  for (size_t i = 0; i < itemCount; ++i) {
    xml += "<item>value";
    xml += std::to_string(i);
    xml += "</item>";
  }
  xml += "</root>";
  return xml;
}

TEST_CASE("Performance regression: parse large XML document", "[performance]")
{
  constexpr size_t kLargeItemCount = 5000;
  const std::string xmlString = makeLargeXML(kLargeItemCount);

  BENCHMARK("parse large XML document") {
    BufferSource source(xmlString);
    XML xml;
    xml.parse(source);
    return xml.root().getChildren().size();
  };

  BufferSource source(xmlString);
  XML xml;
  xml.parse(source);
  REQUIRE(xml.root().getChildren().size() == kLargeItemCount);
}

TEST_CASE("Performance regression: XPath query on large node set", "[performance]")
{
  constexpr size_t kLargeItemCount = 2500;
  const std::string xmlString = makeLargeXML(kLargeItemCount);
  BufferSource source(xmlString);

  XML xml;
  xml.parse(source);
  XPath xpath(xml.root());

  BENCHMARK("XPath evaluate item name test") {
    const auto nodes = xpath.evaluate("//item[starts-with(., 'value')]");
    return nodes.size();
  };

  REQUIRE(xml.root().getChildren().size() == kLargeItemCount);
}

TEST_CASE("Performance regression: XSD validation throughput", "[performance]")
{
  constexpr size_t kLargeItemCount = 2000;
  const std::string xmlString = makeLargeXML(kLargeItemCount);
  BufferSource source(xmlString);
  XML xml;
  xml.parse(source);

  const std::string schema =
    "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'>"
    "<xs:element name='root'>"
    "<xs:complexType>"
    "<xs:sequence>"
    "<xs:element name='item' type='xs:string' maxOccurs='unbounded'/>"
    "</xs:sequence>"
    "</xs:complexType>"
    "</xs:element>"
    "</xs:schema>";

  XSD_Validator validator(xml.root());
  BufferSource schemaSource(schema);
  validator.parse(schemaSource);

  BENCHMARK("validate large document against XSD") {
    validator.validate(xml.root());
    return xml.root().getChildren().size();
  };

  REQUIRE(xml.root().getChildren().size() == kLargeItemCount);
}
