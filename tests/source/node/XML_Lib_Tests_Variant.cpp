#include "XML_Lib_Tests.hpp"

TEST_CASE("Create and use Variant", "[XML][Node][Variant][API]")
{
  SECTION("Create default variant.", "[XML][Variant][API]")
  {
    REQUIRE_NOTHROW(Variant());
  }
  SECTION("Create default variant.", "[XML][Variant][API]")
  {
    auto variant = Variant();
    REQUIRE_FALSE(variant.isIndexable());
    REQUIRE_FALSE(variant.isNameable());
    REQUIRE_NOTHROW(Variant());
  }
  SECTION("Create default variant check its children.", "[XML][Variant][API]")
  {
    auto variant = Variant();
    REQUIRE(variant.getChildren().size() == 0);
  }
  SECTION("Create default variant and add children Node.", "[XML][Variant][API]")
  {
    auto variant = Variant();
    variant.addChild(Node());
    variant.addChild(Node());
    variant.addChild(Node());
    REQUIRE(variant.getChildren().size() == 3);
  }
  SECTION("Create default variant and check contents is none.", "[XML][Variant][API]")
  {
    auto variant = Variant();
    REQUIRE(variant.getContents() == "");
  }
}
