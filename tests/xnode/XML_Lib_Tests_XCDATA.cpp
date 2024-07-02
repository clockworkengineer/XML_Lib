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
    REQUIRE_NOTHROW(XCDATA(""));
  }
  SECTION("Create XCDATA XNode.", "[XML][XNode][XCDATA][API]")
  {
    REQUIRE_NOTHROW(XCDATA("&apos;Test&apos;"));
  }
  SECTION("Create XCDATA XNode and fetch comment related to it.", "[XML][XNode][XCDATA][API]")
  {
    XCDATA xCDATA = XCDATA("&apos;Test&apos;");
    REQUIRE(xCDATA.value() == "&apos;Test&apos;");
  }
  SECTION("Create and use XCDATA using make/XRef API.", "[XML][XNode][XCDATA][Make][XRef][API]")
  {
    XNode xNode = XNode::make<XCDATA>("&apos;Test&apos;");
    REQUIRE_FALSE(!isA<XCDATA>(xNode));
    REQUIRE(XRef<XCDATA>(xNode).value()=="&apos;Test&apos;");
  }
}


