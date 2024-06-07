//
// Unit Tests: XML_Lib_Tests_Parse_Comments
//
// Description:
//

#include "XML_Lib_Tests.hpp"

using namespace XML_Lib;

TEST_CASE("Parse XML elements with comments", "[XML][Parse][Comments]")
{
  XML xml;
  SECTION("A simple single line comment", "[XML][Parse][Comments]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
      "<!-- A single line comment -->\n"
      "<root></root>\n"
    };
    REQUIRE_NOTHROW(xml.parse(source));
  }
  SECTION("Multiple single line comments before root tag", "[XML][Parse][Comments]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
      "<!-- A single line comment -->\n"
      " <!-- A single line comment -->\n"
      " <!-- A single line comment -->\n"
      " <root></root>\n"
    };
    REQUIRE_NOTHROW(xml.parse(source));
  }
  SECTION("Multiple comments inside root element and between its children ", "[XML][Parse][Comments]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<AddressBook>\n"
      "<!--Address one -->\n"
      "<Address>    This is some contents 1   </Address>\n"
      "<!--Address two -->\n"
      "<Address>    This is some contents 2   </Address>\n"
      "<!--Address three -->\n"
      "<Address>    This is some contents 3   </Address>\n"
      "</AddressBook>\n"
    };
    xml.parse(source);
    auto &xDeclaration = XRef<XDeclaration>(xml.declaration());
    auto &xRoot = XRef<XRoot>(xml.root());
    REQUIRE(xDeclaration.version() == "1.0");
    REQUIRE(xDeclaration.encoding() == "UTF-8");
    REQUIRE(xDeclaration.standalone() == "no");
    REQUIRE(xRoot.name() == "AddressBook");
    REQUIRE(xRoot.getChildren().size() == 13);
    REQUIRE(XRef<XComment>(xRoot.getChildren()[1]).value() == "Address one ");
    REQUIRE(xRoot[0].name() == "Address");
    REQUIRE(xRoot[0].getContents() == "    This is some contents 1   ");
    REQUIRE(XRef<XComment>(xRoot.getChildren()[5]).value() == "Address two ");
    REQUIRE(xRoot[1].name() == "Address");
    REQUIRE(xRoot[1].getContents() == "    This is some contents 2   ");
    REQUIRE(XRef<XComment>(xRoot.getChildren()[9]).value() == "Address three ");
    REQUIRE(xRoot[2].name() == "Address");
    REQUIRE(xRoot[2].getContents() == "    This is some contents 3   ");
  }
  SECTION("A single comment after root element", "[XML][Parse][Comments]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
      "<root></root>\n"
      "<!-- A single line comment --> "
    };
    REQUIRE_NOTHROW(xml.parse(source));
  }
  SECTION("A simple multi line comment", "[XML][Parse][Comments]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
      "<!-- A single line comment\n"
      " another line\n"
      " another line\n"
      " and another line\n"
      "--> <root></root>\n"
    };
    REQUIRE_NOTHROW(xml.parse(source));
  }
  SECTION("A simple comment within element content", "[XML][Parse][Comments]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
      "<root>Test<!-- a simple comment -->Test"
      "</root>\n"
    };
    xml.parse(source);
    REQUIRE(XRef<XElement>(xml.root()).getContents() == "TestTest");
  }
  SECTION("A simple comment within element contents and content remains intact", "[XML][Parse][Comments]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
      "<root>Test  <!-- a simple comment -->  Test"
      "</root>\n"
    };
    xml.parse(source);
    REQUIRE(XRef<XElement>(xml.root()).getContents() == "Test    Test");
  }
  SECTION("A simple single line comment containing -- is illegal", "[XML][Parse][Comments]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
      "<!-- A single line comment-- --> "
      "<root></root>\n"
    };
    REQUIRE_THROWS_WITH(
      xml.parse(source), "XML Syntax Error [Line: 2 Column: 32] Missing closing '>' for comment line.");
  }
  SECTION("A simple single line comment ending with -- is illegal", "[XML][Parse][Comments]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
      "<!-- A single line comment ---> "
      "<root></root>\n"
    };
    REQUIRE_THROWS_WITH(
      xml.parse(source), "XML Syntax Error [Line: 2 Column: 33] Missing closing '>' for comment line.");
  }
  SECTION("A comment before declaration.", "[XML][Parse][Comments]")
  {
    BufferSource source{
      "<!-- A single line comment -->\n"
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
      "<root></root>\n"
    };
    REQUIRE_THROWS_WITH(xml.parse(source),
      "XML Syntax Error [Line: 2 Column: 10] Declaration allowed only at the start of the document.");
  }
  SECTION("A commentat at start with no declaratiion.", "[XML][Parse][Comments]")
  {
    BufferSource source{
      "<!-- A single line comment -->\n"
      "<root></root>\n"
    };
    REQUIRE_NOTHROW(xml.parse(source));
  }
  SECTION("A comment in attribute value.", "[XML][Parse][Comments]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
      "<root attr1=\"66<!-- A single line comment -->\"</root>\n"
    };
    REQUIRE_THROWS_WITH(xml.parse(source),
      "XML Syntax Error [Line: 2 Column: 51] Attribute value contains invalid character '<', '\"', ''' or '&'.");
  }
}