#include "XML_Lib_Tests.hpp"

using namespace XML_Lib;

TEST_CASE("Create and use Root Node.","[Node][Root][API]")
{
  SECTION("Create default Root Node.", "[XML][Node][Root][API]")
  {
    REQUIRE_NOTHROW(Root());
  }
  SECTION("Create Root with a given name.", "[XML][Node][Root][API]")
  {
    auto xRoot = Root("test", {}, {});
    REQUIRE( xRoot.name() == "test");
    REQUIRE_FALSE(!xRoot.getAttributes().empty());
    REQUIRE_FALSE(!xRoot.getNameSpaces().empty());
    REQUIRE(xRoot.getContents()=="");
  }
  SECTION("Create and use Root using make/XRef API.", "[XML][Node][Root][Make][XRef][API]")
  {
    std::vector<XMLAttribute> namespaces, attributes;
    auto xNode = Node::make<Root>("test", namespaces, attributes);
    REQUIRE_FALSE(!isA<Root>(xNode));
    REQUIRE(XRef<Root>(xNode).name()=="test");
  }
}


