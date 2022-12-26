//
// Unit Tests: XML
//
// Description: Unit tests for XML DTD attribute validation.
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
TEST_CASE("Validate XML with various DTD attribute validation issues.", "[XML][DTD][Validate][Attributes]")
{
  std::string xmlString;
  SECTION("XML with a DTD that specifies a required attribute that is missing.", "[XML][DTD][Validate][Attributes]")
  {
    xmlString =
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE root [\n"
      "<!ELEMENT root (child1|child2|child3)+ >\n"
      "<!ELEMENT child1 (#PCDATA)>\n"
      "<!ELEMENT child2 (#PCDATA)>\n"
      "<!ELEMENT child3 (#PCDATA)>\n"
      "<!ATTLIST child2 number CDATA #REQUIRED>\n"
      "]>\n"
      "<root>\n"
      "<child1>contents 1</child1>\n"
      "<child2>contents 2</child2>\n"
      "<child3>contents 3</child3>\n"
      "</root>\n";
    BufferSource source{ xmlString };
    XML xml;
    xml.parse(source);
    REQUIRE_THROWS_WITH(
      xml.validate(), "XML Validation Error [Line: 11] Element <child2> is missing required attribute 'number'.");
  }
  SECTION("XML with a DTD that specifies a required attribute that is present.", "[XML][DTD][Validate][Attributes]")
  {
    xmlString =
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE root [\n"
      "<!ELEMENT root (child1|child2|child3)+ >\n"
      "<!ELEMENT child1 (#PCDATA)>\n"
      "<!ELEMENT child2 (#PCDATA)>\n"
      "<!ELEMENT child3 (#PCDATA)>\n"
      "<!ATTLIST child2 number CDATA #REQUIRED>\n"
      "]>\n"
      "<root>\n"
      "<child1>contents 1</child1>\n"
      "<child2 number=\"900000\">contents 2</child2>\n"
      "<child3>contents 3</child3>\n"
      "</root>\n";
    BufferSource source{ xmlString };
    XML xml;
    xml.parse(source);
    REQUIRE_NOTHROW(xml.validate());
  }
  SECTION("XML with a DTD that specifies a implied attribute that is present/not present in an element.",
    "[XML][DTD][Validate][Attributes]")
  {
    xmlString =
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE root [\n"
      "<!ELEMENT root (child1|child2|child3)+ >\n"
      "<!ELEMENT child1 (#PCDATA)>\n"
      "<!ELEMENT child2 (#PCDATA)>\n"
      "<!ELEMENT child3 (#PCDATA)>\n"
      "<!ATTLIST child2 number CDATA #IMPLIED>\n"
      "]>\n"
      "<root>\n"
      "<child1>contents 1</child1>\n"
      "<child2 number=\"900000\">contents 2</child2>\n"
      "<child2>contents 2</child2>\n"
      "<child3>contents 3</child3>\n"
      "</root>\n";
    BufferSource source{ xmlString };
    XML xml;
    xml.parse(source);
    REQUIRE_NOTHROW(xml.validate());
  }
  SECTION("XML with a DTD that specifies a fixed attribute trying to have it to more than one value.",
    "[XML][DTD][Validate][Attributes]")
  {
    xmlString =
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE root [\n"
      "<!ELEMENT root (child1|child2|child3)+ >\n"
      "<!ELEMENT child1 (#PCDATA)>\n"
      "<!ELEMENT child2 (#PCDATA)>\n"
      "<!ELEMENT child3 (#PCDATA)>\n"
      "<!ATTLIST child2 number CDATA #FIXED \"2001\">\n"
      "]>\n"
      "<root>\n"
      "<child1>contents 1</child1>\n"
      "<child2 number=\"2001\">contents 2</child2>\n"
      "<child2 number =\"2002\">contents 2</child2>\n"
      "<child3>contents 3</child3>\n"
      "</root>\n";
    BufferSource source{ xmlString };
    XML xml;
    xml.parse(source);
    REQUIRE_THROWS_WITH(xml.validate(),
      "XML Validation Error [Line: 12] Element <child2> attribute 'number' is '2002' instead of '2001'.");
  }
  SECTION(
    "XML with a DTD that specifies a fixed element attribute that has not been assigned in the data but should be "
    "there for the application. ",
    "[XML][DTD][Validate][Attributes]")
  {
    xmlString =
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE root [\n"
      "<!ELEMENT root (child1|child2|child3)+ >\n"
      "<!ELEMENT child1 (#PCDATA)>\n"
      "<!ELEMENT child2 (#PCDATA)>\n"
      "<!ELEMENT child3 (#PCDATA)>\n"
      "<!ATTLIST child2 number CDATA #FIXED \"2001\">\n"
      "]>\n"
      "<root>\n"
      "<child1>contents 1</child1>\n"
      "<child2 number=\"2001\">contents 2</child2>\n"
      "<child2>contents 2</child2>\n"
      "<child3>contents 3</child3>\n"
      "</root>\n";
    BufferSource source{ xmlString };
    XML xml;
    xml.parse(source);
    REQUIRE_NOTHROW(xml.validate());
    REQUIRE(XRef<XElement>(xml.root()).getType() == XNode::Type::root);
    REQUIRE(XRef<XElement>(xml.root())[1].getAttributeList().size() == 1);
    REQUIRE(XRef<XElement>(xml.root())[1].getAttribute("number").getValue() == "2001");
    REQUIRE(XRef<XElement>(xml.root())[2].getAttributeList().size() == 1);
    REQUIRE(XRef<XElement>(xml.root())[2].getAttribute("number").getValue() == "2001");
  }
  SECTION("XML with a DTD that specifies an element attribute that has a default value if it is not defined. ",
    "[XML][DTD][Validate][Attributes]")
  {
    xmlString =
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE root [\n"
      "<!ELEMENT root (child1|child2|child3)+ >\n"
      "<!ELEMENT child1 (#PCDATA)>\n"
      "<!ELEMENT child2 (#PCDATA)>\n"
      "<!ELEMENT child3 (#PCDATA)>\n"
      "<!ATTLIST child2 number CDATA \"2001\">\n"
      "]>\n"
      "<root>\n"
      "<child1>contents 1</child1>\n"
      "<child2 number=\"2002\">contents 2</child2>\n"
      "<child2>contents 2</child2>\n"
      "<child3>contents 3</child3>\n"
      "</root>\n";
    BufferSource source{ xmlString };
    XML xml;
    xml.parse(source);
    REQUIRE_NOTHROW(xml.validate());
    REQUIRE(XRef<XElement>(xml.root()).getType() == XNode::Type::root);
    REQUIRE(XRef<XElement>(xml.root())[1].getAttributeList().size() == 1);
    REQUIRE(XRef<XElement>(xml.root())[1].getAttribute("number").getValue() == "2002");
    REQUIRE(XRef<XElement>(xml.root())[2].getAttributeList().size() == 1);
    REQUIRE(XRef<XElement>(xml.root())[2].getAttribute("number").getValue() == "2001");
  }
  SECTION("Validate XML with DTD that contains a enumeration attribute gender with a default value if 'F'.",
    "[XML][DTD][Validate][Attributes]")
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
    REQUIRE_NOTHROW(xml.validate());
    REQUIRE(XRef<XNode>(*xml.prolog().getChildren()[2]).getType() == XNode::Type::dtd);
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
    REQUIRE(XRef<XElement>(xml.root())[0].getAttribute("gender").getValue() == "M");
    REQUIRE(XRef<XElement>(xml.root())[1].name() == "person");
    REQUIRE(XRef<XElement>(xml.root())[1].getAttributeList().size() == 1);
    REQUIRE(XRef<XElement>(xml.root())[1].getAttribute("gender").getValue() == "F");
  }
  SECTION("Validate XML with DTD that contains a enumeration attribute gender that is not valid.",
    "[XML][DTD][Validate][Attributes]")
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
      "<person "
      "gender=\"B\"><firstName>Jane</firstName><lastName>Smith</lastName><nationality>english</nationality></person>\n"
      "</queue>\n";
    BufferSource source{ xmlString };
    XML xml;
    xml.parse(source);
    REQUIRE_THROWS_WITH(xml.validate(),
      "XML Validation Error [Line: 11] Element <person> attribute 'gender' contains invalid enumeration value 'B'.");
  }
  SECTION("Validate XML with DTD that specifies the use of an ID attribute type that has a duplicate value.",
    "[XML][DTD][Validate][Attributes]")
  {
    xmlString =
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE collection ["
      "<!ELEMENT collection (item)+>\n"
      "<!ELEMENT item (#PCDATA)>\n"
      "<!ATTLIST item itemID ID #REQUIRED >\n"
      "]>\n"
      "<collection>\n"
      "<item itemID=\"i001\">item descripton</item>\n"
      "<item itemID=\"i002\">item descripton</item>\n"
      "<item itemID=\"i003\">item descripton</item>\n"
      "<item itemID=\"i004\">item descripton</item>\n"
      "<item itemID=\"i001\">item descripton</item>\n"
      "</collection>\n";
    BufferSource source{ xmlString };
    XML xml;
    xml.parse(source);
    REQUIRE_THROWS_WITH(
      xml.validate(), "XML Validation Error [Line: 11] Element <item> ID attribute 'itemID' is not unique.");
  }
  SECTION(
    "Validate XML that has an element with an ID attribute value that is invalid.", "[XML][DTD][Validate][Attributes]")
  {
    xmlString =
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE collection ["
      "<!ELEMENT collection (item)+>\n"
      "<!ELEMENT item (#PCDATA)>\n"
      "<!ATTLIST item itemID ID #REQUIRED >\n"
      "]>\n"
      "<collection>\n"
      "<item itemID=\"i001\">item descripton</item>\n"
      "<item itemID=\"i002\">item descripton</item>\n"
      "<item itemID=\"i003\">item descripton</item>\n"
      "<item itemID=\"i004\">item descripton</item>\n"
      "<item itemID=\"005\">item descripton</item>\n"
      "<item itemID=\"i006\">item descripton</item>\n"
      "</collection>\n";
    BufferSource source{ xmlString };
    XML xml;
    xml.parse(source);
    REQUIRE_THROWS_WITH(
      xml.validate(), "XML Validation Error [Line: 11] Element <item> ID attribute 'itemID' is invalid.");
  }
  SECTION("Validate XML that has a missing ID referenced  by an IDREF.", "[XML][DTD][Validate][Attributes]")
  {
    xmlString =
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE collection ["
      "<!ELEMENT collection (item|itemOnLoan)+>\n"
      "<!ELEMENT item (#PCDATA)>\n"
      "<!ELEMENT itemOnLoan (#PCDATA)>\n"
      "<!ATTLIST item itemID ID #REQUIRED >\n"
      "<!ATTLIST itemOnLoan itemLoanedID IDREF #IMPLIED >\n"
      "]>\n"
      "<collection>\n"
      "<item itemID=\"i001\">item descripton</item>\n"
      "<item itemID=\"i002\">item descripton</item>\n"
      "<item itemID=\"i003\">item descripton</item>\n"
      "<item itemID=\"i004\">item descripton</item>\n"
      "<item itemID=\"i005\">item descripton</item>\n"
      "<item itemID=\"i006\">item descripton</item>\n"
      "<itemOnLoan itemLoanedID=\"i006\">reason for loan</itemOnLoan>\n"
      "<itemOnLoan itemLoanedID=\"i010\">reason for loan</itemOnLoan>\n"
      "</collection>\n";
    BufferSource source{ xmlString };
    XML xml;
    xml.parse(source);
    REQUIRE_THROWS_WITH(xml.validate(),
      "XML Validation Error [Line: 18] IDREF attribute 'i010' does not reference any element with the ID.");
  }
  SECTION("Validate XML that has a an IDREF attribute value that is invalid.", "[XML][DTD][Validate][Attributes]")
  {
    xmlString =
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE collection [\n"
      "<!ELEMENT collection (item|itemsOnLoan)+>\n"
      "<!ELEMENT item (#PCDATA)>\n"
      "<!ELEMENT itemsOnLoan (#PCDATA)>\n"
      "<!ATTLIST item itemID ID #REQUIRED >\n"
      "<!ATTLIST itemsOnLoan itemsLoanedIDs IDREFS #IMPLIED >\n"
      "]>\n"
      "<collection>\n"
      "<item itemID=\"i001\">item descripton</item>\n"
      "<item itemID=\"i002\">item descripton</item>\n"
      "<item itemID=\"i003\">item descripton</item>\n"
      "<item itemID=\"i004\">item descripton</item>\n"
      "<item itemID=\"i005\">item descripton</item>\n"
      "<item itemID=\"i006\">item descripton</item>\n"
      "<itemsOnLoan itemsLoanedIDs=\"i006 i003 005\">reason for loan</itemsOnLoan>\n"
      "</collection>\n";
    BufferSource source{ xmlString };
    XML xml;
    xml.parse(source);
    REQUIRE_THROWS_WITH(xml.validate(),
      "XML Validation Error [Line: 16] Element <itemsOnLoan> IDREFS attribute 'itemsLoanedIDs' contains an invalid "
      "IDREF.");
  }
  SECTION("Validate XML that has a an IDREF attribute value that does not exist on an element.",
    "[XML][DTD][Validate][Attributes]")
  {
    xmlString =
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE collection [\n"
      "<!ELEMENT collection (item|itemsOnLoan)+>\n"
      "<!ELEMENT item (#PCDATA)>\n"
      "<!ELEMENT itemsOnLoan (#PCDATA)>\n"
      "<!ATTLIST item itemID ID #REQUIRED >\n"
      "<!ATTLIST itemsOnLoan itemsLoanedIDs IDREFS #IMPLIED >\n"
      "]>\n"
      "<collection>\n"
      "<item itemID=\"i001\">item descripton</item>\n"
      "<item itemID=\"i002\">item descripton</item>\n"
      "<item itemID=\"i003\">item descripton</item>\n"
      "<item itemID=\"i004\">item descripton</item>\n"
      "<item itemID=\"i005\">item descripton</item>\n"
      "<item itemID=\"i006\">item descripton</item>\n"
      "<itemsOnLoan itemsLoanedIDs=\"i006 i003 i008\">reason for loan</itemsOnLoan>\n"
      "</collection>\n";
    BufferSource source{ xmlString };
    XML xml;
    xml.parse(source);
    REQUIRE_THROWS_WITH(xml.validate(),
      "XML Validation Error [Line: 18] IDREF attribute 'i008' does not reference any element with the ID.");
  }
  SECTION("Validate XML that has an valid NMTOKEN attributes and usage.", "[XML][DTD][Validate][Attributes]")
  {
    xmlString =
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE mountains [\n"
      "<!ELEMENT mountains (mountain)+>\n"
      "<!ELEMENT mountain (name)>\n"
      "<!ELEMENT name (#PCDATA)>\n"
      "<!ATTLIST mountain country NMTOKEN #REQUIRED >\n"
      "]>\n"
      "<mountains>\n"
      "<mountain country=\"NZ\">\n"
      "<name>Mount Cook</name>\n"
      "</mountain>\n"
      "<mountain country=\" AU \">\n"
      "<name>Cradle Mountain</name>\n"
      "</mountain>\n"
      "</mountains>\n";
    BufferSource source{ xmlString };
    XML xml;
    xml.parse(source);
    REQUIRE_NOTHROW(xml.validate());
  }
  SECTION(
    "Validate XML that has an invalid NMTOKEN attribute (internal whitespace).", "[XML][DTD][Validate][Attributes]")
  {
    xmlString =
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE mountains [\n"
      "<!ELEMENT mountains (mountain)+>\n"
      "<!ELEMENT mountain (name)>\n"
      "<!ELEMENT name (#PCDATA)>\n"
      "<!ATTLIST mountain country NMTOKEN #REQUIRED >\n"
      "]>\n"
      "<mountains>\n"
      "<mountain country=\"New Zealand\">\n"
      "<name>Mount Cook</name>\n"
      "</mountain>\n"
      "<mountain country=\"Australia\">\n"
      "<name>Cradle Mountain</name>\n"
      "</mountain>\n"
      "</mountains>\n";
    BufferSource source{ xmlString };
    XML xml;
    xml.parse(source);
    REQUIRE_THROWS_WITH(
      xml.validate(), "XML Validation Error [Line: 9] Element <mountain> NMTOKEN attribute 'country' is invalid.");
  }
  SECTION("Validate XML that has an valid NMTOKENS attributes and usage.", "[XML][DTD][Validate][Attributes]")
  {
    xmlString =
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE mountains [\n"
      "<!ELEMENT mountains (mountain)+>\n"
      "<!ELEMENT mountain (name)>\n"
      "<!ELEMENT name (#PCDATA)>\n"
      "<!ATTLIST mountains country NMTOKENS #REQUIRED >\n"
      "]>\n"
      "<mountains country=\"NZ AU\">\n"
      "<mountain>\n"
      "<name>Mount Cook</name>\n"
      "</mountain>\n"
      "<mountain>\n"
      "<name>Cradle Mountain</name>\n"
      "</mountain>\n"
      "</mountains>\n";
    BufferSource source{ xmlString };
    XML xml;
    xml.parse(source);
    REQUIRE_NOTHROW(xml.validate());
  }
  SECTION("Validate XML that has a valid ENTITY attribute (photo) and usage.", "[XML][DTD][Validate][Attributes]")
  {
    xmlString =
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE mountains [\n"
      "<!ELEMENT mountains (mountain)+>\n"
      "<!ELEMENT mountain (name)>\n"
      "<!ELEMENT name (#PCDATA)>\n"
      "<!ATTLIST mountain photo ENTITY #IMPLIED>\n"
      "<!ENTITY mt_cook_1 SYSTEM \"mt_cook1.jpg\">\n"
      "]>\n"
      "<mountains>\n"
      "<mountain photo=\"mt_cook_1\">\n"
      "<name>Mount Cook</name>\n"
      "</mountain>\n"
      "<mountain>\n"
      "<name>Cradle Mountain</name>\n"
      "</mountain>\n"
      "</mountains>\n";
    BufferSource source{ xmlString };
    XML xml;
    xml.parse(source);
    REQUIRE_NOTHROW(xml.validate());
  }
  SECTION("Validate XML that has an invalid ENTITY attribute (photo).", "[XML][DTD][Validate][Attributes]")
  {
    xmlString =
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE mountains [\n"
      "<!ELEMENT mountains (mountain)+>\n"
      "<!ELEMENT mountain (name)>\n"
      "<!ELEMENT name (#PCDATA)>\n"
      "<!ATTLIST mountain photo ENTITY #IMPLIED>\n"
      "<!ENTITY mt_cook_1 SYSTEM \"mt_cook1.jpg\">\n"
      "]>\n"
      "<mountains>\n"
      "<mountain photo='mt_cook_1'>\n"
      "<name>Mount Cook</name>\n"
      "</mountain>\n"
      "<mountain photo='None'>\n"
      "<name>Cradle Mountain</name>\n"
      "</mountain>\n"
      "</mountains>\n";
    BufferSource source{ xmlString };
    XML xml;
    xml.parse(source);
    REQUIRE_THROWS_WITH(xml.validate(),
      "XML Validation Error [Line: 13] Element <mountain> ENTITY attribute 'photo' value 'None' is not defined.");
  }
  SECTION("Validate XML that has a valid ENTITIES attribute (photo) and usage.", "[XML][DTD][Validate][Attributes]")
  {
    xmlString =
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE mountains [\n"
      "<!ELEMENT mountains (mountain)+>\n"
      "<!ELEMENT mountain (name)>\n"
      "<!ELEMENT name (#PCDATA)>\n"
      "<!ATTLIST mountain photo ENTITIES #IMPLIED>\n"
      "<!ENTITY mt_cook_1 SYSTEM \"mt_cook1.jpg\">\n"
      "<!ENTITY mt_cook_2 SYSTEM \"mt_cook2.jpg\">\n"
      "]>\n"
      "<mountains>\n"
      "<mountain photo=\"mt_cook_1 mt_cook_2\">\n"
      "<name>Mount Cook</name>\n"
      "</mountain>\n"
      "<mountain>\n"
      "<name>Cradle Mountain</name>\n"
      "</mountain>\n"
      "</mountains>\n";
    BufferSource source{ xmlString };
    XML xml;
    xml.parse(source);
    REQUIRE_NOTHROW(xml.validate());
  }
  SECTION("Validate XML that has a invalid ENTITIES attribute (photo).", "[XML][DTD][Validate][Attributes]")
  {
    xmlString =
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE mountains [\n"
      "<!ELEMENT mountains (mountain)+>\n"
      "<!ELEMENT mountain (name)>\n"
      "<!ELEMENT name (#PCDATA)>\n"
      "<!ATTLIST mountain photo ENTITIES #IMPLIED>\n"
      "<!ENTITY mt_cook_1 SYSTEM \"mt_cook1.jpg\">\n"
      "<!ENTITY mt_cook_2 SYSTEM \"mt_cook2.jpg\">\n"
      "]>\n"
      "<mountains>\n"
      "<mountain photo=\"mt_cook_1,mt_cook_2\">\n"
      "<name>Mount Cook</name>\n"
      "</mountain>\n"
      "<mountain>\n"
      "<name>Cradle Mountain</name>\n"
      "</mountain>\n"
      "</mountains>\n";
    BufferSource source{ xmlString };
    XML xml;
    xml.parse(source);
    REQUIRE_THROWS_WITH(xml.validate(),
      "XML Validation Error [Line: 11] Element <mountain> ENTITIES attribute 'photo' value 'mt_cook_1,mt_cook_2' is "
      "not defined.");
  }
  SECTION(
    "Validate XML that has a valid NOTATION attribute (photo_type) and usage.", "[XML][DTD][Validate][Attributes]")
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
    xml.parse(source);
    REQUIRE_NOTHROW(xml.validate());
  }
  SECTION("Validate XML that has a invalid NOTATION attribute (photo_type BMP) and usage.",
    "[XML][DTD][Validate][Attributes]")
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
      "<mountain photo=\"mt_cook_1\" photo_type=\"BMP\">\n"
      "<name>Mount Cook</name>\n"
      "</mountain>\n"
      "<mountain>\n"
      "<name>Cradle Mountain</name>\n"
      "</mountain>\n"
      "</mountains>\n";
    BufferSource source{ xmlString };
    XML xml;
    xml.parse(source);
    REQUIRE_THROWS_WITH(xml.validate(),
      "XML Validation Error [Line: 14] Element <mountain> NOTATION attribute 'photo_type' value 'BMP' is not defined.");
  }
}