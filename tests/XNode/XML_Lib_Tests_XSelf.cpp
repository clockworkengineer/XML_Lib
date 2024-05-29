
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
    REQUIRE_NOTHROW(XSelf());
  }
  SECTION("Create XSelf with a given name.", "[XML][XNode][XSelf][API]")
  {
    XSelf xSelf = XSelf("test", {}, {});
    REQUIRE( xSelf.name() == "test");
    REQUIRE_FALSE(!xSelf.getAttributeList().empty());
    REQUIRE_FALSE(!xSelf.getNamespaceList().empty());
    REQUIRE(xSelf.getContents()=="");
  }
  SECTION("Create and use XSelf using make/XRef API.", "[XML][XNode][XSelf][Make][XRef][API]")
  {
    std::vector<XMLAttribute> namespaces, attributes;
    auto xNode = XNode::make<XSelf>("test", namespaces, attributes);
    REQUIRE_FALSE(!xNode.isSelf());
    REQUIRE(XRef<XSelf>(xNode).name()=="test");
  }
}


