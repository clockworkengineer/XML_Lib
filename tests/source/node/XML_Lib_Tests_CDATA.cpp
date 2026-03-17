#include "XML_Lib_Tests.hpp"

TEST_CASE("Create and use CDATA Node.", "[Node][CDATA][API]")
{
  SECTION("Create default CDATA Node.", "[XML][Node][CDATA][API]") { REQUIRE_NOTHROW(CDATA("")); }
  SECTION("Create CDATA Node.", "[XML][Node][CDATA][API]") { REQUIRE_NOTHROW(CDATA("&apos;Test&apos;")); }
  SECTION("Create CDATA Node and fetch comment related to it.", "[XML][Node][CDATA][API]")
  {
    auto xCDATA = CDATA("&apos;Test&apos;");
    REQUIRE(xCDATA.value() == "&apos;Test&apos;");
  }
  SECTION("Create and use CDATA using make/NRef API.", "[XML][Node][CDATA][Make][NRef][API]")
  {
    Node xNode = Node::make<CDATA>("&apos;Test&apos;");
    REQUIRE_FALSE(!isA<CDATA>(xNode));
    REQUIRE(NRef<CDATA>(xNode).value() == "&apos;Test&apos;");
  }
  SECTION("Create CDATA Node with empty string.", "[XML][Node][CDATA][Edge]")
  {
    CDATA cdata("");
    REQUIRE(cdata.value().empty());
    REQUIRE(cdata.getContents().empty());
  }
  SECTION("Create CDATA Node with large content.", "[XML][Node][CDATA][Large]")
  {
    std::string large(10000, 'x');
    CDATA cdata(large);
    REQUIRE(cdata.value() == large);
    REQUIRE(cdata.getContents() == large);
  }
  SECTION("Create CDATA Node with special characters.", "[XML][Node][CDATA][Special]")
  {
    std::string special = "<>&\"'\n\t";
    CDATA cdata(special);
    REQUIRE(cdata.value() == special);
    REQUIRE(cdata.getContents() == special);
  }
  SECTION("Mutate CDATA Node contents (not allowed, check immutability).", "[XML][Node][CDATA][Mutation]")
  {
    CDATA cdata("original");
    // No mutation API, so value should remain the same
    REQUIRE(cdata.value() == "original");
  }
}
