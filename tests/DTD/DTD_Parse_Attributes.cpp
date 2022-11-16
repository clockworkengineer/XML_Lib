//
// Unit Tests: XML
//
// Description: Unit tests for DTD parsing.
//
// ================
// Test definitions
// =================
#include "XML_Tests.hpp"
// =======================
// JSON class namespace
// =======================
using namespace XML_Lib;
// ==========
// Test cases
// ==========
TEST_CASE("Parse XML DTD with attributes and check values.", "[XML][DTD][Parse][Attributes]")
{
  std::string xmlString;
  SECTION("XML with internal DTD with attributes to parse ", "[XML][DTD][Parse][Attributes]")
  {
    xmlString =
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
      "<!ATTLIST CHANNEL CHAN CDATA #REQUIRED>\n"
      "<!ATTLIST PROGRAMSLOT VTR CDATA #IMPLIED>\n"
      "<!ATTLIST TITLE RATING CDATA #IMPLIED>\n"
      "<!ATTLIST TITLE LANGUAGE CDATA #IMPLIED>]>\n"
      "<TVSCHEDULE></TVSCHEDULE>\n";
    BufferSource source{ xmlString };
    XML xml;
    REQUIRE_NOTHROW(xml.parse(source));
  }
  SECTION("XML with internal DTD with attributes to parse and check values", "[XML][DTD][Parse][Attributes]")
  {
    xmlString =
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
      "<!ATTLIST CHANNEL CHAN CDATA #REQUIRED>\n"
      "<!ATTLIST PROGRAMSLOT VTR CDATA #IMPLIED>\n"
      "<!ATTLIST TITLE RATING CDATA #IMPLIED>\n"
      "<!ATTLIST TITLE LANGUAGE CDATA #IMPLIED>]>\n"
      "<TVSCHEDULE></TVSCHEDULE>\n";
    BufferSource source{ xmlString };
    XML xml;
    xml.parse(source);
    XMLNodeDTD &xmlNodeDTD = XMLNodeRef<XMLNodeDTD>(xml.dtd());
    REQUIRE(XMLNodeRef<XMLNode>(*xml.prolog().children[2]).getNodeType() == XMLNodeType::dtd);
    REQUIRE(xmlNodeDTD.getType() == XMLNodeDTD::Type::internal);
    REQUIRE(xmlNodeDTD.getRootName() == XMLNodeRef<XMLNodeElement>(XMLNodeRef<XMLNodeElement>(xml.root())).name());
    REQUIRE(xmlNodeDTD.getRootName() == "TVSCHEDULE");
    REQUIRE(xmlNodeDTD.getElement("TVSCHEDULE").name == "TVSCHEDULE");
    REQUIRE(xmlNodeDTD.getElement("CHANNEL").name == "CHANNEL");
    REQUIRE(xmlNodeDTD.getElement("BANNER").name == "BANNER");
    REQUIRE(xmlNodeDTD.getElement("DAY").name == "DAY");
    REQUIRE(xmlNodeDTD.getElement("HOLIDAY").name == "HOLIDAY");
    REQUIRE(xmlNodeDTD.getElement("DATE").name == "DATE");
    REQUIRE(xmlNodeDTD.getElement("PROGRAMSLOT").name == "PROGRAMSLOT");
    REQUIRE(xmlNodeDTD.getElement("TIME").name == "TIME");
    REQUIRE(xmlNodeDTD.getElement("TITLE").name == "TITLE");
    REQUIRE(xmlNodeDTD.getElement("DESCRIPTION").name == "DESCRIPTION");
    REQUIRE(xmlNodeDTD.getElement("TVSCHEDULE").attributes.size() == 1);
    REQUIRE(xmlNodeDTD.getElement("CHANNEL").attributes.size() == 1);
    REQUIRE(xmlNodeDTD.getElement("PROGRAMSLOT").attributes.size() == 1);
    REQUIRE(xmlNodeDTD.getElement("TITLE").attributes.size() == 2);
    REQUIRE(xmlNodeDTD.getElement("TVSCHEDULE").attributes[0].name == "NAME");
    REQUIRE(xmlNodeDTD.getElement("CHANNEL").attributes[0].name == "CHAN");
    REQUIRE(xmlNodeDTD.getElement("PROGRAMSLOT").attributes[0].name == "VTR");
    REQUIRE(xmlNodeDTD.getElement("TITLE").attributes[0].name == "RATING");
    REQUIRE(xmlNodeDTD.getElement("TITLE").attributes[1].name == "LANGUAGE");
    REQUIRE(xmlNodeDTD.getElement("TVSCHEDULE").attributes[0].type
            == (XMLNodeDTD::AttributeType::cdata | XMLNodeDTD::AttributeType::required));
    REQUIRE(xmlNodeDTD.getElement("CHANNEL").attributes[0].type
            == (XMLNodeDTD::AttributeType::cdata | XMLNodeDTD::AttributeType::required));
    REQUIRE(xmlNodeDTD.getElement("PROGRAMSLOT").attributes[0].type
            == (XMLNodeDTD::AttributeType::cdata | XMLNodeDTD::AttributeType::implied));
    REQUIRE(xmlNodeDTD.getElement("TITLE").attributes[0].type
            == (XMLNodeDTD::AttributeType::cdata | XMLNodeDTD::AttributeType::implied));
    REQUIRE(xmlNodeDTD.getElement("TITLE").attributes[1].type
            == (XMLNodeDTD::AttributeType::cdata | XMLNodeDTD::AttributeType::implied));
  }
  SECTION("XML with internal DTD with elements with multiple attributes to parse and check values",
    "[XML][DTD][Parse][Attributes]")
  {
    xmlString =
      "<!DOCTYPE CATALOG [\n"
      "<!ENTITY AUTHOR \"John Doe\">\n"
      "<!ENTITY COMPANY \"JD Power Tools, Inc.\">\n"
      "<!ENTITY EMAIL \"jd@jd-tools.com\">\n"
      "<!ELEMENT CATALOG (PRODUCT+)>\n"
      "<!ELEMENT PRODUCT(SPECIFICATIONS+,OPTIONS?,PRICE+,NOTES?)>\n"
      "<!ATTLIST PRODUCT NAME CDATA #IMPLIED CATEGORY (HandTool|Table|Shop-Professional) \"HandTool\""
      "PARTNUM CDATA #IMPLIED PLANT (Pittsburgh|Milwaukee|Chicago) \"Chicago\" INVENTORY "
      "(InStock|Backordered|Discontinued)"
      " \"InStock\">\n"
      "<!ELEMENT SPECIFICATIONS (#PCDATA)>\n"
      "<!ATTLIST SPECIFICATIONS WEIGHT CDATA #IMPLIED POWER CDATA #IMPLIED>\n"
      "<!ELEMENT OPTIONS (#PCDATA)>\n"
      "<!ATTLIST OPTIONS FINISH (Metal|Polished|Matte) \"Matte\" ADAPTER (Included|Optional|NotApplicable)"
      " \"Included\" CASE (HardShell|Soft|NotApplicable) \"HardShell\">\n"
      "<!ELEMENT PRICE (#PCDATA)>\n"
      "<!ATTLIST PRICE MSRP CDATA #IMPLIED"
      "WHOLESALE CDATA #IMPLIED STREET CDATA #IMPLIED SHIPPING CDATA #IMPLIED>\n"
      "<!ELEMENT NOTES (#PCDATA)> ]>\n"
      "<CATALOG> </CATALOG>\n";
    BufferSource source{ xmlString };
    XML xml;
    xml.parse(source);
    XMLNodeDTD &xmlNodeDTD = XMLNodeRef<XMLNodeDTD>(xml.dtd());
    REQUIRE(XMLNodeRef<XMLNode>(*xml.prolog().children[1]).getNodeType() == XMLNodeType::dtd);
    REQUIRE(xmlNodeDTD.getType() == XMLNodeDTD::Type::internal);
    REQUIRE(xmlNodeDTD.getRootName() == XMLNodeRef<XMLNodeElement>(XMLNodeRef<XMLNodeElement>(xml.root())).name());
    REQUIRE(xmlNodeDTD.getRootName() == "CATALOG");
    REQUIRE(xmlNodeDTD.getElement("PRODUCT").name == "PRODUCT");
    REQUIRE(xmlNodeDTD.getElement("PRODUCT").attributes.size() == 5);
    REQUIRE(xmlNodeDTD.getElement("PRODUCT").attributes[0].name == "NAME");
    REQUIRE(xmlNodeDTD.getElement("PRODUCT").attributes[0].type
            == (XMLNodeDTD::AttributeType::cdata | XMLNodeDTD::AttributeType::implied));
    REQUIRE(xmlNodeDTD.getElement("PRODUCT").attributes[1].name == "CATEGORY");
    REQUIRE(xmlNodeDTD.getElement("PRODUCT").attributes[1].type
            == (XMLNodeDTD::AttributeType::enumeration | XMLNodeDTD::AttributeType::normal));
    REQUIRE(xmlNodeDTD.getElement("PRODUCT").attributes[1].enumeration == "(HandTool|Table|Shop-Professional)");
    REQUIRE(xmlNodeDTD.getElement("PRODUCT").attributes[1].value.parsed == "HandTool");
    REQUIRE(xmlNodeDTD.getElement("PRODUCT").attributes[2].name == "PARTNUM");
    REQUIRE(xmlNodeDTD.getElement("PRODUCT").attributes[2].type
            == (XMLNodeDTD::AttributeType::cdata | XMLNodeDTD::AttributeType::implied));
    REQUIRE(xmlNodeDTD.getElement("PRODUCT").attributes[3].name == "PLANT");
    REQUIRE(xmlNodeDTD.getElement("PRODUCT").attributes[3].type
            == (XMLNodeDTD::AttributeType::enumeration | XMLNodeDTD::AttributeType::normal));
    REQUIRE(xmlNodeDTD.getElement("PRODUCT").attributes[3].enumeration == "(Pittsburgh|Milwaukee|Chicago)");
    REQUIRE(xmlNodeDTD.getElement("PRODUCT").attributes[3].value.parsed == "Chicago");
    REQUIRE(xmlNodeDTD.getElement("PRODUCT").attributes[4].name == "INVENTORY");
    REQUIRE(xmlNodeDTD.getElement("PRODUCT").attributes[4].type
            == (XMLNodeDTD::AttributeType::enumeration | XMLNodeDTD::AttributeType::normal));
    REQUIRE(xmlNodeDTD.getElement("PRODUCT").attributes[4].enumeration == "(InStock|Backordered|Discontinued)");
    REQUIRE(xmlNodeDTD.getElement("PRODUCT").attributes[4].value.parsed == "InStock");
    REQUIRE(xmlNodeDTD.getElement("NOTES").name == "NOTES");
    REQUIRE(xmlNodeDTD.getElement("NOTES").content.unparsed == "(#PCDATA)");
  }
}
TEST_CASE("Parse XML DTD that contains enumeration attributes with various errors.",
  "[XML][DTD][Parse][Error][Attributes]")
{
  std::string xmlString;
  SECTION("Parse XML with DTD that contains a enumeration attribute gender with a default value if 'F'.",
    "[XML][DTD][Parse][Error][Attributes]")
  {
    xmlString =
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE queue ["
      "<!ELEMENT queue (person)+>\n"
      "<!ELEMENT person (firstName, lastName, nationality)>\n"
      "<!ELEMENT firstName (#PCDATA)>\n"
      "<!ELEMENT lastName (#PCDATA)>\n"
      "<!ELEMENT nationality (#PCDATA)>\n"
      "<!ATTLIST person gender ( M | F ) \"F\">\n"
      "]>\n"
      "<queue>\n"
      "<person "
      "gender=\"M\"><firstName>Andrew</firstName><lastName>Robinson</lastName><nationality>english</nationality></"
      "person>\n"
      "<person><firstName>Jane</firstName><lastName>Smith</lastName><nationality>english</nationality></person>\n"
      "</queue>\n";
    BufferSource source{ xmlString };
    XML xml;
    xml.parse(source);
    XMLNodeDTD &xmlNodeDTD = XMLNodeRef<XMLNodeDTD>(xml.dtd());
    REQUIRE(XMLNodeRef<XMLNode>(*xml.prolog().children[2]).getNodeType() == XMLNodeType::dtd);
    REQUIRE(xmlNodeDTD.getType() == XMLNodeDTD::Type::internal);
    REQUIRE(xmlNodeDTD.isElementPresent("person") == true);
    REQUIRE(xmlNodeDTD.getElement("person").attributes.size() == 1);
    REQUIRE(xmlNodeDTD.getElement("person").attributes[0].name == "gender");
    REQUIRE(xmlNodeDTD.getElement("person").attributes[0].type
            == (XMLNodeDTD::AttributeType::enumeration | XMLNodeDTD::AttributeType::normal));
    REQUIRE(xmlNodeDTD.getElement("person").attributes[0].enumeration == "(M|F)");
    REQUIRE(xmlNodeDTD.getElement("person").attributes[0].value.parsed == "F");
    REQUIRE(xmlNodeDTD.getRootName() == "queue");
    REQUIRE(xmlNodeDTD.getRootName() == XMLNodeRef<XMLNodeElement>(XMLNodeRef<XMLNodeElement>(xml.root())).name());
    REQUIRE(XMLNodeRef<XMLNodeElement>(xml.root())[0].name() == "person");
    REQUIRE(XMLNodeRef<XMLNodeElement>(xml.root())[0].getAttributeList().size() == 1);
    XMLAttribute attribute = XMLNodeRef<XMLNodeElement>(xml.root())[0].getAttribute("gender");
    REQUIRE(attribute.name == "gender");
    REQUIRE(attribute.value.parsed == "M");
    REQUIRE(XMLNodeRef<XMLNodeElement>(xml.root())[1].name() == "person");
    REQUIRE(XMLNodeRef<XMLNodeElement>(xml.root())[1].getAttributeList().size() == 0);
  }
  SECTION("Parse XML with DTD that contains a enumeration with a syntax error (missing enumeration name).",
    "[XML][DTD][Parse][Error][Attributes]")
  {
    xmlString =
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
      "</queue>\n";
    BufferSource source{ xmlString };
    XML xml;
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error [Line: 7 Column: 36] Invalid name '' encountered.");
  }
  SECTION("Parse XML with DTD that contains a enumeration with a syntax error (missing end bracket).",
    "[XML][DTD][Parse][Error][Attributes]")
  {
    xmlString =
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
      "</queue>\n";
    BufferSource source{ xmlString };
    XML xml;
    REQUIRE_THROWS_WITH(
      xml.parse(source), "XML Syntax Error [Line: 7 Column: 39] Missing closing ')' on enumeration attribute type.");
  }
  SECTION("Parse XML with DTD that contains a enumeration with a default value not in enumeration.",
    "[XML][DTD][Parse][Error][Attributes]")
  {
    xmlString =
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
      "</queue>\n";
    BufferSource source{ xmlString };
    XML xml;
    REQUIRE_THROWS_WITH(
      xml.parse(source), "XML Syntax Error: Default value 'D' for enumeration attribute 'gender' is invalid.");
  }
  SECTION("Parse XML with DTD that contains a enumeration with not all values unique.",
    "[XML][DTD][Parse][Error][Attributes]")
  {
    xmlString =
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
      "</queue>\n";
    BufferSource source{ xmlString };
    XML xml;
    REQUIRE_THROWS_WITH(xml.parse(source),
      "XML Syntax Error: Enumerator value 'F' for attribute 'gender' occurs more than once in its definition.");
  }
  SECTION("Parse XML with DTD that specifies the use of an two different ID attributes for an element.",
    "[XML][DTD][Parse][Error][Attributes]")
  {
    xmlString =
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
      "</collection>\n";
    BufferSource source{ xmlString };
    XML xml;
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error: Element <item> has more than one ID attribute.");
  }
  SECTION("Parse XML with DTD that has a valid NOTATION attribute (photo_type) and usage.",
    "[XML][DTD][Parse][Attributes][NOTATION]")
  {
    xmlString =
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE mountains [\n"
      "<!ELEMENT mountains (mountain)+>\n"
      "<!ELEMENT mountain (name)>\n"
      "<!ELEMENT name (#PCDATA)>\n"
      "<!NOTATION GIF SYSTEM \"image/gif\">\n"
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
      "</mountains>\n";
    BufferSource source{ xmlString };
    XML xml;
    REQUIRE_NOTHROW(xml.parse(source));
    XMLNodeDTD &xmlNodeDTD = XMLNodeRef<XMLNodeDTD>(xml.dtd());
    REQUIRE(XMLNodeRef<XMLNode>(*xml.prolog().children[2]).getNodeType() == XMLNodeType::dtd);
    REQUIRE(xmlNodeDTD.getType() == XMLNodeDTD::Type::internal);
    REQUIRE(xmlNodeDTD.isElementPresent("mountain") == true);
    REQUIRE(xmlNodeDTD.getElement("mountain").attributes.size() == 2);
    REQUIRE(xmlNodeDTD.getElement("mountain").attributes[0].name == "photo");
    REQUIRE(xmlNodeDTD.getElement("mountain").attributes[0].type
            == (XMLNodeDTD::AttributeType::entity | XMLNodeDTD::AttributeType::implied));
    REQUIRE(xmlNodeDTD.getElement("mountain").attributes[1].name == "photo_type");
    REQUIRE(xmlNodeDTD.getElement("mountain").attributes[1].type
            == (XMLNodeDTD::AttributeType::notation | XMLNodeDTD::AttributeType::implied));
    REQUIRE(xmlNodeDTD.getElement("mountain").attributes[1].enumeration == "(GIF|JPG|PNG)");
  }
  SECTION("Parse XML with DTD that has a missing NOTATION attribute (photo_type GIF) and usage.",
    "[XML][DTD][Parse][Attributes][NOTATION]")
  {
    xmlString =
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
      "</mountains>\n";
    BufferSource source{ xmlString };
    XML xml;
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error: NOTATION GIF is not defined.");
  }
}