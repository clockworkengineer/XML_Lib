
//
// Unit Tests: XML_Lib_Tests_XElement.cpp
//
// Description: Create and use XElement XNode
//

#include "XML_Lib_Tests.hpp"

using namespace XML_Lib;

void addAttributes(const XElement &xElement)
{
  xElement.addAttribute("attr1", XMLValue {"value1", "value1"});
  xElement.addAttribute("attr2", XMLValue {"value2", "value2"});
  xElement.addAttribute("attr3", XMLValue {"value3", "value3"});

}
void addNameSpaces(const XElement &xElement)
{
  xElement.addNameSpace("a", XMLValue {"value1", "value1"});
  xElement.addNameSpace("b", XMLValue {"value2", "value2"});
  xElement.addNameSpace("c", XMLValue {"value3", "value3"});

}
TEST_CASE("Create and use XElement XNode.","[XNode][XElement][API]")
{
  SECTION("Create default XElement XNode.", "[XML][XNode][XElement][API]")
  {
    REQUIRE_NOTHROW(XElement());
  }
  SECTION("Create XElement with a given name.", "[XML][XNode][XElement][API]")
  {
    auto xElement = XElement("test", {}, {});
    REQUIRE( xElement.name() == "test");
    REQUIRE_FALSE(!xElement.getAttributes().empty());
    REQUIRE_FALSE(!xElement.getNameSpaces().empty());
    REQUIRE_FALSE(!xElement.getContents().empty());
  }
  SECTION("Create XElement with a given name and add attributes to it.", "[XML][XNode][XElement][API]")
  {
    auto xElement = XElement("test", {}, {});
    REQUIRE( xElement.name() == "test");
    addAttributes(xElement);
    REQUIRE(xElement.getAttributes().size()==3);
    REQUIRE_FALSE(!xElement.getNameSpaces().empty());
  }
  SECTION("Create XElement with a given name and add namespaces to it.", "[XML][XNode][XElement][API]")
  {
    auto xElement = XElement("test", {}, {});
    REQUIRE( xElement.name() == "test");
    addNameSpaces(xElement);
    REQUIRE(xElement.getAttributes().size()==3);
    REQUIRE(xElement.getNameSpaces().size()==3);
  }
  SECTION("Create XElement with a given name, add attributes to it, check that attr2 exists and get its values.", "[XML][XNode][XElement][API]")
  {
    auto xElement = XElement("test", {}, {});
    REQUIRE( xElement.name() == "test");
    addAttributes(xElement);
    REQUIRE(xElement.getAttributes().size()==3);
    REQUIRE_FALSE(!xElement.hasAttribute(("attr2")));
    REQUIRE(xElement["attr2"].getUnparsed() == "value2");
    REQUIRE(xElement["attr2"].getParsed() == "value2");
  }
  SECTION("Create XElement with a given name, add namespaces to it, check that b exists and get its values.", "[XML][XNode][XElement][API]")
  {
    auto xElement = XElement("test", {}, {});
    REQUIRE( xElement.name() == "test");
    addNameSpaces(xElement);
    REQUIRE(xElement.getNameSpaces().size()==3);
    REQUIRE_FALSE(!xElement.hasNameSpace(("b")));
    REQUIRE(xElement.getNameSpace("b").getUnparsed()=="value2");
    REQUIRE(xElement.getNameSpace("b").getParsed()=="value2");
  }
  SECTION("Create and use XElement using make/XRef API.", "[XML][XNode][XElement][Make][XRef][API]")
  {
    std::vector<XMLAttribute> namespaces, attributes;
    auto xNode = XNode::make<XElement>("test", namespaces, attributes);
    REQUIRE_FALSE(!isA<XElement>(xNode));
    REQUIRE(XRef<XElement>(xNode).name()=="test");
  }
}

