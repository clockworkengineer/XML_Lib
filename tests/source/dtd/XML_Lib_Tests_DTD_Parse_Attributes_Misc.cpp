#include "XML_Lib_Tests.hpp"

TEST_CASE("Parse XML DTD that contains enumeration attributes.", "[XML][DTD][Parse][Attributes]")
{
  XML xml;
  SECTION("Parse XML with DTD that contains a enumeration attribute gender with a default value if 'F'.",
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
      "<!ATTLIST person gender ( M | F ) \"F\">\n"
      "]>\n"
      "<queue>\n"
      "<person "
      "gender=\"M\"><firstName>Andrew</firstName><lastName>Robinson</lastName><nationality>english</nationality></"
      "person>\n"
      "<person><firstName>Jane</firstName><lastName>Smith</lastName><nationality>english</nationality></person>\n"
      "</queue>\n"
    };
    xml.parse(source);
    DTD &xDTD = XRef<DTD>(xml.dtd());
    REQUIRE_FALSE(!isA<DTD>(xml.prolog().getChildren()[2]));
    REQUIRE(xDTD.getType() == DTD::Type::internal);
    REQUIRE(xDTD.isElementPresent("person") == true);
    REQUIRE(xDTD.getElement("person").attributes.size() == 1);
    REQUIRE(xDTD.getElement("person").attributes[0].name == "gender");
    REQUIRE(
      xDTD.getElement("person").attributes[0].type == (DTD::AttributeType::enumeration | DTD::AttributeType::normal));
    REQUIRE(xDTD.getElement("person").attributes[0].enumeration == "(M|F)");
    REQUIRE(xDTD.getElement("person").attributes[0].value.getParsed() == "F");
    REQUIRE(xDTD.getRootName() == "queue");
    REQUIRE(xDTD.getRootName() == XRef<Element>(xml.root()).name());
    REQUIRE(XRef<Element>(xml.root())[0].name() == "person");
    REQUIRE(XRef<Element>(xml.root())[0].getAttributes().size() == 1);
    XMLAttribute attribute = XRef<Element>(xml.root())[0]["gender"];
    REQUIRE(attribute.getName() == "gender");
    REQUIRE(attribute.getParsed() == "M");
    REQUIRE(XRef<Element>(xml.root())[1].name() == "person");
    REQUIRE(XRef<Element>(xml.root())[1].getAttributes().size() == 0);
  }
  SECTION("Parse XML with DTD that has a valid NOTATION attribute (photo_type) and usage.",
    "[XML][DTD][Parse][Attributes][NOTATION]")
  {
    BufferSource source{
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
      "</mountains>\n"
    };
    REQUIRE_NOTHROW(xml.parse(source));
    DTD &xDTD = XRef<DTD>(xml.dtd());
    REQUIRE_FALSE(!isA<DTD>(xml.prolog().getChildren()[2]));
    REQUIRE(xDTD.getType() == DTD::Type::internal);
    REQUIRE(xDTD.isElementPresent("mountain") == true);
    REQUIRE(xDTD.getElement("mountain").attributes.size() == 2);
    REQUIRE(xDTD.getElement("mountain").attributes[0].name == "photo");
    REQUIRE(
      xDTD.getElement("mountain").attributes[0].type == (DTD::AttributeType::entity | DTD::AttributeType::implied));
    REQUIRE(xDTD.getElement("mountain").attributes[1].name == "photo_type");
    REQUIRE(
      xDTD.getElement("mountain").attributes[1].type == (DTD::AttributeType::notation | DTD::AttributeType::implied));
    REQUIRE(xDTD.getElement("mountain").attributes[1].enumeration == "(GIF|JPG|PNG)");
  }
}