#include "XML_Lib_Tests.hpp"

TEST_CASE("Parse XML DTD that contains enumeration attributes with various errors.",
  "[XML][DTD][Parse][Error][Attributes]")
{
  XML xml;
  SECTION("Parse XML with DTD that contains a enumeration with a syntax error (missing enumeration name).",
    "[XML][DTD][Parse][Error][Attributes]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE queue ["
      "<!ELEMENT queue (person)+>\n"
      "<!ELEMENT person (firstName, lastName, nationality)>\n"
      "<!ELEMENT firstName (#PCDATA)>\n"
      "<!ELEMENT lastName (#PCDATA)>\n"
      "<!ELEMENT nationality (#PCDATA)>\n"
      "<!ATTLIST person gender ( M | ) \"F\">\n"
      "]>\n"
      "<queue>\n"
      "<person "
      "gender=\"M\"><firstName>Andrew</firstName><lastName>Robinson</lastName><nationality>english</nationality></"
      "person>\n"
      "<person><firstName>Jane</firstName><lastName>Smith</lastName><nationality>english</nationality></person>\n"
      "</queue>\n"
    };
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error [Line: 7 Column: 36] Invalid name '' encountered.");
  }
  SECTION("Parse XML with DTD that contains a enumeration with a syntax error (missing end bracket).",
    "[XML][DTD][Parse][Error][Attributes]")
  {

    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE queue ["
      "<!ELEMENT queue (person)+>\n"
      "<!ELEMENT person (firstName, lastName, nationality)>\n"
      "<!ELEMENT firstName (#PCDATA)>\n"
      "<!ELEMENT lastName (#PCDATA)>\n"
      "<!ELEMENT nationality (#PCDATA)>\n"
      "<!ATTLIST person gender ( M | F  \"F\">\n"
      "]>\n"
      "<queue>\n"
      "<person "
      "gender=\"M\"><firstName>Andrew</firstName><lastName>Robinson</lastName><nationality>english</nationality></"
      "person>\n"
      "<person><firstName>Jane</firstName><lastName>Smith</lastName><nationality>english</nationality></person>\n"
      "</queue>\n"
    };
    REQUIRE_THROWS_WITH(
      xml.parse(source), "XML Syntax Error [Line: 7 Column: 39] Missing closing ')' on enumeration attribute type.");
  }
  SECTION("Parse XML with DTD that contains a enumeration with a default value not in enumeration.",
    "[XML][DTD][Parse][Error][Attributes]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE queue ["
      "<!ELEMENT queue (person)+>\n"
      "<!ELEMENT person (firstName, lastName, nationality)>\n"
      "<!ELEMENT firstName (#PCDATA)>\n"
      "<!ELEMENT lastName (#PCDATA)>\n"
      "<!ELEMENT nationality (#PCDATA)>\n"
      "<!ATTLIST person gender ( M | F ) \"D\">\n"
      "]>\n"
      "<queue>\n"
      "<person "
      "gender=\"M\"><firstName>Andrew</firstName><lastName>Robinson</lastName><nationality>english</nationality></"
      "person>\n"
      "<person><firstName>Jane</firstName><lastName>Smith</lastName><nationality>english</nationality></person>\n"
      "</queue>\n"
    };
    REQUIRE_THROWS_WITH(
      xml.parse(source), "XML Syntax Error: Default value 'D' for enumeration attribute 'gender' is invalid.");
  }
  SECTION("Parse XML with DTD that contains a enumeration with not all values unique.",
    "[XML][DTD][Parse][Error][Attributes]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE queue ["
      "<!ELEMENT queue (person)+>\n"
      "<!ELEMENT person (firstName, lastName, nationality)>\n"
      "<!ELEMENT firstName (#PCDATA)>\n"
      "<!ELEMENT lastName (#PCDATA)>\n"
      "<!ELEMENT nationality (#PCDATA)>\n"
      "<!ATTLIST person gender ( M | F | F) \"M\">\n"
      "]>\n"
      "<queue>\n"
      "<person "
      "gender=\"M\"><firstName>Andrew</firstName><lastName>Robinson</lastName><nationality>english</nationality></"
      "person>\n"
      "<person><firstName>Jane</firstName><lastName>Smith</lastName><nationality>english</nationality></person>\n"
      "</queue>\n"
    };
    REQUIRE_THROWS_WITH(xml.parse(source),
      "XML Syntax Error: Enumerator value 'F' for attribute 'gender' occurs more than once in its definition.");
  }
  SECTION("Parse XML with DTD that specifies the use of an two different ID attributes for an element.",
    "[XML][DTD][Parse][Error][Attributes]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE collection ["
      "<!ELEMENT collection (item)+>\n"
      "<!ELEMENT item (#PCDATA)>\n"
      "<!ATTLIST item itemID1 ID #REQUIRED >\n"
      "<!ATTLIST item itemID2 ID #REQUIRED >\n"
      "]>\n"
      "<collection>\n"
      "<item itemID1=\"i001\" itemID2=\"id001\">item description</item>\n"
      "<item itemID1=\"i002\" itemID2=\"id002\">item description</item>\n"
      "<item itemID1=\"i003\" itemID2=\"id003\">item description</item>\n"
      "<item itemID1=\"i004\" itemID2=\"id004\">item description</item>\n"
      "<item itemID1=\"i005\" itemID2=\"id005\">item description</item>\n"
      "</collection>\n"
    };
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error: Element <item> has more than one ID attribute.");
  }
  SECTION("Parse XML with DTD that has a missing NOTATION attribute (photo_type GIF) and usage.",
    "[XML][DTD][Parse][Error][Attributes]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE mountains [\n"
      "<!ELEMENT mountains (mountain)+>\n"
      "<!ELEMENT mountain (name)>\n"
      "<!ELEMENT name (#PCDATA)>\n"
      "<!NOTATION JPG SYSTEM \"image/jpeg\">\n"
      "<!NOTATION PNG SYSTEM \"image/png\">\n"
      "<!ATTLIST mountain photo ENTITY #IMPLIED\n"
      "photo_type NOTATION (GIF | JPG | PNG) #IMPLIED>\n"
      "<!ENTITY mt_cook_1 SYSTEM \"mt_cook1.jpg\">\n"
      "]>\n"
      "<mountains>\n"
      "<mountain photo=\"mt_cook_1\" photo_type=\"JPG\">\n"
      "<name>Mount Cook</name>\n"
      "</mountain>\n"
      "<mountain>\n"
      "<name>Cradle Mountain</name>\n"
      "</mountain>\n"
      "</mountains>\n"
    };
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error: NOTATION GIF is not defined.");
  }
}