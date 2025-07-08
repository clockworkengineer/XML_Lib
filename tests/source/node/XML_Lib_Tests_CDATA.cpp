#include "XML_Lib_Tests.hpp"

TEST_CASE("Create and use CDATA Node.","[Node][CDATA][API]")
{
  SECTION("Create default CDATA Node.", "[XML][Node][CDATA][API]")
  {
    REQUIRE_NOTHROW(CDATA(""));
  }
  SECTION("Create CDATA Node.", "[XML][Node][CDATA][API]")
  {
    REQUIRE_NOTHROW(CDATA("&apos;Test&apos;"));
  }
  SECTION("Create CDATA Node and fetch comment related to it.", "[XML][Node][CDATA][API]")
  {
    auto xCDATA = CDATA("&apos;Test&apos;");
    REQUIRE(xCDATA.value() == "&apos;Test&apos;");
  }
  SECTION("Create and use CDATA using make/NRef API.", "[XML][Node][CDATA][Make][NRef][API]")
  {
    Node xNode = Node::make<CDATA>("&apos;Test&apos;");
    REQUIRE_FALSE(!isA<CDATA>(xNode));
    REQUIRE(NRef<CDATA>(xNode).value()=="&apos;Test&apos;");
  }
}


