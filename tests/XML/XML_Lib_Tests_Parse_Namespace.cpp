//
// Unit Tests: XML_Lib_Tests_Parse_Namespace
//
// Description:
//

#include "XML_Lib_Tests.hpp"

using namespace XML_Lib;

TEST_CASE("Parse XML with defined namespaces.", "[XML][Parse][Namespace]")
{
  XML xml;
  SECTION("A root document and two namespaces defined in the child two table elements.", "[XML][Parse][Namespace]")
  {
    BufferSource source{
      "<root>\n"
      "<h:table xmlns:h=\"http://www.w3.org/TR/html4/\">\n"
      "<h:tr>\n"
      "<h:td>Apples</h:td>\n"
      "<h:td>Bananas</h:td>\n"
      "</h:tr>\n"
      "</h:table>\n"
      "<f:table xmlns:f=\"https://www.w3schools.com/furniture\">\n"
      "<f:name>African Coffee Table</f:name>\n"
      "<f:width>80</f:width>\n"
      "<f:length>120</f:length>\n"
      "</f:table>\n"
      "</root>\n"
    };

    xml.parse(source);
    auto &xRoot = XRef<XElement>(xml.root());
    REQUIRE(xRoot[0].name() == "h:table");
    REQUIRE(xRoot[0].getNamespaceList().size() == 1);
    REQUIRE(xRoot[0].getNameSpace("h").getParsed() == "http://www.w3.org/TR/html4/");
    REQUIRE(xRoot[1].getNamespaceList().size() == 1);
    REQUIRE(xRoot[1].name() == "f:table");
    REQUIRE(xRoot[1].getNameSpace("f").getParsed() == "https://www.w3schools.com/furniture");
  }
  SECTION("A root document and two namespaces defined in the root element.", "[XML][Parse][Namespace]")
  {
    BufferSource source{
      "<root xmlns:h=\"http://www.w3.org/TR/html4/\" xmlns:f=\"https://www.w3schools.com/furniture\">\n"
      "<h:table>\n"
      "<h:tr>\n"
      "<h:td>Apples</h:td>\n"
      "<h:td>Bananas</h:td>\n"
      "</h:tr>\n"
      "</h:table>\n"
      "<f:table>\n"
      "<f:name>African Coffee Table</f:name>\n"
      "<f:width>80</f:width>\n"
      "<f:length>120</f:length>\n"
      "</f:table>\n"
      "</root>\n"
    };

    xml.parse(source);
    auto &xRoot = XRef<XElement>(xml.root());
    REQUIRE(xRoot[0].name() == "h:table");
    REQUIRE(xRoot[0].getNamespaceList().size() == 2);
    REQUIRE(xRoot[0].getNameSpace("h").getParsed() == "http://www.w3.org/TR/html4/");
    REQUIRE(xRoot[0].getNameSpace("f").getParsed() == "https://www.w3schools.com/furniture");
    REQUIRE(xRoot[1].name() == "f:table");
    REQUIRE(xRoot[1].getNamespaceList().size() == 2);
    REQUIRE(xRoot[1].getNameSpace("h").getParsed() == "http://www.w3.org/TR/html4/");
    REQUIRE(xRoot[1].getNameSpace("f").getParsed() == "https://www.w3schools.com/furniture");
  }
  SECTION(
    "A root document and two namespaces defined in the root element and non-existant namespace g for one of tables.",
    "[XML][Parse][Namespace]")
  {

    BufferSource source{
      "<root xmlns:h=\"http://www.w3.org/TR/html4/\" xmlns:f=\"https://www.w3schools.com/furniture\">\n"
      "<h:table><h:tr><h:td>Apples</h:td><h:td>Bananas</h:td></h:tr></h:table><g:table>\n"
      "<g:name>African Coffee Table</g:name><g:width>80</g:width>\n"
      "<g:length>120</g:length></g:table></root>\n"
    };
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error [Line: 3 Column: 43] Namespace used but not defined.");
  }
  SECTION("A root document with a default namespace", "[XML][Parse][Namespace]")
  {
    BufferSource source{
      "<table xmlns=\"http://www.w3.org/TR/html4/\"><tr><td>Apples</td><td>Bananas</td></tr></table>\n"
    };
    xml.parse(source);
    auto &xRoot = XRef<XElement>(xml.root());
    REQUIRE(xRoot[0].name() == "tr");
    REQUIRE(xRoot[0].getNamespaceList().size() == 1);
    REQUIRE(xRoot[0].getNameSpace(":").getParsed() == "http://www.w3.org/TR/html4/");
  }
  SECTION("A root document and two namespaces (the same name) defined in the root element.", "[XML][Parse][Namespace]")
  {
    BufferSource source{
      "<root xmlns:f=\"http://www.w3.org/TR/html4/\" xmlns:f=\"https://www.w3schools.com/furniture\">\n"
      "<f:table><f:tr><f:td>Apples</f:td><f:td>Bananas</f:td></f:tr></f:table><f:table>\n"
      "<f:name>African Coffee Table</f:name><f:width>80</f:width>\n"
      "<f:length>120</f:length></f:table></root>\n"
    };
    REQUIRE_THROWS_WITH(
      xml.parse(source), "XML Syntax Error: Attribute 'xmlns:f' defined more than once within start tag.");
  }
  SECTION("A root document defining one namespace tha is overridden by a child", "[XML][Parse][Namespace]")
  {
    BufferSource source{
      "<root xmlns:h=\"http://www.w3.org/TR/html4/\" xmlns:f=\"https://www.w3schools.com/furniture\">\n"
      "<h:table><h:tr><h:td>Apples</h:td><h:td>Bananas</h:td></h:tr></h:table>\n"
      "<f:table xmlns:f=\"https://www.w3schools.com/furniture\">\n"
      "<f:name>African Coffee Table</f:name><f:width>80</f:width>\n"
      "<f:length>120</f:length></f:table></root>\n"
    };
    REQUIRE_NOTHROW(xml.parse(source));
  }
}