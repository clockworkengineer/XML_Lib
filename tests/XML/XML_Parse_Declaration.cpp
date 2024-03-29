//
// Unit Tests: XML
//
// Description:
//

#include "XML_Tests.hpp"

using namespace XML_Lib;

TEST_CASE("Use XML object to parse XML declaration", "[XML][Parse][Declaration]")
{
  std::string xmlString;
  SECTION("Parse XML declaration. ", "[XML][Parse][Declaration]")
  {
    xmlString =
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
      "<root></root>\n";
    BufferSource source{ xmlString };
    XML xml;
    xml.parse(source);
    REQUIRE(XRef<XDeclaration>(xml.declaration()).version() == "1.0");
    REQUIRE(XRef<XDeclaration>(xml.declaration()).encoding() == "UTF-8");
    REQUIRE(XRef<XDeclaration>(xml.declaration()).standalone() == "no");
  }
  SECTION("Parse XML declaration with unsupported version. ", "[XML][Parse][Declaration]")
  {
    xmlString =
      "<?xml version=\"1.2\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
      "<root></root>\n";
    BufferSource source{ xmlString };
    XML xml;
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error: Unsupported XML version value '1.2' specified.");
  }
  SECTION("Parse XML declaration with unsupported encoding. ", "[XML][Parse][Declaration]")
  {
    xmlString =
      "<?xml version=\"1.0\" encoding=\"UTF-32\" standalone=\"no\"?>\n"
      "<root></root>\n";
    BufferSource source{ xmlString };
    XML xml;
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error: Unsupported XML encoding value 'UTF-32' specified.");
  }
  SECTION("Parse XML declaration with invalid standalone value. ", "[XML][Parse][Declaration]")
  {
    xmlString =
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"maybe\"?>\n"
      "<root></root>\n";
    BufferSource source{ xmlString };
    XML xml;
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error: Unsupported XML standalone value 'maybe' specified.");
  }
  SECTION("Parse XML declaration with extra content after root element. ", "[XML][Parse][Declaration]")
  {
    xmlString =
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
      "<root></root>extra content.\n";
    BufferSource source{ xmlString };
    XML xml;
    REQUIRE_THROWS_WITH(
      xml.parse(source), "XML Syntax Error [Line: 2 Column: 18] Extra content at the end of document.");
  }
  SECTION("Parse version 1.0, encoding == UTF-8, standalone == yes XML declaration. ", "[XML][Parse][Declaration]")
  {
    xmlString =
      "<?xml version=\"1.0\" encoding=\"UTF-16\" standalone=\"yes\"?> "
      "<root></root>\n";
    BufferSource source{ xmlString };
    XML xml;
    xml.parse(source);
    REQUIRE(XRef<XDeclaration>(xml.declaration()).version() == "1.0");
    REQUIRE(XRef<XDeclaration>(xml.declaration()).encoding() == "UTF-16");
    REQUIRE(XRef<XDeclaration>(xml.declaration()).standalone() == "yes");
  }
  SECTION("Parse version 1.0, standalone == yes XML declaration. ", "[XML][Parse][Declaration]")
  {
    xmlString =
      "<?xml version=\"1.0\" standalone=\"yes\"?>\n"
      "<root></root>\n";
    BufferSource source{ xmlString };
    XML xml;
    xml.parse(source);
    REQUIRE(XRef<XDeclaration>(xml.declaration()).version() == "1.0");
    REQUIRE(XRef<XDeclaration>(xml.declaration()).encoding() == "UTF-8");
    REQUIRE(XRef<XDeclaration>(xml.declaration()).standalone() == "yes");
  }
  SECTION("Check declaration contains at least version attribute.", "[XML][Parse][Declaration]")
  {
    xmlString = "<?xml?> <root></root>\n";
    BufferSource source{ xmlString };
    XML xml;
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error [Line: 1 Column: 6] Version missing from declaration.");
  }
  SECTION("Parse empty XML declaration no end tag ", "[XML][Parse][Declaration]")
  {
    xmlString =
      "<?xml version=\"1.0\">\n"
      "<root></root>";
    BufferSource source{ xmlString };
    XML xml;
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error [Line: 1 Column: 20] Declaration end tag not found.");
  }
  SECTION("Parse empty XML declaration no root element.", "[XML][Parse][Declaration]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n";
    BufferSource source{ xmlString };
    XML xml;
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error [Line: 2 Column: 2] Missing root element.");
  }
  SECTION("Parse empty XML declaration with content before root element.", "[XML][Parse][Declaration]")
  {
    xmlString =
      "<?xml version=\"1.0\"?>\n"
      "content<root></root>";
    BufferSource source{ xmlString };
    XML xml;
    REQUIRE_THROWS_WITH(
      xml.parse(source), "XML Syntax Error [Line: 2 Column: 2] Content detected before root element.");
  }
  SECTION("Parse empty XML declaration no closing root tag ", "[XML][Parse][Declaration]")
  {
    xmlString =
      "<?xml version=\"1.0\"?>\n"
      "<root>\n";
    BufferSource source{ xmlString };
    XML xml;
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error [Line: 3 Column: 2] Missing closing tag.");
  }
  SECTION("Parse empty XML declaration no closing child tag ", "[XML][Parse][Declaration]")
  {
    xmlString =
      "<?xml version=\"1.0\"?>\n"
      "<root><child></root>\n";
    BufferSource source{ xmlString };
    XML xml;
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error [Line: 2 Column: 24] Missing closing tag.");
  }
  SECTION("Parse wrongly ordered attributes in XML declaration. ", "[XML][Parse][Declaration]")
  {
    xmlString =
      "<?xml version=\"1.0\" standalone=\"no\" encoding=\"UTF-8\"?>\n"
      " <root></root>\n";
    BufferSource source{ xmlString };
    XML xml;
    REQUIRE_THROWS_WITH(xml.parse(source),
      "XML Syntax Error [Line: 1 Column: 45] Incorrect order for version, encoding and standalone attributes.");
  }
  SECTION("Parse XML with declaration but no root element", "[XML][Parse][Declaration]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n";
    BufferSource source{ xmlString };
    XML xml;
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error [Line: 2 Column: 2] Missing root element.");
  }
  SECTION("Parse XML with no declaration but a root element. ", "[XML][Parse][Declaration]")
  {
    xmlString = "<root></root>\n";
    BufferSource source{ xmlString };
    XML xml;
    REQUIRE_NOTHROW(xml.parse(source));
    REQUIRE(XRef<XDeclaration>(xml.declaration()).version() == "1.0");
    REQUIRE(XRef<XDeclaration>(xml.declaration()).encoding() == "UTF-8");
    REQUIRE(XRef<XDeclaration>(xml.declaration()).standalone() == "no");
    REQUIRE(XRef<XRoot>(xml.root()).name() == "root");
  }
}
