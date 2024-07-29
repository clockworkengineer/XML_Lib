#include "XML_Lib_Tests.hpp"

TEST_CASE("Stringify XML with DTD_Validator both internal and external", "[XML][DTD_Validator][Stringify]")
{
  SECTION("Stringify XML with internal DTD_Validator", "[XML][DTD_Validator][Stringify]")
  {
    checkStringify(
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
      "<!DOCTYPE note ["
      "<!ELEMENT note (to,from,heading,body)>\n"
      "<!ELEMENT to (#PCDATA)>\n"
      "<!ELEMENT from (#PCDATA)>\n"
      "<!ELEMENT heading (#PCDATA)>\n"
      "<!ELEMENT body (#PCDATA)>\n"
      "]>\n"
      "<note>\n"
      "<to>Tove</to><from>Jani</from><heading>Reminder</heading>\n"
      "<body>Don't forget me this weekend</body>\n"
      "</note>\n");
  }
  SECTION("Stringify XML with external (SYSTEM) DTD_Validator", "[XML][DTD_Validator][Stringify]")
  {
    checkStringify(
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
      "<!DOCTYPE note SYSTEM \"./files/note001.dtd\">\n"
      "<note>\n"
      "<to>Tove</to><from>Jani</from><heading>Reminder</heading>\n"
      "<body>Don't forget me this weekend!</body>\n"
      "</note>\n");
  }
  SECTION("Stringify XML with external (PUBLIC) DTD_Validator", "[XML][DTD_Validator][Stringify]")
  {
    checkStringify(
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
      "<!DOCTYPE note PUBLIC \"-//W3C//DTD_Validator XHTML 1.0 Transitional//EN\" "
      "\"http://www.w3.org/TR/xhtml1/DTD_Validator/xhtml1-transitional.dtd\">\n"
      "<note>\n"
      "<to>Tove</to><from>Jani</from><heading>Reminder</heading>\n"
      "<body>Don't forget me this weekend!</body>\n"
      "</note>\n");
  }
  SECTION("Stringify XML with DTD_Validator with entity definitions and uses", "[XML][DTD_Validator][Stringify]")
  {
    checkStringify(
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
      "<!DOCTYPE note [\n"
      "<!ENTITY nbsp \"&#xA0;\">\n"
      "<!ENTITY writer \"Writer: Donald Duck.\">\n"
      "<!ENTITY copyright \"Copyright: W3Schools.\">]>\n"
      "<note>\n"
      "<to>Tove</to><from>Jani</from><heading>Reminder</heading>\n"
      "<body>Don't forget me this weekend!</body><footer>&writer;&nbsp;&copyright;</footer>\n"
      "</note>\n");
  }
  SECTION("Stringify XML with external file DTD_Validator.", "[XML][DTD_Validator][Stringify]")
  {
    checkStringify(
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
      "<!DOCTYPE note SYSTEM \"./files/note001.dtd\">\n"
      "<note>\n"
      "<to>Tove"
      "</to><from>Jani</from><heading>Reminder</heading><body>Don't forget me this weekend!</body>\n"
      "</note>\n");
  }
  SECTION("Stringify XML with external URL DTD_Validator.", "[XML][DTD_Validator][Stringify]")
  {
    checkStringify(
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
      "<!DOCTYPE html PUBLIC \"-//W3C//DTD_Validator XHTML 1.0 Transitional//EN\""
      " \"http://www.w3.org/TR/xhtml1/DTD_Validator/xhtml1-transitional.dtd\">\n"
      "<html></html>\n");
  }
  SECTION("Stringify XML with internal DTD_Validator with attributes.", "[XML][DTD_Validator][Stringify]")
  {
    checkStringify(
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
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
      "<!ATTLIST CHANNEL CHAN CDATA #REQUIRED>\n"
      "<!ATTLIST PROGRAMSLOT VTR CDATA #IMPLIED>\n"
      "<!ATTLIST TITLE RATING CDATA #IMPLIED>\n"
      "<!ATTLIST TITLE LANGUAGE CDATA #IMPLIED>]>\n"
      "<TVSCHEDULE></TVSCHEDULE>\n");
  }
  SECTION("Stringify XML with internal DTD_Validator containing comments.", "[XML][DTD_Validator][Stringify]")
  {
    checkStringify(
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
      "<!DOCTYPE note ["
      "<!-- root element is note that contains to, from, heading and body elements -->\n"
      "<!ELEMENT note (to,from,heading,body)>\n"
      "<!-- Note to field -->\n"
      "<!ELEMENT to (#PCDATA)>\n"
      "<!-- Note from field -->\n"
      "<!ELEMENT from (#PCDATA)>\n"
      "<!-- Note heading field -->\n"
      "<!ELEMENT heading (#PCDATA)>\n"
      "<!-- Note boy field -->\n"
      "<!ELEMENT body (#PCDATA)>\n"
      "]>\n"
      "<note>\n"
      "<to>Tove</to><from>Jani</from><heading>Reminder</heading>\n"
      "<body>Don't forget me this weekend</body>\n"
      "</note>\n");
  }
}