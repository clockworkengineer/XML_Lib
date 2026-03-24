#include "XML_Lib_Tests.hpp"

TEST_CASE("Create and use Self Node.","[Node][Self][API]")
{
  SECTION("Create default Self Node.", "[XML][Node][Self][API]")
  {
    REQUIRE_NOTHROW(Self());
  }
  SECTION("Create Self with a given name.", "[XML][Node][Self][API]")
  {
    auto xSelf = Self("test", {}, {});
    REQUIRE( xSelf.name() == "test");
    REQUIRE_FALSE(!xSelf.getAttributes().empty());
    REQUIRE_FALSE(!xSelf.getNameSpaces().empty());
    REQUIRE(xSelf.getContents()=="");
  }
  SECTION("Create and use Self using make/NRef API.", "[XML][Node][Self][Make][NRef][API]")
  {
    std::vector<XMLAttribute> namespaces, attributes;
    auto xNode = Node::make<Self>("test", namespaces, attributes);
    REQUIRE_FALSE(!isA<Self>(xNode));
    REQUIRE(NRef<Self>(xNode).name()=="test");
  }
}
// Additional tests for Self
TEST_CASE("Self edge cases and integration", "[Node][Self][Edge][Integration]")
{
  SECTION("Self copy/move semantics", "[XML][Node][Self][CopyMove]")
  {
    Self original("self");
    Self moved(std::move(original));
    REQUIRE(moved.name() == "self");
    REQUIRE(moved.getNodeType() == Variant::Type::self);
  }

  SECTION("Self with child nodes (should be empty)", "[XML][Node][Self][Children]")
  {
    std::vector<XMLAttribute> emptyAttrs;
    std::vector<XMLAttribute> emptyNamespaces;
    Node selfNode = Node::make<Self>("self", emptyAttrs, emptyNamespaces);
    REQUIRE(selfNode.getChildren().empty());
  }

  SECTION("Self integration with XML parsing", "[XML][Node][Self][XML]")
  {
    XML xml;
    BufferSource source{
      "<?xml version=\"1.0\"?>\n<root><self attr='1'/></root>\n"
    };
    xml.parse(source);
    auto &rootChildren = NRef<Element>(xml.root()).getChildren();
    bool foundSelf = false;
    for (const auto &child : rootChildren) {
      if (isA<Self>(child)) {
        foundSelf = true;
        REQUIRE(NRef<Self>(child).name() == "self");
        REQUIRE(NRef<Self>(child).getAttributes().size() == 1);
        REQUIRE(NRef<Self>(child).getAttributes()[0].getName() == "attr");
        REQUIRE(NRef<Self>(child).getAttributes()[0].getParsed() == "1");
      }
    }
    REQUIRE(foundSelf);
  }
}


