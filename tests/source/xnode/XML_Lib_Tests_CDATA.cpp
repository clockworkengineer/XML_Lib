#include "XML_Lib_Tests.hpp"

TEST_CASE("Create and use CDATA XNode.","[XNode][CDATA][API]")
{
  SECTION("Create default CDATA XNode.", "[XML][XNode][CDATA][API]")
  {
    REQUIRE_NOTHROW(CDATA(""));
  }
  SECTION("Create CDATA XNode.", "[XML][XNode][CDATA][API]")
  {
    REQUIRE_NOTHROW(CDATA("&apos;Test&apos;"));
  }
  SECTION("Create CDATA XNode and fetch comment related to it.", "[XML][XNode][CDATA][API]")
  {
    auto xCDATA = CDATA("&apos;Test&apos;");
    REQUIRE(xCDATA.value() == "&apos;Test&apos;");
  }
  SECTION("Create and use CDATA using make/XRef API.", "[XML][XNode][CDATA][Make][XRef][API]")
  {
    XNode xNode = XNode::make<CDATA>("&apos;Test&apos;");
    REQUIRE_FALSE(!isA<CDATA>(xNode));
    REQUIRE(XRef<CDATA>(xNode).value()=="&apos;Test&apos;");
  }
}


