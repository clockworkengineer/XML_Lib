//
// Unit Tests: XML_Lib_Tests_Value
//
// Description: XML Value  unit tests.
//

#include "XML_Lib_Tests.hpp"

using namespace XML_Lib;

TEST_CASE("XML value useage tests cases.", "[XML][ExternalReference]")
{
  SECTION("Create an empty value does not throw exeception.", "[XML][ExternalReference][Create]")
  {
    REQUIRE_NOTHROW(XMLValue(""));
  }
  SECTION("Create an empty value and check its type.", "[XML][ExternalReference][API]")
  {
    XMLValue value("");
    REQUIRE_FALSE(value.isEntityReference());
    REQUIRE_FALSE(value.isCharacterReference());
    REQUIRE_FALSE(value.isReference());
  }
  SECTION("Create an entity reference value and check its type.", "[XML][ExternalReference][API]")
  {
    XMLValue value("&test;");
    REQUIRE_FALSE(!value.isEntityReference());
    REQUIRE_FALSE(value.isCharacterReference());
    REQUIRE_FALSE(!value.isReference());
  }
  SECTION("Create an character reference value and check its type.", "[XML][ExternalReference][API]")
  {
    XMLValue value("&#0180;");
    REQUIRE_FALSE(value.isEntityReference());
    REQUIRE_FALSE(!value.isCharacterReference());
    REQUIRE_FALSE(!value.isReference());
  }
  SECTION("Create a value with parsed and unparsed data.", "[XML][ExternalReference][API]")
  {
    XMLValue value("unparsed","parsed");
    REQUIRE_FALSE(value.isEntityReference());
    REQUIRE_FALSE(value.isCharacterReference());
    REQUIRE_FALSE(value.isReference());
    REQUIRE(value.getUnparsed() == "unparsed");
    REQUIRE(value.getParsed() == "parsed");
  }
}