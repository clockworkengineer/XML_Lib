
//
// Unit Tests: XML_Lib_Tests_XRoot.cpp
//
// Description: Create and use XRoot XNode
//

#include "XML_Lib_Tests.hpp"

using namespace XML_Lib;

TEST_CASE("Create and use XRoot XNode.","[XNode][XRoot][API]")
{
  SECTION("Create default XRoot XNode.", "[XML][XNode][XRoot][API]")
  {
    REQUIRE_NOTHROW(Root());
  }
  SECTION("Create XRoot with a given name.", "[XML][XNode][XRoot][API]")
  {
    Root xRoot = Root("test", {}, {});
    REQUIRE( xRoot.name() == "test");
    REQUIRE_FALSE(!xRoot.getAttributes().empty());
    REQUIRE_FALSE(!xRoot.getNameSpaces().empty());
    REQUIRE(xRoot.getContents()=="");
  }
  SECTION("Create and use XRoot using make/XRef API.", "[XML][XNode][XRoot][Make][XRef][API]")
  {
    std::vector<XMLAttribute> namespaces, attributes;
    auto xNode = XNode::make<Root>("test", namespaces, attributes);
    REQUIRE_FALSE(!isA<Root>(xNode));
    REQUIRE(XRef<Root>(xNode).name()=="test");
  }
}


