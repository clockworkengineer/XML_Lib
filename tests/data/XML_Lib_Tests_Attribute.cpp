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
  SECTION("Create an attribute character reference value and check its type.", "[XML][Value][Create]")
  {
    auto attribute = XMLAttribute("test", XMLValue("&#0180;", "parsed"));
    REQUIRE_FALSE(attribute.isEntityReference());
    REQUIRE_FALSE(!attribute.isCharacterReference());
    REQUIRE_FALSE(!attribute.isReference());
    REQUIRE(attribute.getName() == "test");
    REQUIRE(attribute.getUnparsed() == "&#0180;");
    REQUIRE(attribute.getParsed() == "parsed");
  }
// Test attribute search empty list
// Test attribute search success.
// Test attribute search failure.
}