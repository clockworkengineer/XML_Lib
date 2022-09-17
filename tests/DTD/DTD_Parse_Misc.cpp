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
TEST_CASE("Parse XML with DTD both internal/external", "[XML][DTD][Parse]")
{
  std::string xmlString;
  SECTION("XML with internal DTD", "[XML][DTD][Parse]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
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
                "</note>\n";
    BufferSource source { xmlString };
    XML xml;
    REQUIRE_NOTHROW(xml.parse(source));
    XMLNodeDTD &xmlNodeDTD  = xml.dtd();
    REQUIRE(xmlNodeDTD.getType() == XMLNodeDTD::Type::internal);
  }
  SECTION("XML with external (SYSTEM) DTD", "[XML][DTD][Parse]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                "<!DOCTYPE note SYSTEM \"./testData/note001.dtd\">\n"
                "<note>\n"
                "<to>Tove</to><from>Jani</from><heading>Reminder</heading>\n"
                "<body>Don't forget me this weekend!</body>\n"
                "</note>\n";
    BufferSource source { xmlString };
    XML xml;
    REQUIRE_NOTHROW(xml.parse(source));
    XMLNodeDTD &xmlNodeDTD  = xml.dtd();
    REQUIRE(xmlNodeDTD.getType() == XMLNodeDTD::Type::external);
  }
  SECTION("XML with external (PUBLIC) DTD", "[XML][DTD][Parse]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                "<!DOCTYPE note PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
                "<note>\n"
                "<to>Tove</to><from>Jani</from><heading>Reminder</heading>\n"
                "<body>Don't forget me this weekend!</body>\n"
                "</note>\n";
    BufferSource source { xmlString };
    XML xml;
    REQUIRE_NOTHROW(xml.parse(source));
    XMLNodeDTD &xmlNodeDTD  = xml.dtd();
    REQUIRE(xmlNodeDTD.getType() == XMLNodeDTD::Type::external);
  }
  SECTION("XML with external DTD with !NOTATION to parse and check values.", "[XML][DTD][Parse]")
  {
    xmlString = "<!DOCTYPE REPORT SYSTEM \"./testData/report02.dtd\">"
                "<REPORT></REPORT>\n";
    BufferSource source { xmlString };
    XML xml;
    xml.parse(source);
    XMLNodeDTD &xmlNodeDTD  = xml.dtd();
    REQUIRE(XMLNodeRef<XMLNode>(*xml.prolog().children[1]).getNodeType() == XMLNodeType::dtd);
    REQUIRE(xmlNodeDTD.getType() == XMLNodeDTD::Type::external);
    REQUIRE(xmlNodeDTD.getNotation("GIF").type == "SYSTEM");
    REQUIRE(xmlNodeDTD.getNotation("GIF").systemID == "GIF");
    REQUIRE(xmlNodeDTD.getNotation("JPG").type == "SYSTEM");
    REQUIRE(xmlNodeDTD.getNotation("JPG").systemID == "JPG");
    REQUIRE(xmlNodeDTD.getNotation("BMP").type == "SYSTEM");
    REQUIRE(xmlNodeDTD.getNotation("BMP").systemID == "BMP");
  }
  SECTION("XML with internal DTD containing comments.", "[XML][DTD][Parse]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
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
                "</note>\n";
    BufferSource source { xmlString };
    XML xml;
    REQUIRE_NOTHROW(xml.parse(source));
    XMLNodeDTD &xmlNodeDTD  = xml.dtd();
    REQUIRE(xmlNodeDTD.getType() == XMLNodeDTD::Type::internal);
  }
}
TEST_CASE("Parse XML DTD and check values.", "[XML][DTD][Parse]")
{
  std::string xmlString;
  SECTION("XML with internal to parse DTD and check values", "[XML][DTD][Parse]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>\n"
                "<!DOCTYPE address [\n"
                "<!ELEMENT address (name,company,phone)>\n"
                "<!ELEMENT name (#PCDATA)><!ELEMENT company (#PCDATA)>\n"
                "<!ELEMENT phone (#PCDATA)><!ELEMENT br EMPTY>\n"
                "<!ELEMENT footer ANY>]>\n"
                "<address>\n"
                "<name>Tanmay Patil</name>\n"
                "<company>TutorialsPoint</company>\n"
                "<phone>(011) 123-4567"
                "</phone>\n"
                "</address>\n";
    BufferSource source { xmlString };
    XML xml;
    xml.parse(source);
    XMLNodeDTD &xmlNodeDTD  = xml.dtd();
    REQUIRE(XMLNodeRef<XMLNode>(*xml.prolog().children[2]).getNodeType() == XMLNodeType::dtd);
    REQUIRE(xmlNodeDTD.getType() == XMLNodeDTD::Type::internal);
    REQUIRE(xmlNodeDTD.getRootName() == XMLNodeRef<XMLNodeElement>(xml.root()).name());
    REQUIRE(xmlNodeDTD.getElement("address").name == "address");
    REQUIRE(xmlNodeDTD.getElement("address").content.unparsed == "(name,company,phone)");
    REQUIRE(xmlNodeDTD.getElement("name").name == "name");
    REQUIRE(xmlNodeDTD.getElement("name").content.unparsed == "(#PCDATA)");
    REQUIRE(xmlNodeDTD.getElement("company").name == "company");
    REQUIRE(xmlNodeDTD.getElement("company").content.unparsed == "(#PCDATA)");
    REQUIRE(xmlNodeDTD.getElement("phone").name == "phone");
    REQUIRE(xmlNodeDTD.getElement("phone").content.unparsed == "(#PCDATA)");
    REQUIRE(xmlNodeDTD.getElement("br").name == "br");
    REQUIRE(xmlNodeDTD.getElement("br").content.unparsed == "EMPTY");
    REQUIRE(xmlNodeDTD.getElement("footer").name == "footer");
    REQUIRE(xmlNodeDTD.getElement("footer").content.unparsed == "ANY");
  }
  SECTION("XML with external file DTD and check values", "[XML][DTD][Parse]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                "<!DOCTYPE note SYSTEM \"./testData/note001.dtd\">\n"
                "<note>\n"
                "<to>Tove"
                "</to><from>Jani</from><heading>Reminder</heading><body>Don't forget me this weekend!</body>\n"
                "</note>\n";
    BufferSource source { xmlString };
    XML xml;
    xml.parse(source);
    XMLNodeDTD &xmlNodeDTD  = xml.dtd();
    REQUIRE(XMLNodeRef<XMLNode>(*xml.prolog().children[2]).getNodeType() == XMLNodeType::dtd);
    REQUIRE(xmlNodeDTD.getType() == XMLNodeDTD::Type::external);
    REQUIRE(xmlNodeDTD.getRootName() == XMLNodeRef<XMLNodeElement>(xml.root()).name());
    REQUIRE(xmlNodeDTD.getExternalReference().type == "SYSTEM");
    REQUIRE(xmlNodeDTD.getExternalReference().systemID == "./testData/note001.dtd");
    REQUIRE(xmlNodeDTD.getElement("note").name == "note");
    REQUIRE(xmlNodeDTD.getElement("note").content.unparsed == "(to,from,heading,body)");
  }
  SECTION("XML with external URL DTD to parse and check values", "[XML][DTD][Parse]")
  {
    xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\""
                " \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
                "<html></html>\n";
    BufferSource source { xmlString };
    XML xml;
    xml.parse(source);
    XMLNodeDTD &xmlNodeDTD  = xml.dtd();
    REQUIRE(XMLNodeRef<XMLNode>(*xml.prolog().children[2]).getNodeType() == XMLNodeType::dtd);
    REQUIRE(xmlNodeDTD.getType() == XMLNodeDTD::Type::external);
    REQUIRE(xmlNodeDTD.getRootName() == XMLNodeRef<XMLNodeElement>(xml.root()).name());
    REQUIRE(xmlNodeDTD.getExternalReference().type == "PUBLIC");
    REQUIRE(xmlNodeDTD.getExternalReference().systemID == "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd");
    REQUIRE(xmlNodeDTD.getExternalReference().publicID == "-//W3C//DTD XHTML 1.0 Transitional//EN");
  }
}
TEST_CASE("Parse XML DTD with various element content specification errors.", "[XML][DTD][Parse][Error]")
{
  std::string xmlString;
  SECTION("XML with a DTD that contains an illegal mixed content specification (#PCDATA doesn't come first).", "[XML][DTD][Parse][Error]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!-- Fig. B.5 : mixed.xml-->\n"
                "<!-- Mixed content type elements -->\n"
                "<!DOCTYPE format [\n"
                "<!ELEMENT format ( bold | italic | #PCDATA)*>\n"
                "<!ELEMENT bold ( #PCDATA )>\n"
                "<!ELEMENT italic ( #PCDATA )>\n"
                "]>\n"
                "<format>\n"
                "Book catalog entry:\n"
                "<bold>XML</bold>\n"
                "<italic>XML How to Program</italic>\n"
                "This book carefully explains XML-based systems development."
                "</format>";
    BufferSource source { xmlString };
    XML xml;
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error: Invalid content specification for element <format>.");
  }
  SECTION("XML with a DTD that contains an illegal mixed content specification (does not end with '*').", "[XML][DTD][Parse][Error]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!-- Fig. B.5 : mixed.xml-->\n"
                "<!-- Mixed content type elements -->\n"
                "<!DOCTYPE format [\n"
                "<!ELEMENT format ( #PCDATA | bold | italic )+>\n"
                "<!ELEMENT bold ( #PCDATA )>\n"
                "<!ELEMENT italic ( #PCDATA )>\n"
                "]>\n"
                "<format>\n"
                "Book catalog entry:\n"
                "<bold>XML</bold>\n"
                "<italic>XML How to Program</italic>\n"
                "This book carefully explains XML-based systems development."
                "</format>";
    BufferSource source { xmlString };
    XML xml;
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error: Invalid content specification for element <format>.");
  }
  SECTION("Parse XML with DTD that contains a content specification in error (missing ',').", "[XML][DTD][Parse][Error]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE note ["
                "<!ELEMENT note (to,from,heading body)>\n"
                "<!ELEMENT to (#PCDATA)>\n"
                "<!ELEMENT from (#PCDATA)>\n"
                "<!ELEMENT heading (#PCDATA)>\n"
                "<!ELEMENT body (#PCDATA)>\n"
                "]>\n"
                "<note>\n"
                "<to>Tove</to><from>Jani</from><heading>Reminder</heading>\n"
                "<body>Don't forget me this weekend</body>\n"
                "</note>\n";
    BufferSource source { xmlString };
    XML xml;
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error: Invalid content specification for element <note>.");
  }
  SECTION("Parse XML with DTD that contains a content specification in error (missing element name).", "[XML][DTD][Parse][Error]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE note ["
                "<!ELEMENT note (to,from,heading,)>\n"
                "<!ELEMENT to (#PCDATA)>\n"
                "<!ELEMENT from (#PCDATA)>\n"
                "<!ELEMENT heading (#PCDATA)>\n"
                "<!ELEMENT body (#PCDATA)>\n"
                "]>\n"
                "<note>\n"
                "<to>Tove</to><from>Jani</from><heading>Reminder</heading>\n"
                "<body>Don't forget me this weekend</body>\n"
                "</note>\n";
    BufferSource source { xmlString };
    XML xml;
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error: Invalid content specification for element <note>.");
  }
  SECTION("XML with a DTD that contains an illegal mixed content specification (uses ',').", "[XML][DTD][Parse][Error]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!-- Fig. B.5 : mixed.xml-->\n"
                "<!-- Mixed content type elements -->\n"
                "<!DOCTYPE format [\n"
                "<!ELEMENT format ( #PCDATA | bold , italic )*>\n"
                "<!ELEMENT bold ( #PCDATA )>\n"
                "<!ELEMENT italic ( #PCDATA )>\n"
                "]>\n"
                "<format>\n"
                "Book catalog entry:\n"
                "<bold>XML</bold>\n"
                "<italic>XML How to Program</italic>\n"
                "This book carefully explains XML-based systems development."
                "</format>";
    BufferSource source { xmlString };
    XML xml;
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error: Invalid content specification for element <format>.");
  }
}
TEST_CASE("XML with a DTD conditional INCLUDE/IGNORE tags", "[XML][DTD][Parse][Conditional]")
{
  std::string xmlString;
  SECTION("XML with a DTD conditional INCLUDE/IGNORE tags", "[XML][DTD][Parse][Conditional]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE root SYSTEM \"./testData/conditional001.dtd\">\n"
                "<root>\n"
                "</root>";
    BufferSource source { xmlString };
    XML xml;
    REQUIRE_NOTHROW(xml.parse(source));
    XMLNodeDTD &xmlNodeDTD  = xml.dtd();
    REQUIRE(xmlNodeDTD.getType() == XMLNodeDTD::Type::external);
  }
  SECTION("XML with a DTD with conditional INCLUDE containing an entity.", "[XML][DTD][Parse][Conditional]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE root SYSTEM \"./testData/conditional001.dtd\">\n"
                "<root>\n"
                "</root>";
    BufferSource source { xmlString };
    XML xml;
    REQUIRE_NOTHROW(xml.parse(source));
    XMLNodeDTD &xmlNodeDTD  = xml.dtd();
    REQUIRE(xmlNodeDTD.getType() == XMLNodeDTD::Type::external);
    REQUIRE(xmlNodeDTD.m_entityMapper.get("&example;").internal == "Joe Smith");
  }
  SECTION("XML with a DTD with invalid conditional value.", "[XML][DTD][Parse][Conditional]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE root SYSTEM \"./testData/conditional002.dtd\">\n"
                "<root>\n"
                "</root>";
    BufferSource source { xmlString };
    XML xml;
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error [Line: 1 Column: 19] Conditional value not INCLUDE or IGNORE.");
  }
  SECTION("XML with a DTD with missing opening '[' from conditional.", "[XML][DTD][Parse][Conditional]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE root SYSTEM \"./testData/conditional003.dtd\">\n"
                "<root>\n"
                "</root>";
    BufferSource source { xmlString };
    XML xml;
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error [Line: 1 Column: 23] Missing opening '[' from conditional.");
  }
  SECTION("XML with a DTD with conditional controlled with a entity reference value (INCLUDE) containing an entity definition.", "[XML][DTD][Parse][Conditional]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE root SYSTEM \"./testData/conditional004.dtd\">\n"
                "<root>\n"
                "</root>";
    BufferSource source { xmlString };
    XML xml;
    REQUIRE_NOTHROW(xml.parse(source));
    XMLNodeDTD &xmlNodeDTD  = xml.dtd();
    REQUIRE(xmlNodeDTD.getType() == XMLNodeDTD::Type::external);
    REQUIRE(xmlNodeDTD.m_entityMapper.get("&example;").internal == "Joe Smith");
  }
  SECTION("XML with a DTD with conditional controlled entity reference value (IGNORE) containing an entity definition.", "[XML][DTD][Parse][Conditional]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE root SYSTEM \"./testData/conditional005.dtd\">\n"
                "<root>\n"
                "</root>";
    BufferSource source { xmlString };
    XML xml;
    REQUIRE_NOTHROW(xml.parse(source));
    XMLNodeDTD &xmlNodeDTD  = xml.dtd();
    REQUIRE(xmlNodeDTD.getType() == XMLNodeDTD::Type::external);
    REQUIRE(xmlNodeDTD.m_entityMapper.get("&example;").internal == "");
  }
  SECTION("XML with a DTD with nested conditionals that are both INCLUDE.", "[XML][DTD][Parse][Conditional]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE root SYSTEM \"./testData/conditional006.dtd\">\n"
                "<root>\n"
                "</root>";
    BufferSource source { xmlString };
    XML xml;
    REQUIRE_NOTHROW(xml.parse(source));
    XMLNodeDTD &xmlNodeDTD  = xml.dtd();
    REQUIRE(xmlNodeDTD.getType() == XMLNodeDTD::Type::external);
    REQUIRE(xmlNodeDTD.m_entityMapper.get("&example;").internal == "Joe Smith");
  }
  SECTION("XML with a DTD with nested conditionals that are both INCLUDE and two entities.", "[XML][DTD][Parse][Conditional]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE root SYSTEM \"./testData/conditional007.dtd\">\n"
                "<root>\n"
                "</root>";
    BufferSource source { xmlString };
    XML xml;
    REQUIRE_NOTHROW(xml.parse(source));
    XMLNodeDTD &xmlNodeDTD  = xml.dtd();
    REQUIRE(xmlNodeDTD.getType() == XMLNodeDTD::Type::external);
    REQUIRE(xmlNodeDTD.m_entityMapper.get("&example;").internal == "Joe Smith");
    REQUIRE(xmlNodeDTD.m_entityMapper.get("&example1;").internal == "Joe Smith 1");
  }
  SECTION("XML with a DTD with nested conditionals that are  outter INCLUDE inner IGNORE plus two entities.", "[XML][DTD][Parse][Conditional]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE root SYSTEM \"./testData/conditional008.dtd\">\n"
                "<root>\n"
                "</root>";
    BufferSource source { xmlString };
    XML xml;
    REQUIRE_NOTHROW(xml.parse(source));
    XMLNodeDTD &xmlNodeDTD  = xml.dtd();
    REQUIRE(xmlNodeDTD.getType() == XMLNodeDTD::Type::external);
    REQUIRE(xmlNodeDTD.m_entityMapper.get("&example;").internal == "");
    REQUIRE(xmlNodeDTD.m_entityMapper.get("&example1;").internal == "Joe Smith 1");
  }
  SECTION("XML with a DTD with nested conditionals that are  outter IGNORE inner INCLUDE plus two entities.", "[XML][DTD][Parse][Conditional]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE root SYSTEM \"./testData/conditional009.dtd\">\n"
                "<root>\n"
                "</root>";
    BufferSource source { xmlString };
    XML xml;
    REQUIRE_NOTHROW(xml.parse(source));
    XMLNodeDTD &xmlNodeDTD  = xml.dtd();
    REQUIRE(xmlNodeDTD.getType() == XMLNodeDTD::Type::external);
    REQUIRE(xmlNodeDTD.m_entityMapper.get("&example1;").internal == "");
    REQUIRE(xmlNodeDTD.m_entityMapper.get("&example;").internal == "");
  }
  SECTION("XML with a DTD with nested conditionals controlled from internally defined DTD that is parsed first (switch on).", "[XML][DTD][Parse][Conditional]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE root SYSTEM \"./testData/conditional010.dtd\" [ <!ENTITY % debug \"INCLUDE\"> ]>\n"
                "<root>\n"
                "</root>";
    BufferSource source { xmlString };
    XML xml;
    REQUIRE_NOTHROW(xml.parse(source));
    XMLNodeDTD &xmlNodeDTD  = xml.dtd();
    REQUIRE(xmlNodeDTD.getType() == (XMLNodeDTD::Type::internal | XMLNodeDTD::Type::external));
    REQUIRE(xmlNodeDTD.m_entityMapper.get("&example;").internal == "Joe Smith");
    REQUIRE(xmlNodeDTD.m_entityMapper.get("&example1;").internal == "Joe Smith 1");
  }
  SECTION("XML with a DTD with nested conditionals controlled from internally defined DTD that is parsed first (switch off).", "[XML][DTD][Parse][Conditional]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE root SYSTEM \"./testData/conditional010.dtd\" [ <!ENTITY % debug \"IGNORE\"> ]>\n"
                "<root>\n"
                "</root>";
    BufferSource source { xmlString };
    XML xml;
    REQUIRE_NOTHROW(xml.parse(source));
    XMLNodeDTD &xmlNodeDTD  = xml.dtd();
    REQUIRE(xmlNodeDTD.getType() == (XMLNodeDTD::Type::internal | XMLNodeDTD::Type::external));
    REQUIRE(xmlNodeDTD.m_entityMapper.get("&example;").internal == "");
    REQUIRE(xmlNodeDTD.m_entityMapper.get("&example1;").internal == "");
  }
  SECTION("XML with a DTD with nested conditionals controlled from internally defined DTD that is parsed first (invalid value).", "[XML][DTD][Parse][Conditional]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE root SYSTEM \"./testData/conditional010.dtd\" [ <!ENTITY % debug \"IGNOE\"> ]>\n"
                "<root>\n"
                "</root>";
    BufferSource source { xmlString };
    XML xml;
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error [Line: 1 Column: 23] Conditional value not INCLUDE or IGNORE.");
  }
  SECTION("XML with a DTD with nested conditionals controlled from internally defined DTD that is parsed first (novalue).", "[XML][DTD][Parse][Conditional]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE root SYSTEM \"./testData/conditional010.dtd\">\n"
                "<root>\n"
                "</root>";
    BufferSource source { xmlString };
    XML xml;
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error [Line: 1 Column: 23] Conditional value not INCLUDE or IGNORE.");
  }
}

TEST_CASE("Parse XML with more than DTD declaration", "[XML][DTD][Parse]")
{
  std::string xmlString;
  SECTION("XML with more than one DTD declaration ", "[XML][DTD][Parse]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<!DOCTYPE note ["
                "<!ELEMENT note (to,from,heading,body)>\n"
                "<!ELEMENT to (#PCDATA)>\n"
                "<!ELEMENT from (#PCDATA)>\n"
                "<!ELEMENT heading (#PCDATA)>\n"
                "<!ELEMENT body (#PCDATA)>\n"
                "]>\n"
                "<!DOCTYPE note ["
                "<!ELEMENT note (to,from,heading)>\n"
                "<!ELEMENT to (#PCDATA)>\n"
                "<!ELEMENT from (#PCDATA)>\n"
                "<!ELEMENT heading (#PCDATA)>\n"
                "]>\n"
                "<note>\n"
                "<to>Tove</to><from>Jani</from><heading>Reminder</heading>\n"
                "<body>Don't forget me this weekend</body>\n"
                "</note>\n";
    BufferSource source { xmlString };
    XML xml;
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error [Line: 8 Column: 14] More than one DOCTYPE declaration.");
  }
}