//
// Unit Tests: XML
//
// Description:
//

#include "XML_Tests.hpp"

using namespace XML_Lib;

TEST_CASE("Check the parsing of XML containing program instructions", "[XML][Parse][PI]")
{
  std::string xmlString;
  SECTION("Parse XML containing PI after declaration", "[XML][Parse][PI]")
  {
    xmlString =
      "<?xml version=\"1.0\" encoding=\"UTF-16\" standalone=\"yes\"?>\n"
      "<?xml-stylesheet href=\"tutorialspointstyle.css\" type=\"text/css\"?>\n"
      "<root></root>\n";
    BufferSource source{ xmlString };
    XML xml;
    REQUIRE_NOTHROW(xml.parse(source));
  }
  SECTION("Parse XML containing multiple PI after declaration", "[XML][Parse][PI]")
  {
    xmlString =
      "<?xml version=\"1.0\" encoding=\"UTF-16\" standalone=\"yes\"?>\n"
      "<?xml-stylesheet href=\"tutorialspointstyle.css\" type=\"text/css\"?> "
      "<?display table-view?>\n"
      "<root></root>\n";
    BufferSource source{ xmlString };
    XML xml;
    REQUIRE_NOTHROW(xml.parse(source));
  }
  SECTION("Parse XML containing PI in root section", "[XML][Parse][PI]")
  {
    xmlString =
      "<?xml version=\"1.0\" encoding=\"UTF-16\" standalone=\"yes\"?>\n"
      "<root><?xml-stylesheet href=\"tutorialspointstyle.css\" type=\"text/css\"?>\n"
      "</root>\n";
    BufferSource source{ xmlString };
    XML xml;
    REQUIRE_NOTHROW(xml.parse(source));
  }
  SECTION("Parse XML containing PI after root section", "[XML][Parse][PI]")
  {
    xmlString =
      "<?xml version=\"1.0\" encoding=\"UTF-16\" standalone=\"yes\"?>\n"
      "<root></root>\n"
      "<?xml-stylesheet href=\"tutorialspointstyle.css\" type=\"text/css\"?>\n";
    BufferSource source{ xmlString };
    XML xml;
    REQUIRE_NOTHROW(xml.parse(source));
  }
  SECTION("Parse XML containing PI after declaration and check values", "[XML][Parse][PI]")
  {
    xmlString =
      "<?xml version=\"1.0\" encoding=\"UTF-16\" standalone=\"yes\"?>\n"
      "<?xml-stylesheet href=\"tutorialspointstyle.css\" type=\"text/css\"?>\n"
      "<root></root>\n";
    BufferSource source{ xmlString };
    XML xml;
    xml.parse(source);
    REQUIRE_FALSE(!xml.prolog().getChildren()[2].isPI());
    REQUIRE(XRef<XPI>(xml.prolog().getChildren()[2]).name() == "xml-stylesheet");
    REQUIRE(
      XRef<XPI>(xml.prolog().getChildren()[2]).parameters() == "href=\"tutorialspointstyle.css\" type=\"text/css\"");
  }
  SECTION("Parse XML containing PI in root section and check values", "[XML][Parse][PI]")
  {
    xmlString =
      "<?xml version=\"1.0\" encoding=\"UTF-16\" standalone=\"yes\"?>\n"
      "<root><?xml-stylesheet href=\"tutorialspointstyle.css\" type=\"text/css\"?>\n"
      "</root>\n";
    BufferSource source{ xmlString };
    XML xml;
    xml.parse(source);
    REQUIRE_FALSE(!xml.root().getChildren()[0].isPI());
    REQUIRE(XRef<XPI>(xml.root().getChildren()[0]).name() == "xml-stylesheet");
    REQUIRE(
      XRef<XPI>(xml.root().getChildren()[0]).parameters() == "href=\"tutorialspointstyle.css\" type=\"text/css\"");
  }
}