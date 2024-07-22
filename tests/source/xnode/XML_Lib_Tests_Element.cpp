
//
// Unit Tests: XML_Lib_Tests_Element.cpp
//
// Description: Create and use Element XNode
//

#include "XML_Lib_Tests.hpp"

using namespace XML_Lib;

void addAttributes(const Element &xElement)
{
  xElement.addAttribute("attr1", XMLValue {"value1", "value1"});
  xElement.addAttribute("attr2", XMLValue {"value2", "value2"});
  xElement.addAttribute("attr3", XMLValue {"value3", "value3"});

}
void addNameSpaces(const Element &xElement)
{
  xElement.addNameSpace("a", XMLValue {"value1", "value1"});
  xElement.addNameSpace("b", XMLValue {"value2", "value2"});
  xElement.addNameSpace("c", XMLValue {"value3", "value3"});

}
TEST_CASE("Create and use Element XNode.","[XNode][Element][API]")
{
  SECTION("Create default Element XNode.", "[XML][XNode][Element][API]")
  {
    REQUIRE_NOTHROW(Element());
  }
  SECTION("Create Element with a given name.", "[XML][XNode][Element][API]")
  {
    auto xElement = Element("test", {}, {});
    REQUIRE( xElement.name() == "test");
    REQUIRE_FALSE(!xElement.getAttributes().empty());
    REQUIRE_FALSE(!xElement.getNameSpaces().empty());
    REQUIRE_FALSE(!xElement.getContents().empty());
  }
  SECTION("Create Element with a given name and add attributes to it.", "[XML][XNode][Element][API]")
  {
    auto xElement = Element("test", {}, {});
    REQUIRE( xElement.name() == "test");
    addAttributes(xElement);
    REQUIRE(xElement.getAttributes().size()==3);
    REQUIRE_FALSE(!xElement.getNameSpaces().empty());
  }
  SECTION("Create Element with a given name and add namespaces to it.", "[XML][XNode][Element][API]")
  {
    auto xElement = Element("test", {}, {});
    REQUIRE( xElement.name() == "test");
    addNameSpaces(xElement);
    REQUIRE(xElement.getAttributes().size()==3);
    REQUIRE(xElement.getNameSpaces().size()==3);
  }
  SECTION("Create Element with a given name, add attributes to it, check that attr2 exists and get its values.", "[XML][XNode][Element][API]")
  {
    auto xElement = Element("test", {}, {});
    REQUIRE( xElement.name() == "test");
    addAttributes(xElement);
    REQUIRE(xElement.getAttributes().size()==3);
    REQUIRE_FALSE(!xElement.hasAttribute(("attr2")));
    REQUIRE(xElement["attr2"].getUnparsed() == "value2");
    REQUIRE(xElement["attr2"].getParsed() == "value2");
  }
  SECTION("Create Element with a given name, add namespaces to it, check that b exists and get its values.", "[XML][XNode][Element][API]")
  {
    auto xElement = Element("test", {}, {});
    REQUIRE( xElement.name() == "test");
    addNameSpaces(xElement);
    REQUIRE(xElement.getNameSpaces().size()==3);
    REQUIRE_FALSE(!xElement.hasNameSpace(("b")));
    REQUIRE(xElement.getNameSpace("b").getUnparsed()=="value2");
    REQUIRE(xElement.getNameSpace("b").getParsed()=="value2");
  }
  SECTION("Create and use Element using make/XRef API.", "[XML][XNode][Element][Make][XRef][API]")
  {
    std::vector<XMLAttribute> namespaces, attributes;
    auto xNode = XNode::make<Element>("test", namespaces, attributes);
    REQUIRE_FALSE(!isA<Element>(xNode));
    REQUIRE(XRef<Element>(xNode).name()=="test");
  }
}

