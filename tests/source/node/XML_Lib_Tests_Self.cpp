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
  SECTION("Create and use Self using make/XRef API.", "[XML][Node][Self][Make][XRef][API]")
  {
    std::vector<XMLAttribute> namespaces, attributes;
    auto xNode = Node::make<Self>("test", namespaces, attributes);
    REQUIRE_FALSE(!isA<Self>(xNode));
    REQUIRE(XRef<Self>(xNode).name()=="test");
  }
}


