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
}