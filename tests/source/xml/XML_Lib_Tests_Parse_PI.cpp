#include "XML_Lib_Tests.hpp"

TEST_CASE("Check the parsing of XML containing program instructions", "[XML][Parse][PI]")
{
  XML xml;
  SECTION("Parse XML containing PI after declaration", "[XML][Parse][PI]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-16\" standalone=\"yes\"?>\n"
      "<?xml-stylesheet href=\"tutorialspointstyle.css\" type=\"text/css\"?>\n"
      "<root></root>\n"
    };
    REQUIRE_NOTHROW(xml.parse(source));
  }
  SECTION("Parse XML containing multiple PI after declaration", "[XML][Parse][PI]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-16\" standalone=\"yes\"?>\n"
      "<?xml-stylesheet href=\"tutorialspointstyle.css\" type=\"text/css\"?> "
      "<?display table-view?>\n"
      "<root></root>\n"
    };
    REQUIRE_NOTHROW(xml.parse(source));
  }
  SECTION("Parse XML containing PI in root section", "[XML][Parse][PI]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-16\" standalone=\"yes\"?>\n"
      "<root><?xml-stylesheet href=\"tutorialspointstyle.css\" type=\"text/css\"?>\n"
      "</root>\n"
    };
    REQUIRE_NOTHROW(xml.parse(source));
  }
  SECTION("Parse XML containing PI after root section", "[XML][Parse][PI]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-16\" standalone=\"yes\"?>\n"
      "<root></root>\n"
      "<?xml-stylesheet href=\"tutorialspointstyle.css\" type=\"text/css\"?>\n"
    };
    REQUIRE_NOTHROW(xml.parse(source));
  }
  SECTION("Parse XML containing PI after declaration and check values", "[XML][Parse][PI]")
  {

    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-16\" standalone=\"yes\"?>\n"
      "<?xml-stylesheet href=\"tutorialspointstyle.css\" type=\"text/css\"?>\n"
      "<root></root>\n"
    };
    xml.parse(source);
    REQUIRE_FALSE(!isA<PI>(xml.prolog().getChildren()[2]));
    REQUIRE(NRef<PI>(xml.prolog().getChildren()[2]).name() == "xml-stylesheet");
    REQUIRE(
      NRef<PI>(xml.prolog().getChildren()[2]).parameters() == "href=\"tutorialspointstyle.css\" type=\"text/css\"");
  }
  SECTION("Parse XML containing PI in root section and check values", "[XML][Parse][PI]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-16\" standalone=\"yes\"?>\n"
      "<root><?xml-stylesheet href=\"tutorialspointstyle.css\" type=\"text/css\"?>\n"
      "</root>\n"
    };
    xml.parse(source);
    auto &xRootChildren = xml.root().getChildren();
    REQUIRE_FALSE(!isA<PI>(xRootChildren[0]));
    REQUIRE(NRef<PI>(xRootChildren[0]).name() == "xml-stylesheet");
    REQUIRE(NRef<PI>(xRootChildren[0]).parameters() == "href=\"tutorialspointstyle.css\" type=\"text/css\"");
  }
  SECTION("Parse PI with empty name and parameters", "[XML][Parse][PI]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<? ?>\n"
      "<root></root>\n"
    };
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error [Line: 2 Column: 7] Invalid name '' encountered.");
  }
  SECTION("Parse PI with special characters in name and parameters", "[XML][Parse][PI]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<?xml-stylesheet-π type='text/xsl' href='style.xsl' & π?>\n"
      "<root></root>\n"
    };
    REQUIRE_NOTHROW(xml.parse(source));
  }
  SECTION("Parse malformed PI missing closing", "[XML][Parse][PI]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<?xml-stylesheet type='text/xsl' href='style.xsl'\n"
      "<root></root>\n"
    };
    REQUIRE_THROWS(xml.parse(source));
  }
  SECTION("Parse PI at document start and end", "[XML][Parse][PI]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<?display start?>\n"
      "<root></root>\n"
      "<?display end?>\n"
    };
    REQUIRE_NOTHROW(xml.parse(source));
  }
  SECTION("Parse PI with long parameters", "[XML][Parse][PI]")
  {
    std::string longParams(1000, 'a');
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<?xml-stylesheet " + longParams + "?>\n"
      "<root></root>\n"
    };
    REQUIRE_NOTHROW(xml.parse(source));
  }
}