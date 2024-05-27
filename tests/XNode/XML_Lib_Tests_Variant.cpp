//
// Unit Tests: XML_Lib_Tests_Variant.cpp
//
// Description: Create and use XNode variants.
//

#include "XML_Lib_Tests.hpp"

using namespace XML_Lib;

TEST_CASE("Create and use Variant", "[XML][XNode][Variant][API]")
{
  XML xml;
  SECTION("Create default variant.", "[XML][Variant][API]")
  {
    REQUIRE_NOTHROW(Variant());
  }
  SECTION("Create default variant and check its type.", "[XML][Variant][API]")
  {
    auto variant = Variant();
    REQUIRE_FALSE(variant.isComment());
    REQUIRE_FALSE(variant.isContent());
    REQUIRE_FALSE(variant.isDeclaration());
    REQUIRE_FALSE(variant.isElement());
    REQUIRE_FALSE(variant.isEntity());
    REQUIRE_FALSE(variant.isIndexable());
    REQUIRE_FALSE(variant.isNameable());
    REQUIRE_FALSE(variant.isProlog());
    REQUIRE_FALSE(variant.isRoot());
    REQUIRE_FALSE(variant.isSelf());
    REQUIRE_FALSE(variant.isPI());
    REQUIRE_FALSE(variant.isDTD());
    REQUIRE_FALSE(variant.isCDATA());
  }
  SECTION("Create default variant check its children.", "[XML][Variant][API]")
  {
    auto variant = Variant();
    REQUIRE(variant.getChildren().size() == 0);
  }
  SECTION("Create default variant and add children XNode.", "[XML][Variant][API]")
  {
    auto variant = Variant();
    variant.addChild((XNode()));
    variant.addChild((XNode()));
    variant.addChild((XNode()));
    REQUIRE(variant.getChildren().size() == 3);
  }
  SECTION("Create default variant and check contents is none.", "[XML][Variant][API]")
  {
    auto variant = Variant();
    REQUIRE(variant.getContents() == "");
  }
}
