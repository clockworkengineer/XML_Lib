#include "XML_Lib_Tests.hpp"

TEST_CASE("Parse XML DTD with attributes and check values.", "[XML][DTD][Parse][Attributes]")
{
  XML xml;
  SECTION("XML with internal DTD with attributes to parse ", "[XML][DTD][Parse][Attributes]")
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
      "<!ATTLIST CHANNEL CHAN CDATA #REQUIRED>\n"
      "<!ATTLIST PROGRAMSLOT VTR CDATA #IMPLIED>\n"
      "<!ATTLIST TITLE RATING CDATA #IMPLIED>\n"
      "<!ATTLIST TITLE LANGUAGE CDATA #IMPLIED>]>\n"
      "<TVSCHEDULE></TVSCHEDULE>\n"
    };
    REQUIRE_NOTHROW(xml.parse(source));
  }
  SECTION("XML with internal DTD with attributes to parse and check values", "[XML][DTD][Parse][Attributes]")
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
      "<!ATTLIST CHANNEL CHAN CDATA #REQUIRED>\n"
      "<!ATTLIST PROGRAMSLOT VTR CDATA #IMPLIED>\n"
      "<!ATTLIST TITLE RATING CDATA #IMPLIED>\n"
      "<!ATTLIST TITLE LANGUAGE CDATA #IMPLIED>]>\n"
      "<TVSCHEDULE></TVSCHEDULE>\n"
    };
    xml.parse(source);
    DTD &xDTD = XRef<DTD>(xml.dtd());
    REQUIRE_FALSE(!isA<DTD>(xml.prolog().getChildren()[2]));
    REQUIRE(xDTD.getType() == DTD::Type::internal);
    REQUIRE(xDTD.getRootName() == XRef<Element>(xml.root()).name());
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
      xDTD.getElement("TVSCHEDULE").attributes[0].type == (DTD::AttributeType::cdata | DTD::AttributeType::required));
    REQUIRE(
      xDTD.getElement("CHANNEL").attributes[0].type == (DTD::AttributeType::cdata | DTD::AttributeType::required));
    REQUIRE(
      xDTD.getElement("PROGRAMSLOT").attributes[0].type == (DTD::AttributeType::cdata | DTD::AttributeType::implied));
    REQUIRE(xDTD.getElement("TITLE").attributes[0].type == (DTD::AttributeType::cdata | DTD::AttributeType::implied));
    REQUIRE(xDTD.getElement("TITLE").attributes[1].type == (DTD::AttributeType::cdata | DTD::AttributeType::implied));
  }
  SECTION("XML with internal DTD with elements with multiple attributes to parse and check values",
    "[XML][DTD][Parse][Attributes]")
  {
    BufferSource source{
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
      "<CATALOG> </CATALOG>\n"
    };
    xml.parse(source);
    DTD &xDTD = XRef<DTD>(xml.dtd());
    REQUIRE_FALSE(!isA<DTD>(xml.prolog().getChildren()[1]));
    REQUIRE(xDTD.getType() == DTD::Type::internal);
    REQUIRE(xDTD.getRootName() == XRef<Element>(xml.root()).name());
    REQUIRE(xDTD.getRootName() == "CATALOG");
    REQUIRE(xDTD.getElement("PRODUCT").name == "PRODUCT");
    REQUIRE(xDTD.getElement("PRODUCT").attributes.size() == 5);
    REQUIRE(xDTD.getElement("PRODUCT").attributes[0].name == "NAME");
    REQUIRE(
      xDTD.getElement("PRODUCT").attributes[0].type == (DTD::AttributeType::cdata | DTD::AttributeType::implied));
    REQUIRE(xDTD.getElement("PRODUCT").attributes[1].name == "CATEGORY");
    REQUIRE(xDTD.getElement("PRODUCT").attributes[1].type
            == (DTD::AttributeType::enumeration | DTD::AttributeType::normal));
    REQUIRE(xDTD.getElement("PRODUCT").attributes[1].enumeration == "(HandTool|Table|Shop-Professional)");
    REQUIRE(xDTD.getElement("PRODUCT").attributes[1].value.getParsed() == "HandTool");
    REQUIRE(xDTD.getElement("PRODUCT").attributes[2].name == "PARTNUM");
    REQUIRE(
      xDTD.getElement("PRODUCT").attributes[2].type == (DTD::AttributeType::cdata | DTD::AttributeType::implied));
    REQUIRE(xDTD.getElement("PRODUCT").attributes[3].name == "PLANT");
    REQUIRE(xDTD.getElement("PRODUCT").attributes[3].type
            == (DTD::AttributeType::enumeration | DTD::AttributeType::normal));
    REQUIRE(xDTD.getElement("PRODUCT").attributes[3].enumeration == "(Pittsburgh|Milwaukee|Chicago)");
    REQUIRE(xDTD.getElement("PRODUCT").attributes[3].value.getParsed() == "Chicago");
    REQUIRE(xDTD.getElement("PRODUCT").attributes[4].name == "INVENTORY");
    REQUIRE(xDTD.getElement("PRODUCT").attributes[4].type
            == (DTD::AttributeType::enumeration | DTD::AttributeType::normal));
    REQUIRE(xDTD.getElement("PRODUCT").attributes[4].enumeration == "(InStock|Backordered|Discontinued)");
    REQUIRE(xDTD.getElement("PRODUCT").attributes[4].value.getParsed() == "InStock");
    REQUIRE(xDTD.getElement("NOTES").name == "NOTES");
    REQUIRE(xDTD.getElement("NOTES").content.getUnparsed() == "(#PCDATA)");
  }
}