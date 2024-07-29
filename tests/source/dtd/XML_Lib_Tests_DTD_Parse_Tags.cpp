#include "XML_Lib_Tests.hpp"

TEST_CASE("Parse XML DTD with missing terminating '>' on tags.", "[XML][DTD][Parse][Tags][Error]")
{
  XML xml;
  SECTION("XML with internal DTD with missing terminating '>' on !ELEMENT", "[XML][DTD][Parse]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE note ["
      "<!ELEMENT note (to,from,heading,body)>\n"
      "<!ELEMENT to (#PCDATA)\n"
      "<!ELEMENT from (#PCDATA)>\n"
      "<!ELEMENT heading (#PCDATA)>\n"
      "<!ELEMENT body (#PCDATA)>\n"
      "]>\n"
      "<note>\n"
      "<to>Tove</to><from>Jani</from><heading>Reminder</heading>\n"
      "<body>Don't forget me this weekend</body>\n"
      "</note>\n"
    };
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error [Line: 4 Column: 2] Missing '>' terminator.");
  }
  SECTION("XML with internal DTD with missing terminating '>' on !ATTLIST", "[XML][DTD][Parse][Tags][Error]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
      "<!DOCTYPE TVSCHEDULE [\n"
      "<!ELEMENT TVSCHEDULE (CHANNEL+)>\n"
      "<!ELEMENT CHANNEL (BANNER,DAY+)>\n"
      "<!ELEMENT BANNER (#PCDATA)>\n"
      "<!ELEMENT DAY (DATE,(HOLIDAY|PROGRAMSLOT+)+)>\n"
      "<!ELEMENT HOLIDAY (#PCDATA)>\n"
      "<!ELEMENT DATE (#PCDATA)>\n"
      "<!ELEMENT PROGRAMSLOT (TIME,TITLE,DESCRIPTION?)>\n"
      "<!ELEMENT TIME (#PCDATA)>\n"
      "<!ELEMENT TITLE (#PCDATA)>\n"
      "<!ELEMENT DESCRIPTION (#PCDATA)>\n"
      "<!ATTLIST TVSCHEDULE NAME CDATA #REQUIRED>\n"
      "<!ATTLIST CHANNEL CHAN CDATA #REQUIRED\n"
      "<!ATTLIST PROGRAMSLOT VTR CDATA #IMPLIED>\n"
      "<!ATTLIST TITLE RATING CDATA #IMPLIED>\n"
      "<!ATTLIST TITLE LANGUAGE CDATA #IMPLIED>]>\n"
      "<TVSCHEDULE></TVSCHEDULE>\n"
    };
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error [Line: 15 Column: 2] Missing '>' terminator.");
  }
  SECTION("XML with internal DTD with missing terminating '>' on !ENTITY", "[XML][DTD][Parse][Tags][Error]")
  {
    BufferSource source{
      "<!DOCTYPE REPORT [\n"
      "<!ELEMENT REPORT (paragraph)*>\n"
      "<!ELEMENT paragraph (#PCDATA)>\n"
      "<!ENTITY contact \"0893-456334534\" ]>\n"
      "<REPORT></REPORT>\n"
    };
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error [Line: 4 Column: 36] Missing '>' terminator.");
  }
  SECTION("XML with internal DTD with missing terminating '>' on !NOTATION", "[XML][DTD][Parse][Tags][Error]")
  {
    BufferSource source{
      "<!DOCTYPE REPORT [\n"
      "<!ELEMENT REPORT (paragraph)*>\n"
      "<!ELEMENT paragraph (#PCDATA)>\n"
      "<!ENTITY contact \"0893-456334534\">\n"
      "<!NOTATION GIF SYSTEM \"\"\n"
      "<!NOTATION JPG SYSTEM \"\">\n"
      "<!NOTATION BMP SYSTEM \"\">\n"
      "]>\n"
      "<REPORT></REPORT>\n"
    };
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error [Line: 6 Column: 2] Missing '>' terminator.");
  }
}