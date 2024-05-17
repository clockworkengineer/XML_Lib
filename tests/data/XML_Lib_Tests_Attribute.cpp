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
}