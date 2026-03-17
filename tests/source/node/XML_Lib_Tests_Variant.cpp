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
// Additional tests for Variant
TEST_CASE("Variant edge cases and integration", "[XML][Node][Variant][Edge][Integration]")
{
  SECTION("Variant copy/move semantics", "[XML][Variant][CopyMove]")
  {
    Variant original(Variant::Type::base);
    Variant moved(std::move(original));
    REQUIRE(moved.getNodeType() == Variant::Type::base);
  }

  SECTION("Variant with different node types", "[XML][Variant][NodeType]")
  {
    Variant prolog(Variant::Type::prolog);
    Variant element(Variant::Type::element);
    Variant content(Variant::Type::content);
    REQUIRE(prolog.getNodeType() == Variant::Type::prolog);
    REQUIRE(element.getNodeType() == Variant::Type::element);
    REQUIRE(content.getNodeType() == Variant::Type::content);
    REQUIRE(element.isNameable());
    REQUIRE(element.isIndexable());
    REQUIRE_FALSE(prolog.isNameable());
    REQUIRE(prolog.isIndexable());
  }

  SECTION("Variant integration with derived types", "[XML][Variant][Integration]")
  {
    Content contentVariant("integration test");
    REQUIRE(contentVariant.getNodeType() == Variant::Type::content);
    REQUIRE(contentVariant.getContents() == "integration test");
    contentVariant.addContent("123");
    REQUIRE(contentVariant.getContents() == "integration test123");
  }

  SECTION("Variant addChild edge case", "[XML][Variant][AddChild]")
  {
    Variant variant;
    Node child;
    variant.addChild(child);
    REQUIRE(variant.getChildren().size() == 1);
    variant.addChild(Node());
    REQUIRE(variant.getChildren().size() == 2);
  }
}
