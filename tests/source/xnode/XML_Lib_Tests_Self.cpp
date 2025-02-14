#include "XML_Lib_Tests.hpp"

TEST_CASE("Create and use Self XNode.","[XNode][Self][API]")
{
  SECTION("Create default Self XNode.", "[XML][XNode][Self][API]")
  {
    REQUIRE_NOTHROW(Self());
  }
  SECTION("Create Self with a given name.", "[XML][XNode][Self][API]")
  {
    auto xSelf = Self("test", {}, {});
    REQUIRE( xSelf.name() == "test");
    REQUIRE_FALSE(!xSelf.getAttributes().empty());
    REQUIRE_FALSE(!xSelf.getNameSpaces().empty());
    REQUIRE(xSelf.getContents()=="");
  }
  SECTION("Create and use Self using make/XRef API.", "[XML][XNode][Self][Make][XRef][API]")
  {
    std::vector<XMLAttribute> namespaces, attributes;
    auto xNode = XNode::make<Self>("test", namespaces, attributes);
    REQUIRE_FALSE(!isA<Self>(xNode));
    REQUIRE(XRef<Self>(xNode).name()=="test");
  }
}


