#include "XML_Lib_Tests.hpp"

using namespace XML_Lib;

TEST_CASE("Create and use Root XNode.","[XNode][Root][API]")
{
  SECTION("Create default Root XNode.", "[XML][XNode][Root][API]")
  {
    REQUIRE_NOTHROW(Root());
  }
  SECTION("Create Root with a given name.", "[XML][XNode][Root][API]")
  {
    Root xRoot = Root("test", {}, {});
    REQUIRE( xRoot.name() == "test");
    REQUIRE_FALSE(!xRoot.getAttributes().empty());
    REQUIRE_FALSE(!xRoot.getNameSpaces().empty());
    REQUIRE(xRoot.getContents()=="");
  }
  SECTION("Create and use Root using make/XRef API.", "[XML][XNode][Root][Make][XRef][API]")
  {
    std::vector<XMLAttribute> namespaces, attributes;
    auto xNode = XNode::make<Root>("test", namespaces, attributes);
    REQUIRE_FALSE(!isA<Root>(xNode));
    REQUIRE(XRef<Root>(xNode).name()=="test");
  }
}


