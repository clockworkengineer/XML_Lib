//
// Unit Tests: XML_Lib_Tests_Value
//
// Description: XML Value  unit tests.
//

#include "XML_Lib_Tests.hpp"

using namespace XML_Lib;

TEST_CASE("XML value usage tests cases.", "[XML][Value]")
{
  SECTION("Create an empty value does not throw exception.", "[XML][Value][Create]")
  {
    REQUIRE_NOTHROW(XMLValue(""));
  }
  SECTION("Create an empty value and check its type.", "[XML][Value][Create]")
  {
    XMLValue value("");
    REQUIRE_FALSE(value.isEntityReference());
    REQUIRE_FALSE(value.isCharacterReference());
    REQUIRE_FALSE(value.isReference());
  }
  SECTION("Create an entity reference value and check its type.", "[XML][Value][Create]")
  {
    XMLValue value("&test;");
    REQUIRE_FALSE(!value.isEntityReference());
    REQUIRE_FALSE(value.isCharacterReference());
    REQUIRE_FALSE(!value.isReference());
  }
  SECTION("Create an character reference value and check its type.", "[XML][Value][Create]")
  {
    XMLValue value("&#0180;");
    REQUIRE_FALSE(value.isEntityReference());
    REQUIRE_FALSE(!value.isCharacterReference());
    REQUIRE_FALSE(!value.isReference());
  }
  SECTION("Create a value with parsed and unparsed data.", "[XML][Value][Create]")
  {
    XMLValue value("unparsed","parsed");
    REQUIRE_FALSE(value.isEntityReference());
    REQUIRE_FALSE(value.isCharacterReference());
    REQUIRE_FALSE(value.isReference());
    REQUIRE(value.getUnparsed() == "unparsed");
    REQUIRE(value.getParsed() == "parsed");
  }
}