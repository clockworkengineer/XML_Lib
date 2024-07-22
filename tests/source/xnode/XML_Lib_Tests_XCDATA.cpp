//
// Unit Tests: XML_Lib_Tests_XCDATA.cpp
//
// Description: Create and use XCDATA XNode
//

#include "XML_Lib_Tests.hpp"

using namespace XML_Lib;

TEST_CASE("Create and use XCDATA XNode.","[XNode][XCDATA][API]")
{
  SECTION("Create default XCDATA XNode.", "[XML][XNode][XCDATA][API]")
  {
    REQUIRE_NOTHROW(CDATA(""));
  }
  SECTION("Create XCDATA XNode.", "[XML][XNode][XCDATA][API]")
  {
    REQUIRE_NOTHROW(CDATA("&apos;Test&apos;"));
  }
  SECTION("Create XCDATA XNode and fetch comment related to it.", "[XML][XNode][XCDATA][API]")
  {
    CDATA xCDATA = CDATA("&apos;Test&apos;");
    REQUIRE(xCDATA.value() == "&apos;Test&apos;");
  }
  SECTION("Create and use XCDATA using make/XRef API.", "[XML][XNode][XCDATA][Make][XRef][API]")
  {
    XNode xNode = XNode::make<CDATA>("&apos;Test&apos;");
    REQUIRE_FALSE(!isA<CDATA>(xNode));
    REQUIRE(XRef<CDATA>(xNode).value()=="&apos;Test&apos;");
  }
}


