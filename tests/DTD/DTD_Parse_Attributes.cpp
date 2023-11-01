//
// Unit Tests: XML
//
// Description: Unit tests for DTD parsing.
//

#include "XML_Tests.hpp"

using namespace XML_Lib;

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
    XDTD &xDTD = XRef<XDTD>(xml.dtd());
    REQUIRE(XRef<XNode>(*xml.prolog().getChildren()[2]).getType() == XNode::Type::dtd);
    REQUIRE(xDTD.getType() == XDTD::Type::internal);
    REQUIRE(xDTD.getRootName() == XRef<XElement>(XRef<XElement>(xml.root())).name());
    REQUIRE(xDTD.getRootName() == "TVSCHEDULE");
    REQUIRE(xDTD.getElement("TVSCHEDULE").name == "TVSCHEDULE");
    REQUIRE(xDTD.getElement("CHANNEL").name == "CHANNEL");
    REQUIRE(xDTD.getElement("BANNER").name == "BANNER");
    REQUIRE(xDTD.getElement("DAY").name == "DAY");
    REQUIRE(xDTD.getElement("HOLIDAY").name == "HOLIDAY");
    REQUIRE(xDTD.getElement("DATE").name == "DATE");
    REQUIRE(xDTD.getElement("PROGRAMSLOT").name == "PROGRAMSLOT");
    REQUIRE(xDTD.getElement("TIME").name == "TIME");
    REQUIRE(xDTD.getElement("TITLE").name == "TITLE");
    REQUIRE(xDTD.getElement("DESCRIPTION").name == "DESCRIPTION");
    REQUIRE(xDTD.getElement("TVSCHEDULE").attributes.size() == 1);
    REQUIRE(xDTD.getElement("CHANNEL").attributes.size() == 1);
    REQUIRE(xDTD.getElement("PROGRAMSLOT").attributes.size() == 1);
    REQUIRE(xDTD.getElement("TITLE").attributes.size() == 2);
    REQUIRE(xDTD.getElement("TVSCHEDULE").attributes[0].name == "NAME");
    REQUIRE(xDTD.getElement("CHANNEL").attributes[0].name == "CHAN");
    REQUIRE(xDTD.getElement("PROGRAMSLOT").attributes[0].name == "VTR");
    REQUIRE(xDTD.getElement("TITLE").attributes[0].name == "RATING");
    REQUIRE(xDTD.getElement("TITLE").attributes[1].name == "LANGUAGE");
    REQUIRE(
      xDTD.getElement("TVSCHEDULE").attributes[0].type == (XDTD::AttributeType::cdata | XDTD::AttributeType::required));
    REQUIRE(
      xDTD.getElement("CHANNEL").attributes[0].type == (XDTD::AttributeType::cdata | XDTD::AttributeType::required));
    REQUIRE(
      xDTD.getElement("PROGRAMSLOT").attributes[0].type == (XDTD::AttributeType::cdata | XDTD::AttributeType::implied));
    REQUIRE(xDTD.getElement("TITLE").attributes[0].type == (XDTD::AttributeType::cdata | XDTD::AttributeType::implied));
    REQUIRE(xDTD.getElement("TITLE").attributes[1].type == (XDTD::AttributeType::cdata | XDTD::AttributeType::implied));
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
    XDTD &xDTD = XRef<XDTD>(xml.dtd());
    REQUIRE(XRef<XNode>(*xml.prolog().getChildren()[1]).getType() == XNode::Type::dtd);
    REQUIRE(xDTD.getType() == XDTD::Type::internal);
    REQUIRE(xDTD.getRootName() == XRef<XElement>(XRef<XElement>(xml.root())).name());
    REQUIRE(xDTD.getRootName() == "CATALOG");
    REQUIRE(xDTD.getElement("PRODUCT").name == "PRODUCT");
    REQUIRE(xDTD.getElement("PRODUCT").attributes.size() == 5);
    REQUIRE(xDTD.getElement("PRODUCT").attributes[0].name == "NAME");
    REQUIRE(
      xDTD.getElement("PRODUCT").attributes[0].type == (XDTD::AttributeType::cdata | XDTD::AttributeType::implied));
    REQUIRE(xDTD.getElement("PRODUCT").attributes[1].name == "CATEGORY");
    REQUIRE(xDTD.getElement("PRODUCT").attributes[1].type
            == (XDTD::AttributeType::enumeration | XDTD::AttributeType::normal));
    REQUIRE(xDTD.getElement("PRODUCT").attributes[1].enumeration == "(HandTool|Table|Shop-Professional)");
    REQUIRE(xDTD.getElement("PRODUCT").attributes[1].value.getParsed() == "HandTool");
    REQUIRE(xDTD.getElement("PRODUCT").attributes[2].name == "PARTNUM");
    REQUIRE(
      xDTD.getElement("PRODUCT").attributes[2].type == (XDTD::AttributeType::cdata | XDTD::AttributeType::implied));
    REQUIRE(xDTD.getElement("PRODUCT").attributes[3].name == "PLANT");
    REQUIRE(xDTD.getElement("PRODUCT").attributes[3].type
            == (XDTD::AttributeType::enumeration | XDTD::AttributeType::normal));
    REQUIRE(xDTD.getElement("PRODUCT").attributes[3].enumeration == "(Pittsburgh|Milwaukee|Chicago)");
    REQUIRE(xDTD.getElement("PRODUCT").attributes[3].value.getParsed() == "Chicago");
    REQUIRE(xDTD.getElement("PRODUCT").attributes[4].name == "INVENTORY");
    REQUIRE(xDTD.getElement("PRODUCT").attributes[4].type
            == (XDTD::AttributeType::enumeration | XDTD::AttributeType::normal));
    REQUIRE(xDTD.getElement("PRODUCT").attributes[4].enumeration == "(InStock|Backordered|Discontinued)");
    REQUIRE(xDTD.getElement("PRODUCT").attributes[4].value.getParsed() == "InStock");
    REQUIRE(xDTD.getElement("NOTES").name == "NOTES");
    REQUIRE(xDTD.getElement("NOTES").content.getUnparsed() == "(#PCDATA)");
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
    XDTD &xDTD = XRef<XDTD>(xml.dtd());
    REQUIRE(XRef<XNode>(*xml.prolog().getChildren()[2]).getType() == XNode::Type::dtd);
    REQUIRE(xDTD.getType() == XDTD::Type::internal);
    REQUIRE(xDTD.isElementPresent("person") == true);
    REQUIRE(xDTD.getElement("person").attributes.size() == 1);
    REQUIRE(xDTD.getElement("person").attributes[0].name == "gender");
    REQUIRE(
      xDTD.getElement("person").attributes[0].type == (XDTD::AttributeType::enumeration | XDTD::AttributeType::normal));
    REQUIRE(xDTD.getElement("person").attributes[0].enumeration == "(M|F)");
    REQUIRE(xDTD.getElement("person").attributes[0].value.getParsed() == "F");
    REQUIRE(xDTD.getRootName() == "queue");
    REQUIRE(xDTD.getRootName() == XRef<XElement>(XRef<XElement>(xml.root())).name());
    REQUIRE(XRef<XElement>(xml.root())[0].name() == "person");
    REQUIRE(XRef<XElement>(xml.root())[0].getAttributeList().size() == 1);
    XMLAttribute attribute = XRef<XElement>(xml.root())[0].getAttribute("gender");
    REQUIRE(attribute.getName() == "gender");
    REQUIRE(attribute.getValue() == "M");
    REQUIRE(XRef<XElement>(xml.root())[1].name() == "person");
    REQUIRE(XRef<XElement>(xml.root())[1].getAttributeList().size() == 0);
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
    XDTD &xDTD = XRef<XDTD>(xml.dtd());
    REQUIRE(XRef<XNode>(*xml.prolog().getChildren()[2]).getType() == XNode::Type::dtd);
    REQUIRE(xDTD.getType() == XDTD::Type::internal);
    REQUIRE(xDTD.isElementPresent("mountain") == true);
    REQUIRE(xDTD.getElement("mountain").attributes.size() == 2);
    REQUIRE(xDTD.getElement("mountain").attributes[0].name == "photo");
    REQUIRE(
      xDTD.getElement("mountain").attributes[0].type == (XDTD::AttributeType::entity | XDTD::AttributeType::implied));
    REQUIRE(xDTD.getElement("mountain").attributes[1].name == "photo_type");
    REQUIRE(
      xDTD.getElement("mountain").attributes[1].type == (XDTD::AttributeType::notation | XDTD::AttributeType::implied));
    REQUIRE(xDTD.getElement("mountain").attributes[1].enumeration == "(GIF|JPG|PNG)");
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