#include "XML_Lib_Tests.hpp"

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
    auto &xRoot = NRef<Element>(xml.root());
    REQUIRE(xRoot[0].name() == "h:table");
    REQUIRE(xRoot[0].getNameSpaces().size() == 1);
    REQUIRE(xRoot[0].getNameSpace("h").getParsed() == "http://www.w3.org/TR/html4/");
    REQUIRE(xRoot[1].getNameSpaces().size() == 1);
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
    auto &xRoot = NRef<Element>(xml.root());
    REQUIRE(xRoot[0].name() == "h:table");
    REQUIRE(xRoot[0].getNameSpaces().size() == 2);
    REQUIRE(xRoot[0].getNameSpace("h").getParsed() == "http://www.w3.org/TR/html4/");
    REQUIRE(xRoot[0].getNameSpace("f").getParsed() == "https://www.w3schools.com/furniture");
    REQUIRE(xRoot[1].name() == "f:table");
    REQUIRE(xRoot[1].getNameSpaces().size() == 2);
    REQUIRE(xRoot[1].getNameSpace("h").getParsed() == "http://www.w3.org/TR/html4/");
    REQUIRE(xRoot[1].getNameSpace("f").getParsed() == "https://www.w3schools.com/furniture");
  }
  SECTION(
    "A root document and two namespaces defined in the root element and non-existent namespace g for one of tables.",
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
    auto &xRoot = NRef<Element>(xml.root());
    REQUIRE(xRoot[0].name() == "tr");
    REQUIRE(xRoot[0].getNameSpaces().size() == 1);
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
  SECTION("A root document defining one namespace that is overridden by a child", "[XML][Parse][Namespace]")
  {
    BufferSource source{
      "<root xmlns:h=\"http://www.w3.org/TR/html4/\" xmlns:f=\"https://www.w3schools.com/furniture\">\n"
      "<h:table><h:tr><h:td>Apples</h:td><h:td>Bananas</h:td></h:tr></h:table>\n"
      "<f:table xmlns:f=\"https://www.w3schools.com/furniture/child\">\n"
      "<f:name>African Coffee Table</f:name><f:width>80</f:width>\n"
      "<f:length>120</f:length></f:table></root>\n"
    };
    REQUIRE_NOTHROW(xml.parse(source));
    auto &xRoot = NRef<Element>(xml.root());
    REQUIRE(xRoot[0].name() == "h:table");
    REQUIRE(xRoot[0].getNameSpaces().size() == 2);
    REQUIRE(xRoot[0].getNameSpace("h").getParsed() == "http://www.w3.org/TR/html4/");
    REQUIRE(xRoot[0].getNameSpace("f").getParsed() == "https://www.w3schools.com/furniture");
    REQUIRE(xRoot[1].getNameSpaces().size() == 3);
    REQUIRE(xRoot[1].name() == "f:table");
    REQUIRE(xRoot[1].getNameSpace("f").getParsed() == "https://www.w3schools.com/furniture/child");
  }
  SECTION("Default namespace overridden by child element", "[XML][Parse][Namespace]")
  {
    BufferSource source{
      "<root xmlns=\"http://www.w3.org/TR/html4/\">\n"
      "<table xmlns=\"https://www.w3schools.com/furniture\">\n"
      "<tr><td>Apples</td><td>Bananas</td></tr>\n"
      "</table>\n"
      "</root>\n"
    };
    xml.parse(source);
    auto &xRoot = NRef<Element>(xml.root());
    REQUIRE(xRoot[0].getNameSpace(":").getParsed() == "https://www.w3schools.com/furniture");
  }
  SECTION("Element with undeclared prefix should throw", "[XML][Parse][Namespace]")
  {
    BufferSource source{
      "<root>\n"
      "<x:table>\n"
      "<x:tr><x:td>Apples</x:td><x:td>Bananas</x:td></x:tr>\n"
      "</x:table>\n"
      "</root>\n"
    };
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error [Line: 3 Column: 35] Namespace used but not defined.");
  }
  SECTION("Element with empty namespace URI", "[XML][Parse][Namespace]")
  {
    BufferSource source{
      "<root xmlns:h=\"\">\n"
      "<h:table><h:tr><h:td>Apples</h:td></h:tr></h:table>\n"
      "</root>\n"
    };
    xml.parse(source);
    auto &xRoot = NRef<Element>(xml.root());
    REQUIRE(xRoot[0].getNameSpace("h").getParsed() == "");
  }
  SECTION("Element with boundary namespace name", "[XML][Parse][Namespace]")
  {
    BufferSource source{
      "<root xmlns:a=\"http://example.com/a\" xmlns:b=\"http://example.com/b\">\n"
      "<a:table><b:tr><a:td>Apples</a:td><b:td>Bananas</b:td></b:tr></a:table>\n"
      "</root>\n"
    };
    xml.parse(source);
    auto &xRoot = NRef<Element>(xml.root());
    REQUIRE(xRoot[0].getNameSpace("a").getParsed() == "http://example.com/a");
    REQUIRE(xRoot[0].getNameSpace("b").getParsed() == "http://example.com/b");
  }
}

TEST_CASE("QName API for namespace-aware elements.", "[XML][Namespace][QName]")
{
  XML xml;
  SECTION("getPrefix returns correct prefix for prefixed element", "[XML][Namespace][QName]")
  {
    BufferSource source{
      "<root xmlns:h=\"http://www.w3.org/TR/html4/\">\n"
      "<h:table></h:table>\n"
      "</root>\n"
    };
    xml.parse(source);
    auto &xRoot = NRef<Element>(xml.root());
    REQUIRE(xRoot[0].getPrefix() == "h");
  }
  SECTION("getLocalName returns name without prefix for prefixed element", "[XML][Namespace][QName]")
  {
    BufferSource source{
      "<root xmlns:h=\"http://www.w3.org/TR/html4/\">\n"
      "<h:table></h:table>\n"
      "</root>\n"
    };
    xml.parse(source);
    auto &xRoot = NRef<Element>(xml.root());
    REQUIRE(xRoot[0].getLocalName() == "table");
  }
  SECTION("getPrefix returns empty string for unprefixed element", "[XML][Namespace][QName]")
  {
    BufferSource source{ "<root><table></table></root>" };
    xml.parse(source);
    auto &xRoot = NRef<Element>(xml.root());
    REQUIRE(xRoot[0].getPrefix() == "");
  }
  SECTION("getLocalName returns full name for unprefixed element", "[XML][Namespace][QName]")
  {
    BufferSource source{ "<root><table></table></root>" };
    xml.parse(source);
    auto &xRoot = NRef<Element>(xml.root());
    REQUIRE(xRoot[0].getLocalName() == "table");
  }
  SECTION("getNamespaceURI resolves URI from in-scope namespace", "[XML][Namespace][QName]")
  {
    BufferSource source{
      "<root xmlns:h=\"http://www.w3.org/TR/html4/\">\n"
      "<h:table></h:table>\n"
      "</root>\n"
    };
    xml.parse(source);
    auto &xRoot = NRef<Element>(xml.root());
    REQUIRE(xRoot[0].getNamespaceURI() == "http://www.w3.org/TR/html4/");
  }
  SECTION("getNamespaceURI resolves default namespace URI", "[XML][Namespace][QName]")
  {
    BufferSource source{ "<table xmlns=\"http://www.w3.org/TR/html4/\"><tr></tr></table>\n" };
    xml.parse(source);
    auto &xRoot = NRef<Element>(xml.root());
    REQUIRE(xRoot[0].getPrefix() == "");
    REQUIRE(xRoot[0].getLocalName() == "tr");
    REQUIRE(xRoot[0].getNamespaceURI() == "http://www.w3.org/TR/html4/");
  }
  SECTION(
    "getNamespaceURI returns empty string for unprefixed element with no default namespace", "[XML][Namespace][QName]")
  {
    BufferSource source{ "<root><table></table></root>" };
    xml.parse(source);
    auto &xRoot = NRef<Element>(xml.root());
    REQUIRE(xRoot[0].getNamespaceURI() == "");
  }
}

TEST_CASE("Namespace-aware serialization round-trip.", "[XML][Namespace][Stringify]")
{
  SECTION("Round-trip with prefixed namespace on child element", "[XML][Namespace][Stringify]")
  {
    const std::string xmlString =
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
      "<root>"
      "<h:table xmlns:h=\"http://www.w3.org/TR/html4/\"></h:table>"
      "</root>";
    checkStringify(xmlString);
  }
  SECTION("Round-trip with namespace declared on root element", "[XML][Namespace][Stringify]")
  {
    const std::string xmlString =
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
      "<root xmlns:h=\"http://www.w3.org/TR/html4/\" xmlns:f=\"https://www.w3schools.com/furniture\">"
      "<h:table></h:table>"
      "<f:table></f:table>"
      "</root>";
    checkStringify(xmlString);
  }
  SECTION("Round-trip with default namespace", "[XML][Namespace][Stringify]")
  {
    const std::string xmlString =
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
      "<table xmlns=\"http://www.w3.org/TR/html4/\">"
      "<tr><td>Apples</td></tr>"
      "</table>";
    checkStringify(xmlString);
  }
}

TEST_CASE("Attribute namespace validation.", "[XML][Namespace][Attribute]")
{
  XML xml;
  SECTION("Attribute with declared namespace prefix is allowed", "[XML][Namespace][Attribute]")
  {
    BufferSource source{
      "<root xmlns:h=\"http://www.w3.org/TR/html4/\">\n"
      "<h:table h:border=\"1\"></h:table>\n"
      "</root>\n"
    };
    REQUIRE_NOTHROW(xml.parse(source));
  }
  SECTION("Attribute with undeclared namespace prefix should throw", "[XML][Namespace][Attribute]")
  {
    BufferSource source{
      "<root>\n"
      "<table x:border=\"1\"></table>\n"
      "</root>\n"
    };
    REQUIRE_THROWS_WITH(xml.parse(source),
      "XML Syntax Error [Line: 3 Column: 1] Namespace used but not defined in attribute 'x:border'.");
  }
}