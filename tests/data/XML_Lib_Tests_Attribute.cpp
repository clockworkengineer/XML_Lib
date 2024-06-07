//
// Unit Tests: XML_Lib_Tests_Attribute
//
// Description: XML Attribute  unit tests.
//

#include "XML_Lib_Tests.hpp"

using namespace XML_Lib;

TEST_CASE("XML attribute useage tests cases.", "[XML][Value]")
{
  SECTION("Create an empty attribute does not throw exeception.", "[XML][Attribute][Create]")
  {
    REQUIRE_NOTHROW(XMLAttribute("", XMLValue("")));
  }
  SECTION("Create an test attribute and check its value.", "[XML][Attribute][Create]")
  {
    auto attribute = XMLAttribute("test", XMLValue("unparsed", "parsed"));
    REQUIRE(attribute.getName() == "test");
    REQUIRE(attribute.getUnparsed() == "unparsed");
    REQUIRE(attribute.getParsed() == "parsed");
  }
  SECTION("Create an attribute entity reference value and check its type.", "[XML][Attribute][Create]")
  {
    auto attribute = XMLAttribute("test", XMLValue("&test;", "parsed"));
    REQUIRE_FALSE(!attribute.isEntityReference());
    REQUIRE_FALSE(attribute.isCharacterReference());
    REQUIRE_FALSE(!attribute.isReference());
    REQUIRE(attribute.getName() == "test");
    REQUIRE(attribute.getUnparsed() == "&test;");
    REQUIRE(attribute.getParsed() == "parsed");
  }
  SECTION("Create an attribute character reference value and check its type.", "[XML][Attribute][Create]")
  {
    auto attribute = XMLAttribute("test", XMLValue("&#0180;", "parsed"));
    REQUIRE_FALSE(attribute.isEntityReference());
    REQUIRE_FALSE(!attribute.isCharacterReference());
    REQUIRE_FALSE(!attribute.isReference());
    REQUIRE(attribute.getName() == "test");
    REQUIRE(attribute.getUnparsed() == "&#0180;");
    REQUIRE(attribute.getParsed() == "parsed");
  }
  SECTION("Create an empty vector of attributes and try to search.", "[XML][Attribute][API]")
  {
    std::vector<XMLAttribute> attributes;
    REQUIRE_FALSE(XMLAttribute::isAttrubutePresent(attributes, "test"));
  }
  SECTION("Create an  vector of three attributes and perform a successful search.", "[XML][Attribute][API]")
  {
    std::vector<XMLAttribute> attributes;
    attributes.emplace_back("test1", XMLValue("&test1;", "parsed"));
    attributes.emplace_back("test2", XMLValue("&test2;", "parsed"));
    attributes.emplace_back("test3", XMLValue("&test3;", "parsed"));
    REQUIRE_FALSE(!XMLAttribute::isAttrubutePresent(attributes, "test3"));
  }
  SECTION("Create an  vector of three attributes and perform a failed search.", "[XML][Attribute][API]")
  {
    std::vector<XMLAttribute> attributes;
    attributes.emplace_back("test1", XMLValue("&test1;", "parsed"));
    attributes.emplace_back("test2", XMLValue("&test2;", "parsed"));
    attributes.emplace_back("test3", XMLValue("&test3;", "parsed"));
    REQUIRE_FALSE(XMLAttribute::isAttrubutePresent(attributes, "test4"));
  }
}