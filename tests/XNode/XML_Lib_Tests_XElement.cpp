
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
  xElement.addNameSpace("a:namespace", XMLValue {"value1", "value1"});
  xElement.addNameSpace("b:namespace", XMLValue {"value2", "value2"});
  xElement.addNameSpace("c:namespace", XMLValue {"value3", "value3"});

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
    REQUIRE_FALSE(!xElement.getAttributeList().empty());
    REQUIRE_FALSE(!xElement.getNamespaceList().empty());
    REQUIRE(xElement.getContents()=="");
  }
  SECTION("Create XElement with a given name and add attributes to it.", "[XML][XNode][XElement][API]")
  {
    auto xElement = XElement("test", {}, {});
    REQUIRE( xElement.name() == "test");
    addAttributes(xElement);
    REQUIRE(xElement.getAttributeList().size()==3);
    REQUIRE_FALSE(!xElement.getNamespaceList().empty());
  }
  SECTION("Create XElement with a given name and add namespaces to it.", "[XML][XNode][XElement][API]")
  {
    auto xElement = XElement("test", {}, {});
    REQUIRE( xElement.name() == "test");
    addNameSpaces(xElement);
    REQUIRE_FALSE(!xElement.getAttributeList().empty());
    REQUIRE(xElement.getNamespaceList().size()==3);
  }
  SECTION("Create XElement with a given name, add attributes to it, check that attr2 exists and get its values.", "[XML][XNode][XElement][API]")
  {
    auto xElement = XElement("test", {}, {});
    REQUIRE( xElement.name() == "test");
    addAttributes(xElement);
    REQUIRE(xElement.getAttributeList().size()==3);
    REQUIRE_FALSE(!xElement.isAttributePresent(("attr2")));
    REQUIRE(xElement["attr2"].getUnparsed() == "value2");
    REQUIRE(xElement["attr2"].getParsed() == "value2");
  }
  SECTION("Create XElement with a given name, add namespaces to it, check that b:namespace exists and get its values.", "[XML][XNode][XElement][API]")
  {
    auto xElement = XElement("test", {}, {});
    REQUIRE( xElement.name() == "test");
    addNameSpaces(xElement);
    REQUIRE(xElement.getNamespaceList().size()==3);
    REQUIRE_FALSE(!xElement.isNameSpacePresent(("b:namespace")));
    REQUIRE(xElement.getNameSpace("b:namespace").getUnparsed()=="value2");
    REQUIRE(xElement.getNameSpace("b:namespace").getParsed()=="value2");
  }
  SECTION("Create and use XElement using make/JRef API.", "[XML][XNode][XElement][Make][XRef][API]")
  {
    std::vector<XMLAttribute> namespaces, attributes;
    auto xNode = XNode::make<XElement>("test", namespaces, attributes);
    REQUIRE_FALSE(!xNode.isElement());
    REQUIRE(XRef<XElement>(xNode).name()=="test");
  }
}

