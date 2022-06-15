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
using namespace XMLLib;
// ==========
// Test cases
// ==========
TEST_CASE("Parse XML with internal DTD that contains entity definitions and uses", "[XML][DTD][Parse][Entity]")
{
  std::string xmlString;
  SECTION("XML DTD with entity definitions and uses", "[XML][DTD][Parse]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                "<!DOCTYPE note [\n"
                "<!ENTITY nbsp \"&#xA0;\">\n"
                "<!ENTITY writer \"Writer: Donald Duck.\">\n"
                "<!ENTITY copyright \"Copyright: W3Schools.\">]>\n"
                "<note>\n"
                "<to>Tove</to><from>Jani</from><heading>Reminder</heading>\n"
                "<body>Don't forget me this weekend!</body><footer>&writer;&nbsp;&copyright;</footer>\n"
                "</note>\n";
    BufferSource source { xmlString };
    XML xml;
    REQUIRE_NOTHROW(xml.parse(source));
  }
  SECTION("XML DTD with entity internal definitions and uses. Check translation of entity values", "[XML][DTD][Parse][Entity]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                "<!DOCTYPE note [\n"
                "<!ENTITY nbsp \"&#xA0;\">\n"
                "<!ENTITY writer \"Writer: Donald Duck.\">\n"
                "<!ENTITY copyright \"Copyright: W3Schools.\">]>\n"
                "<note>\n"
                "<to>Tove</to><from>Jani</from><heading>Reminder</heading>\n"
                "<body>Don't forget me this weekend!</body><footer>&writer;&nbsp;&copyright;</footer>\n"
                "</note>\n";
    BufferSource source { xmlString };
    XML xml;
    xml.parse(source);
    XMLNodeDTD &xmlNodeDTD  = xml.dtd();
    REQUIRE(XMLNodeRef<XMLNode>(*xml.prolog().children[2]).getNodeType() == XMLNodeType::dtd);
    REQUIRE(xmlNodeDTD.getType() == XMLNodeDTD::Type::internal);
    REQUIRE(XMLNodeRef<XMLNode>(xml.root()).getNodeType() == XMLNodeType::root);
    REQUIRE(xml.root()[4].elementName == "footer");
    REQUIRE(xml.root()[4].getContents() == reinterpret_cast<const char *>(u8"Writer: Donald Duck.\u00A0Copyright: W3Schools."));
  }
  SECTION("XML DTD with entity and how it deals with entity character expansion case 1)", "[XML][DTD][Parse][Entity]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                "<!DOCTYPE note [\n"
                "<!ENTITY example \"<p>An ampersand (&#38;#38;) may be escaped numerically (&#38;#38;#38;) or with a general entity (&amp;amp;).</p>\">]>\n"
                "<note>&example;</note>\n";
    BufferSource source { xmlString };
    XML xml;
    xml.parse(source);
    XMLNodeDTD &xmlNodeDTD  = xml.dtd();
    REQUIRE(XMLNodeRef<XMLNode>(*xml.prolog().children[2]).getNodeType() == XMLNodeType::dtd);
    REQUIRE(xmlNodeDTD.getType() == XMLNodeDTD::Type::internal);
    REQUIRE(xmlNodeDTD.m_entityMapper.get("&example;").internal == "<p>An ampersand (&#38;) may be escaped numerically (&#38;#38;) or with a general entity (&amp;amp;).</p>");
    REQUIRE(xml.root()[0].elementName == "p");
    REQUIRE(xml.root()[0].getContents() == "An ampersand (&) may be escaped numerically (&#38;) or with a general entity (&amp;).");
  }
  SECTION("XML DTD with entity and how it deals with entity character expansion case 2)", "[XML][DTD][Parse][Entity]")
  {
    xmlString = "<?xml version='1.0'?>\n"
                "<!DOCTYPE test [\n"
                "<!ELEMENT test (#PCDATA) >\n"
                "<!ENTITY % xx '&#37;zz;'>\n"
                "<!ENTITY % zz '&#60;!ENTITY tricky \"error-prone\" >' >\n"
                "%xx;\n"
                "]>\n"
                "<test>This sample shows a &tricky; method.</test>\n";
    BufferSource source { xmlString };
    XML xml;
    xml.parse(source);
    XMLNodeDTD &xmlNodeDTD  = xml.dtd();
    REQUIRE(XMLNodeRef<XMLNode>(*xml.prolog().children[2]).getNodeType() == XMLNodeType::dtd);
    REQUIRE(xmlNodeDTD.getType() == XMLNodeDTD::Type::internal);
    REQUIRE(xmlNodeDTD.m_entityMapper.get("%xx;").internal == "%zz;");
    REQUIRE(xmlNodeDTD.m_entityMapper.get("%zz;").internal == "<!ENTITY tricky \"error-prone\" >");
    REQUIRE(xml.root().elementName == "test");
    REQUIRE(xml.root().getContents() == "This sample shows a error-prone method.");
  }
  SECTION("XML DTD with entity and how it deals with entity character expansion case 3)", "[XML][DTD][Parse][Entity]")
  {
    xmlString = "<?xml version='1.0'?>\n"
                "<!DOCTYPE foo [\n"
                "<!ENTITY x \"&lt;\">\n"
                "]>\n"
                "<foo attr=\"&x;\"/>\n";
    BufferSource source { xmlString };
    XML xml;
    xml.parse(source);
    XMLNodeDTD &xmlNodeDTD  = xml.dtd();
    REQUIRE(XMLNodeRef<XMLNode>(*xml.prolog().children[2]).getNodeType() == XMLNodeType::dtd);
    REQUIRE(xmlNodeDTD.getType() == XMLNodeDTD::Type::internal);
    REQUIRE(xmlNodeDTD.m_entityMapper.get("&x;").internal == "&lt;");
    REQUIRE(XMLNodeRef<XMLNodeElement>(xml.root()).getAttributeList().size() == 1);
    XMLAttribute attribute = xml.root().getAttribute("attr");
    REQUIRE(attribute.name == "attr");
    REQUIRE(attribute.value.unparsed == "&x;");
    REQUIRE(attribute.value.parsed == "&lt;");
  }
  // This should throw an error as & ' " < >  not allowed to be assigned to attribute directly (NEED TO FIX)
  SECTION("XML DTD with entity and how it deals with entity character expansion case 4)", "[XML][DTD][Parse][Entity]")
  {
    xmlString = "<?xml version='1.0'?>\n"
                "<!DOCTYPE foo [\n"
                "<!ENTITY x \"&#60;\">\n"
                "]>\n"
                "<foo attr=\"&x;\"/>\n";
    BufferSource source { xmlString };
    XML xml;
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error [Line: 5 Column: 21] Attribute value contains invalid character '<', '\"', ''' or '&'.");
  }
  SECTION("XML DTD with entity used within an entity.", "[XML][DTD][Parse][Entity]")
  {
    xmlString = "<?xml version='1.0'?>\n"
                "<!DOCTYPE author [\n"
                "<!ELEMENT author (#PCDATA)>\n"
                "<!ENTITY email \"josmith@theworldaccordingtojosmith.com\">\n"
                "<!--the following use of a general entity is legal \n"
                "if it is used in the XML document-->\n"
                "<!ENTITY js \"Jo Smith &email;\">\n"
                "]>\n"
                "<author>&js;</author>\n";
    BufferSource source { xmlString };
    XML xml;
    xml.parse(source);
    XMLNodeDTD &xmlNodeDTD  = xml.dtd();
    REQUIRE(XMLNodeRef<XMLNode>(*xml.prolog().children[2]).getNodeType() == XMLNodeType::dtd);
    REQUIRE(xmlNodeDTD.getType() == XMLNodeDTD::Type::internal);
    REQUIRE(xmlNodeDTD.m_entityMapper.get("&js;").internal == "Jo Smith &email;");
    REQUIRE(xml.root().elementName == "author");
    REQUIRE(XMLNodeRef<XMLNode>(*xml.root().children[0]).getNodeType() == XMLNodeType::entity);
    REQUIRE(XMLNodeRef<XMLNodeEntityReference>(*xml.root().children[0]).getContents() == "Jo Smith josmith@theworldaccordingtojosmith.com");
    REQUIRE(xml.root().getContents() == "Jo Smith josmith@theworldaccordingtojosmith.com");
  }
  SECTION("XML DTD with entity used within an entity with recursion.", "[XML][DTD][Parse]")
  {
    xmlString = "<?xml version='1.0'?>\n"
                "<!DOCTYPE author [\n"
                "<!ELEMENT author (#PCDATA)>\n"
                "<!ENTITY email \"josmith@theworldaccordingtojosmith.com\">\n"
                "<!--the two entity"
                " statements are infinitely recursive-->\n"
                "<!ENTITY email \"user@user.com &js;\">\n"
                "<!ENTITY js \"Jo Smith &email;\">\n"
                "]>\n"
                "<author>&js;</author>\n";
    BufferSource source { xmlString };
    XML xml;
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error: Entity '&email;' contains recursive definition which is not allowed.");
  }
  SECTION("XML DTD with entity that contains a character reference that is parsed straight away.", "[XML][DTD][Parse][Entity]")
  {
    xmlString = "<?xml version='1.0'?>\n"
                "<!DOCTYPE author [\n"
                "<!ELEMENT author (#PCDATA)>\n"
                "<!ENTITY email \"josmith&#x40;theworldaccordingtojosmith.com\">\n"
                "<!ENTITY js \"Jo Smith &email;\">\n"
                "]>\n"
                "<author>&js;</author>\n";
    BufferSource source { xmlString };
    XML xml;
    xml.parse(source);
    XMLNodeDTD &xmlNodeDTD  = xml.dtd();
    REQUIRE(XMLNodeRef<XMLNode>(*xml.prolog().children[2]).getNodeType() == XMLNodeType::dtd);
    REQUIRE(xmlNodeDTD.getType() == XMLNodeDTD::Type::internal);
    REQUIRE(xmlNodeDTD.m_entityMapper.get("&email;").internal == "josmith@theworldaccordingtojosmith.com");
  }
  SECTION("XML DTD with entity that is defined externally (file name.txt).", "[XML][DTD][Parse][Entity]")
  {
    xmlString = "<?xml version='1.0'?>\n"
                "<!DOCTYPE foo [\n"
                "<!ELEMENT foo ANY>\n"
                "<!ENTITY name SYSTEM \"./testData/name.txt\">\n"
                "]>\n"
                "<foo>Hello &name;</foo>\n";
    BufferSource source { xmlString };
    XML xml;
    xml.parse(source);
    XMLNodeDTD &xmlNodeDTD  = xml.dtd();
    REQUIRE(XMLNodeRef<XMLNode>(*xml.prolog().children[2]).getNodeType() == XMLNodeType::dtd);
    REQUIRE(xmlNodeDTD.getType() == XMLNodeDTD::Type::internal);
    REQUIRE(xmlNodeDTD.getRootName() == XMLNodeRef<XMLNodeElement>(xml.root()).elementName);
    REQUIRE(xml.root().elementName == "foo");
    REQUIRE(xml.root().getContents() == "Hello John Joe Doe");
  }
  SECTION("XML DTD with entity that is defined externally (file that does not exist).", "[XML][DTD][Parse][Entity]")
  {
    xmlString = "<?xml version='1.0'?>\n"
                "<!DOCTYPE foo [\n"
                "<!ELEMENT foo ANY>\n"
                "<!ENTITY name SYSTEM \"./testData/unknown.txt\">\n"
                "]>\n"
                "<foo>Hello &name;</foo>\n";
    BufferSource source { xmlString };
    XML xml;
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error: Entity '&name;' source file './testData/unknown.txt' does not exist.");
  }
  SECTION("XML with internal DTD with parameter entities to parse  (internal cannot appear within tags).", "[XML][DTD][Parse][Entity]")
  {
    xmlString = "<!DOCTYPE REPORT [\n"
                "<!ENTITY % empty_report \"<!ELEMENT REPORT EMPTY>\">"
                "%empty_report;\n"
                "]>\n"
                "<REPORT></REPORT>\n";
    BufferSource source { xmlString };
    XML xml;
    REQUIRE_NOTHROW(xml.parse(source));
  }
  SECTION("XML with internal DTD with parameter entities to parse and check values (internal cannot appear within tags).", "[XML][DTD][Parse][Entity]")
  {
    xmlString = "<!DOCTYPE REPORT [\n"
                "<!ENTITY % empty_report \"<!ELEMENT REPORT EMPTY>\">"
                "%empty_report;\n"
                "]>\n"
                "<REPORT></REPORT>\n";
    BufferSource source { xmlString };
    XML xml;
    xml.parse(source);
    XMLNodeDTD &xmlNodeDTD  = xml.dtd();
    REQUIRE(XMLNodeRef<XMLNode>(*xml.prolog().children[1]).getNodeType() == XMLNodeType::dtd);
    REQUIRE(xmlNodeDTD.getType() == XMLNodeDTD::Type::internal);
    REQUIRE(xmlNodeDTD.getRootName() == XMLNodeRef<XMLNodeElement>(xml.root()).elementName);
    REQUIRE(xmlNodeDTD.getRootName() == "REPORT");
    REQUIRE(xmlNodeDTD.m_entityMapper.get("%empty_report;").internal == "<!ELEMENT REPORT EMPTY>");
    REQUIRE(xmlNodeDTD.getElement("REPORT").name == "REPORT");
    REQUIRE(xmlNodeDTD.getElement("REPORT").content.parsed == "EMPTY");
  }
  SECTION("XML with external DTD with parameter entities to parse.", "[XML][DTD][Parse][Entity]")
  {
    xmlString = "<!DOCTYPE REPORT SYSTEM \"./testData/report01.dtd\">\n"
                "<REPORT></REPORT>\n";
    BufferSource source { xmlString };
    XML xml;
    REQUIRE_NOTHROW(xml.parse(source));
    XMLNodeDTD &xmlNodeDTD  = xml.dtd();
    REQUIRE(xmlNodeDTD.getType() == XMLNodeDTD::Type::external);
  }

  SECTION("XML with external DTD with both types of entities to parse an check values", "[XML][DTD][Parse][Entity]")
  {
    xmlString = "<!DOCTYPE REPORT SYSTEM \"./testData/report01.dtd\">"
                "<REPORT></REPORT>\n";
    BufferSource source { xmlString };
    XML xml;
    xml.parse(source);
    XMLNodeDTD &xmlNodeDTD  = xml.dtd();
    REQUIRE(XMLNodeRef<XMLNode>(*xml.prolog().children[1]).getNodeType() == XMLNodeType::dtd);
    REQUIRE(xmlNodeDTD.getType() == XMLNodeDTD::Type::external);
    REQUIRE(xmlNodeDTD.getRootName() == XMLNodeRef<XMLNodeElement>(xml.root()).elementName);
    REQUIRE(xmlNodeDTD.getRootName() == "REPORT");
    REQUIRE(xmlNodeDTD.m_entityMapper.get("%contact;").internal == "phone");
    REQUIRE(xmlNodeDTD.m_entityMapper.get("%area;").internal == "name, street, pincode, city");
    REQUIRE(xmlNodeDTD.getElement("REPORT").name == "REPORT");
    REQUIRE(xmlNodeDTD.getElement("REPORT").content.unparsed == "(residence|apartment|office|shop)*");
    REQUIRE(xmlNodeDTD.getElement("residence").name == "residence");
    REQUIRE(xmlNodeDTD.getElement("residence").content.unparsed == "(name, street, pincode, city, phone)");
    REQUIRE(xmlNodeDTD.getElement("apartment").name == "apartment");
    REQUIRE(xmlNodeDTD.getElement("apartment").content.unparsed == "(name, street, pincode, city, phone)");
    REQUIRE(xmlNodeDTD.getElement("office").name == "office");
    REQUIRE(xmlNodeDTD.getElement("office").content.unparsed == "(name, street, pincode, city, phone)");
    REQUIRE(xmlNodeDTD.getElement("shop").name == "shop");
    REQUIRE(xmlNodeDTD.getElement("shop").content.unparsed == "(name, street, pincode, city, phone)");
  }
  SECTION("XML DTD with parameter entity within general entity.", "[XML][DTD][Parse][Entity]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                "<!DOCTYPE note SYSTEM \"./testData/note002.dtd\">\n"
                "<note>"
                "&signature;"
                "</note>\n";
    BufferSource source { xmlString };
    XML xml;
    xml.parse(source);
    XMLNodeDTD &xmlNodeDTD  = xml.dtd();
    REQUIRE(XMLNodeRef<XMLNode>(*xml.prolog().children[2]).getNodeType() == XMLNodeType::dtd);
    REQUIRE(xmlNodeDTD.getType() == XMLNodeDTD::Type::external);
    REQUIRE(XMLNodeRef<XMLNode>(xml.root()).getNodeType() == XMLNodeType::root);
    REQUIRE(xmlNodeDTD.m_entityMapper.get("&signature;").internal == "© 1999 Yoyodyne, Inc. &legal;");
    REQUIRE(xml.root().getContents() == "© 1999 Yoyodyne, Inc. All Rights Reserved.");
  }
}