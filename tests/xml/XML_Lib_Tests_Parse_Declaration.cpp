//
// Unit Tests: XML_Lib_Tests_Parse_Declaration
//
// Description:
//

#include "XML_Lib_Tests.hpp"

using namespace XML_Lib;

TEST_CASE("Use XML object to parse XML declaration", "[XML][Parse][Declaration]")
{
  XML xml;
  SECTION("Parse XML declaration. ", "[XML][Parse][Declaration]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
      "<root></root>\n"
    };
    xml.parse(source);
    auto &xDeclaration = XRef<XDeclaration>(xml.declaration());
    REQUIRE(xDeclaration.version() == "1.0");
    REQUIRE(xDeclaration.encoding() == "UTF-8");
    REQUIRE(xDeclaration.standalone() == "no");
  }
  SECTION("Parse XML declaration with unsupported version. ", "[XML][Parse][Declaration]")
  {
    BufferSource source{
      "<?xml version=\"1.2\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
      "<root></root>\n"
    };
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error: Unsupported XML version value '1.2' specified.");
  }
  SECTION("Parse XML declaration with unsupported encoding. ", "[XML][Parse][Declaration]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-32\" standalone=\"no\"?>\n"
      "<root></root>\n"
    };
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error: Unsupported XML encoding value 'UTF-32' specified.");
  }
  SECTION("Parse XML declaration with invalid standalone value. ", "[XML][Parse][Declaration]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"maybe\"?>\n"
      "<root></root>\n"
    };
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error: Unsupported XML standalone value 'maybe' specified.");
  }
  SECTION("Parse XML declaration with extra content after root element. ", "[XML][Parse][Declaration]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
      "<root></root>extra content.\n"
    };
    REQUIRE_THROWS_WITH(
      xml.parse(source), "XML Syntax Error [Line: 2 Column: 18] Extra content at the end of document.");
  }
  SECTION("Parse version 1.0, encoding == UTF-8, standalone == yes XML declaration. ", "[XML][Parse][Declaration]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-16\" standalone=\"yes\"?> "
      "<root></root>\n"
    };
    xml.parse(source);
    auto &xDeclaration = XRef<XDeclaration>(xml.declaration());
    REQUIRE(xDeclaration.version() == "1.0");
    REQUIRE(xDeclaration.encoding() == "UTF-16");
    REQUIRE(xDeclaration.standalone() == "yes");
  }
  SECTION("Parse version 1.0, standalone == yes XML declaration. ", "[XML][Parse][Declaration]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" standalone=\"yes\"?>\n"
      "<root></root>\n"
    };
    xml.parse(source);
    auto &xDeclaration = XRef<XDeclaration>(xml.declaration());
    REQUIRE(xDeclaration.version() == "1.0");
    REQUIRE(xDeclaration.encoding() == "UTF-8");
    REQUIRE(xDeclaration.standalone() == "yes");
  }
  SECTION("Check declaration contains at least version attribute.", "[XML][Parse][Declaration]")
  {
    BufferSource source{ "<?xml?> <root></root>\n" };
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error [Line: 1 Column: 6] Version missing from declaration.");
  }
  SECTION("Parse empty XML declaration no end tag ", "[XML][Parse][Declaration]")
  {
    BufferSource source{
      "<?xml version=\"1.0\">\n"
      "<root></root>"
    };
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error [Line: 1 Column: 20] Declaration end tag not found.");
  }
  SECTION("Parse  XML declaration with no root element.", "[XML][Parse][Declaration]")
  {
    BufferSource source{ "<?xml version=\"1.0\"?>\n" };
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error [Line: 2 Column: 2] Missing root element.");
  }
  SECTION("Parse empty XML declaration two root elements.", "[XML][Parse][Declaration]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<root></root>\n"
      "<root></root>"
    };
    REQUIRE_THROWS_WITH(
      xml.parse(source), "XML Syntax Error [Line: 3 Column: 4] Extra content at the end of document.");
  }
  SECTION("Parse empty XML declaration with content before root element.", "[XML][Parse][Declaration]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "content<root></root>"
    };
    REQUIRE_THROWS_WITH(
      xml.parse(source), "XML Syntax Error [Line: 2 Column: 2] Content detected before root element.");
  }
  SECTION("Parse empty XML declaration no closing root tag ", "[XML][Parse][Declaration]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<root>\n"
    };
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error [Line: 3 Column: 2] Missing closing tag.");
  }
  SECTION("Parse empty XML declaration no closing child tag ", "[XML][Parse][Declaration]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<root><child></root>\n"
    };
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error [Line: 2 Column: 24] Missing closing tag.");
  }
  SECTION("Parse wrongly ordered attributes in XML declaration. ", "[XML][Parse][Declaration]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" standalone=\"no\" encoding=\"UTF-8\"?>\n"
      " <root></root>\n"
    };
    REQUIRE_THROWS_WITH(xml.parse(source),
      "XML Syntax Error [Line: 1 Column: 45] Incorrect order for version, encoding and standalone attributes.");
  }
  SECTION("Parse XML with declaration but no root element", "[XML][Parse][Declaration]")
  {
    BufferSource source{ "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n" };
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error [Line: 2 Column: 2] Missing root element.");
  }
  SECTION("Parse XML with no declaration but a root element. ", "[XML][Parse][Declaration]")
  {
    BufferSource source{ "<root></root>\n" };
    REQUIRE_NOTHROW(xml.parse(source));
    auto &xDeclaration = XRef<XDeclaration>(xml.declaration());
    REQUIRE(xDeclaration.version() == "1.0");
    REQUIRE(xDeclaration.encoding() == "UTF-8");
    REQUIRE(xDeclaration.standalone() == "no");
    REQUIRE(XRef<XRoot>(xml.root()).name() == "root");
  }
}
