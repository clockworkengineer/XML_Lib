
//
// Unit Tests: XML_Lib_Tests_XSelf.cpp
//
// Description: Create and use XSelf XNode
//

#include "XML_Lib_Tests.hpp"

using namespace XML_Lib;

TEST_CASE("Create and use XSelf XNode.","[XNode][XSelf][API]")
{
  SECTION("Create default XSelf XNode.", "[XML][XNode][XSelf][API]")
  {
    REQUIRE_NOTHROW(Self());
  }
  SECTION("Create XSelf with a given name.", "[XML][XNode][XSelf][API]")
  {
    Self xSelf = Self("test", {}, {});
    REQUIRE( xSelf.name() == "test");
    REQUIRE_FALSE(!xSelf.getAttributes().empty());
    REQUIRE_FALSE(!xSelf.getNameSpaces().empty());
    REQUIRE(xSelf.getContents()=="");
  }
  SECTION("Create and use XSelf using make/XRef API.", "[XML][XNode][XSelf][Make][XRef][API]")
  {
    std::vector<XMLAttribute> namespaces, attributes;
    auto xNode = XNode::make<Self>("test", namespaces, attributes);
    REQUIRE_FALSE(!isA<Self>(xNode));
    REQUIRE(XRef<Self>(xNode).name()=="test");
  }
}


