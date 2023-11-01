//
// Unit Tests: XML
//
// Description:
//

#include "XML_Tests.hpp"

using namespace XML_Lib;

TEST_CASE("Parse XML elements with comments", "[XML][Parse][Comments]")
{
  std::string xmlString;
  SECTION("A simple single line comment", "[XML][Parse][Comments]")
  {
    xmlString =
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
      "<!-- A single line comment -->\n"
      "<root></root>\n";
    BufferSource source{ xmlString };
    XML xml;
    REQUIRE_NOTHROW(xml.parse(source));
  }
  SECTION("Multiple single line comments before root tag", "[XML][Parse][Comments]")
  {
    xmlString =
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
      "<!-- A single line comment -->\n"
      " <!-- A single line comment -->\n"
      " <!-- A single line comment -->\n"
      " <root></root>\n";
    BufferSource source{ xmlString };
    XML xml;
    REQUIRE_NOTHROW(xml.parse(source));
  }
  SECTION("Multiple comments inside root element and between its children ", "[XML][Parse][Comments]")
  {
    xmlString =
      "<?xml version=\"1.0\"?>\n"
      "<AddressBook>\n"
      "<!--Address one -->\n"
      "<Address>    This is some contents 1   </Address>\n"
      "<!--Address two -->\n"
      "<Address>    This is some contents 2   </Address>\n"
      "<!--Address three -->\n"
      "<Address>    This is some contents 3   </Address>\n"
      "</AddressBook>\n";
    BufferSource source{ xmlString };
    XML xml;
    xml.parse(source);
    REQUIRE(XRef<XDeclaration>(xml.declaration()).version() == "1.0");
    REQUIRE(XRef<XDeclaration>(xml.declaration()).encoding() == "UTF-8");
    REQUIRE(XRef<XDeclaration>(xml.declaration()).standalone() == "no");
    REQUIRE(XRef<XElement>(xml.root()).name() == "AddressBook");
    REQUIRE(XRef<XElement>(xml.root()).getChildren().size() == 13);
    REQUIRE(XRef<XComment>(*XRef<XElement>(xml.root()).getChildren()[1]).comment() == "Address one ");
    REQUIRE(XRef<XElement>(xml.root())[0].name() == "Address");
    REQUIRE(XRef<XElement>(xml.root())[0].getContents() == "    This is some contents 1   ");
    REQUIRE(XRef<XComment>(*XRef<XElement>(xml.root()).getChildren()[5]).comment() == "Address two ");
    REQUIRE(XRef<XElement>(xml.root())[1].name() == "Address");
    REQUIRE(XRef<XElement>(xml.root())[1].getContents() == "    This is some contents 2   ");
    REQUIRE(XRef<XComment>(*XRef<XElement>(xml.root()).getChildren()[9]).comment() == "Address three ");
    REQUIRE(XRef<XElement>(xml.root())[2].name() == "Address");
    REQUIRE(XRef<XElement>(xml.root())[2].getContents() == "    This is some contents 3   ");
  }
  SECTION("A single comment after root element", "[XML][Parse][Comments]")
  {
    xmlString =
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
      "<root></root>\n"
      "<!-- A single line comment --> ";
    BufferSource source{ xmlString };
    XML xml;
    REQUIRE_NOTHROW(xml.parse(source));
  }
  SECTION("A simple multi line comment", "[XML][Parse][Comments]")
  {
    xmlString =
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
      "<!-- A single line comment\n"
      " another line\n"
      " another line\n"
      " and another line\n"
      "--> <root></root>\n";
    BufferSource source{ xmlString };
    XML xml;
    REQUIRE_NOTHROW(xml.parse(source));
  }
  SECTION("A simple comment within element content", "[XML][Parse][Comments]")
  {
    xmlString =
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
      "<root>Test<!-- a simple comment -->Test"
      "</root>\n";
    BufferSource source{ xmlString };
    XML xml;
    xml.parse(source);
    REQUIRE(XRef<XElement>(xml.root()).getContents() == "TestTest");
  }
  SECTION("A simple comment within element contents and content remains intact", "[XML][Parse][Comments]")
  {
    xmlString =
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
      "<root>Test  <!-- a simple comment -->  Test"
      "</root>\n";
    BufferSource source{ xmlString };
    XML xml;
    xml.parse(source);
    REQUIRE(XRef<XElement>(xml.root()).getContents() == "Test    Test");
  }
  SECTION("A simple single line comment containing -- is illegal", "[XML][Parse][Comments]")
  {
    xmlString =
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
      "<!-- A single line comment-- --> "
      "<root></root>\n";
    BufferSource source{ xmlString };
    XML xml;
    REQUIRE_THROWS_WITH(
      xml.parse(source), "XML Syntax Error [Line: 2 Column: 30] Missing closing '>' for comment line.");
  }
  SECTION("A simple single line comment ending with -- is illegal", "[XML][Parse][Comments]")
  {
    xmlString =
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
      "<!-- A single line comment ---> "
      "<root></root>\n";
    BufferSource source{ xmlString };
    XML xml;
    REQUIRE_THROWS_WITH(
      xml.parse(source), "XML Syntax Error [Line: 2 Column: 31] Missing closing '>' for comment line.");
  }
}