#include "XML_Lib_Tests.hpp"

TEST_CASE("Parse XML with DTD_Validator both internal/external", "[XML][DTD_Validator][Parse]")
{
  XML xml;
  SECTION("XML with internal DTD_Validator", "[XML][DTD_Validator][Parse]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
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
      "</note>\n"
    };
    REQUIRE_NOTHROW(xml.parse(source));
    DTD &xDTD = XRef<DTD>(xml.dtd());
    REQUIRE(xDTD.getType() == DTD::Type::internal);
  }
  SECTION("XML with external (SYSTEM) DTD_Validator", "[XML][DTD_Validator][Parse]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
      "<!DOCTYPE note SYSTEM \"./files/note001.dtd\">\n"
      "<note>\n"
      "<to>Tove</to><from>Jani</from><heading>Reminder</heading>\n"
      "<body>Don't forget me this weekend!</body>\n"
      "</note>\n"
    };

    REQUIRE_NOTHROW(xml.parse(source));
    DTD &xDTD = XRef<DTD>(xml.dtd());
    REQUIRE(xDTD.getType() == DTD::Type::external);
  }
  SECTION("XML with external (PUBLIC) DTD_Validator", "[XML][DTD_Validator][Parse]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
      "<!DOCTYPE note PUBLIC \"-//W3C//DTD_Validator XHTML 1.0 Transitional//EN\" "
      "\"http://www.w3.org/TR/xhtml1/DTD_Validator/xhtml1-transitional.dtd\">\n"
      "<note>\n"
      "<to>Tove</to><from>Jani</from><heading>Reminder</heading>\n"
      "<body>Don't forget me this weekend!</body>\n"
      "</note>\n"
    };
    REQUIRE_NOTHROW(xml.parse(source));
    DTD &xDTD = XRef<DTD>(xml.dtd());
    REQUIRE(xDTD.getType() == DTD::Type::external);
  }
  SECTION("XML with external DTD_Validator with !NOTATION to parse and check values.", "[XML][DTD_Validator][Parse]")
  {

    BufferSource source{
      "<!DOCTYPE REPORT SYSTEM \"./files/report02.dtd\">"
      "<REPORT></REPORT>\n"
    };
    xml.parse(source);
    DTD &xDTD = XRef<DTD>(xml.dtd());
    REQUIRE_FALSE(!isA<DTD>(xml.prolog().getChildren()[1]));
    REQUIRE(xDTD.getType() == DTD::Type::external);
    REQUIRE(xDTD.getNotation("GIF").getType() == "SYSTEM");
    REQUIRE(xDTD.getNotation("GIF").getSystemID() == "GIF");
    REQUIRE(xDTD.getNotation("JPG").getType() == "SYSTEM");
    REQUIRE(xDTD.getNotation("JPG").getSystemID() == "JPG");
    REQUIRE(xDTD.getNotation("BMP").getType() == "SYSTEM");
    REQUIRE(xDTD.getNotation("BMP").getSystemID() == "BMP");
  }
  SECTION("XML with internal DTD_Validator containing comments.", "[XML][DTD_Validator][Parse]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
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
      "</note>\n"
    };
    REQUIRE_NOTHROW(xml.parse(source));
    DTD &xDTD = XRef<DTD>(xml.dtd());
    REQUIRE(xDTD.getType() == DTD::Type::internal);
  }
}
TEST_CASE("Parse XML DTD_Validator and check values.", "[XML][DTD_Validator][Parse]")
{
  XML xml;
  SECTION("XML with internal to parse DTD_Validator and check values", "[XML][DTD_Validator][Parse]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>\n"
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
      "</address>\n"
    };
    xml.parse(source);
    DTD &xDTD = XRef<DTD>(xml.dtd());
    REQUIRE_FALSE(!isA<DTD>(xml.prolog().getChildren()[2]));
    REQUIRE(xDTD.getType() == DTD::Type::internal);
    REQUIRE(xDTD.getRootName() == XRef<Element>(xml.root()).name());
    REQUIRE(xDTD.getElement("address").name == "address");
    REQUIRE(xDTD.getElement("address").content.getUnparsed() == "(name,company,phone)");
    REQUIRE(xDTD.getElement("name").name == "name");
    REQUIRE(xDTD.getElement("name").content.getUnparsed() == "(#PCDATA)");
    REQUIRE(xDTD.getElement("company").name == "company");
    REQUIRE(xDTD.getElement("company").content.getUnparsed() == "(#PCDATA)");
    REQUIRE(xDTD.getElement("phone").name == "phone");
    REQUIRE(xDTD.getElement("phone").content.getUnparsed() == "(#PCDATA)");
    REQUIRE(xDTD.getElement("br").name == "br");
    REQUIRE(xDTD.getElement("br").content.getUnparsed() == "EMPTY");
    REQUIRE(xDTD.getElement("footer").name == "footer");
    REQUIRE(xDTD.getElement("footer").content.getUnparsed() == "ANY");
  }
  SECTION("XML with external file DTD_Validator and check values", "[XML][DTD_Validator][Parse]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
      "<!DOCTYPE note SYSTEM \"./files/note001.dtd\">\n"
      "<note>\n"
      "<to>Tove"
      "</to><from>Jani</from><heading>Reminder</heading><body>Don't forget me this weekend!</body>\n"
      "</note>\n"
    };
    xml.parse(source);
    DTD &xDTD = XRef<DTD>(xml.dtd());
    REQUIRE_FALSE(!isA<DTD>(xml.prolog().getChildren()[2]));
    REQUIRE(xDTD.getType() == DTD::Type::external);
    REQUIRE(xDTD.getRootName() == XRef<Element>(xml.root()).name());
    REQUIRE(xDTD.getExternalReference().getType() == "SYSTEM");
    REQUIRE(xDTD.getExternalReference().getSystemID() == "./files/note001.dtd");
    REQUIRE(xDTD.getElement("note").name == "note");
    REQUIRE(xDTD.getElement("note").content.getUnparsed() == "(to,from,heading,body)");
  }
  SECTION("XML with external URL DTD_Validator to parse and check values", "[XML][DTD_Validator][Parse]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
      "<!DOCTYPE html PUBLIC \"-//W3C//DTD_Validator XHTML 1.0 Transitional//EN\""
      " \"http://www.w3.org/TR/xhtml1/DTD_Validator/xhtml1-transitional.dtd\">\n"
      "<html></html>\n"
    };
    xml.parse(source);
    DTD &xDTD = XRef<DTD>(xml.dtd());
    REQUIRE_FALSE(!isA<DTD>(xml.prolog().getChildren()[2]));
    REQUIRE(xDTD.getType() == DTD::Type::external);
    REQUIRE(xDTD.getRootName() == XRef<Element>(xml.root()).name());
    REQUIRE(xDTD.getExternalReference().getType() == XMLExternalReference::kPublicID);
    REQUIRE(xDTD.getExternalReference().getSystemID() == "http://www.w3.org/TR/xhtml1/DTD_Validator/xhtml1-transitional.dtd");
    REQUIRE(xDTD.getExternalReference().getPublicID() == "-//W3C//DTD_Validator XHTML 1.0 Transitional//EN");
  }
}
TEST_CASE("Parse XML DTD_Validator with various element content specification errors.", "[XML][DTD_Validator][Parse][Error]")
{
  XML xml;
  SECTION("XML with a DTD_Validator that contains an illegal mixed content specification (#PCDATA doesn't come first).",
    "[XML][DTD_Validator][Parse][Error]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
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
      "</format>"
    };

    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error: Invalid content specification for element <format>.");
  }
  SECTION("XML with a DTD_Validator that contains an illegal mixed content specification (does not end with '*').",
    "[XML][DTD_Validator][Parse][Error]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
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
      "</format>"
    };
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error: Invalid content specification for element <format>.");
  }
  SECTION(
    "Parse XML with DTD_Validator that contains a content specification in error (missing ',').", "[XML][DTD_Validator][Parse][Error]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
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
      "</note>\n"
    };
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error: Invalid content specification for element <note>.");
  }
  SECTION("Parse XML with DTD_Validator that contains a content specification in error (missing element name).",
    "[XML][DTD_Validator][Parse][Error]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
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
      "</note>\n"
    };
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error: Invalid content specification for element <note>.");
  }
  SECTION("XML with a DTD_Validator that contains an illegal mixed content specification (uses ',').", "[XML][DTD_Validator][Parse][Error]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
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
      "</format>"
    };
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error: Invalid content specification for element <format>.");
  }
}
TEST_CASE("XML with a DTD_Validator conditional INCLUDE/IGNORE tags", "[XML][DTD_Validator][Parse][Conditional]")
{
  XML xml;
  SECTION("XML with a DTD_Validator conditional INCLUDE/IGNORE tags", "[XML][DTD_Validator][Parse][Conditional]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE root SYSTEM \"./files/conditional001.dtd\">\n"
      "<root>\n"
      "</root>"
    };
    REQUIRE_NOTHROW(xml.parse(source));
    DTD &xDTD = XRef<DTD>(xml.dtd());
    REQUIRE(xDTD.getType() == DTD::Type::external);
  }
  SECTION("XML with a DTD_Validator with conditional INCLUDE containing an entity.", "[XML][DTD_Validator][Parse][Conditional]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE root SYSTEM \"./files/conditional001.dtd\">\n"
      "<root>\n"
      "</root>"
    };
    REQUIRE_NOTHROW(xml.parse(source));
    DTD &xDTD = XRef<DTD>(xml.dtd());
    REQUIRE(xDTD.getType() == DTD::Type::external);
    REQUIRE(xDTD.getEntityMapper().getInternal("&example;") == "Joe Smith");
  }
  SECTION("XML with a DTD_Validator with invalid conditional value.", "[XML][DTD_Validator][Parse][Conditional]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE root SYSTEM \"./files/conditional002.dtd\">\n"
      "<root>\n"
      "</root>"
    };
    REQUIRE_THROWS_WITH(
      xml.parse(source), "XML Syntax Error [Line: 1 Column: 19] Conditional value not INCLUDE or IGNORE.");
  }
  SECTION("XML with a DTD_Validator with missing opening '[' from conditional.", "[XML][DTD_Validator][Parse][Conditional]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE root SYSTEM \"./files/conditional003.dtd\">\n"
      "<root>\n"
      "</root>"
    };
    REQUIRE_THROWS_WITH(
      xml.parse(source), "XML Syntax Error [Line: 1 Column: 23] Missing opening '[' from conditional.");
  }
  SECTION(
    "XML with a DTD_Validator with conditional controlled with a entity reference value (INCLUDE) containing an entity "
    "definition.",
    "[XML][DTD_Validator][Parse][Conditional]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE root SYSTEM \"./files/conditional004.dtd\">\n"
      "<root>\n"
      "</root>"
    };
    REQUIRE_NOTHROW(xml.parse(source));
    DTD &xDTD = XRef<DTD>(xml.dtd());
    REQUIRE(xDTD.getType() == DTD::Type::external);
    REQUIRE(xDTD.getEntityMapper().getInternal("&example;") == "Joe Smith");
  }
  SECTION("XML with a DTD_Validator with conditional controlled entity reference value (IGNORE) containing an entity definition.",
    "[XML][DTD_Validator][Parse][Conditional]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE root SYSTEM \"./files/conditional005.dtd\">\n"
      "<root>\n"
      "</root>"
    };
    REQUIRE_NOTHROW(xml.parse(source));
    DTD &xDTD = XRef<DTD>(xml.dtd());
    REQUIRE(xDTD.getType() == DTD::Type::external);
    REQUIRE_THROWS_WITH(xDTD.getEntityMapper().getInternal("&example;"),
      "EntityMapper Error: Internal entity reference not found for '&example;'.");
  }
  SECTION("XML with a DTD_Validator with nested conditionals that are both INCLUDE.", "[XML][DTD_Validator][Parse][Conditional]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE root SYSTEM \"./files/conditional006.dtd\">\n"
      "<root>\n"
      "</root>"
    };
    REQUIRE_NOTHROW(xml.parse(source));
    DTD &xDTD = XRef<DTD>(xml.dtd());
    REQUIRE(xDTD.getType() == DTD::Type::external);
    REQUIRE(xDTD.getEntityMapper().getInternal("&example;") == "Joe Smith");
  }
  SECTION(
    "XML with a DTD_Validator with nested conditionals that are both INCLUDE and two entities.", "[XML][DTD_Validator][Parse][Conditional]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE root SYSTEM \"./files/conditional007.dtd\">\n"
      "<root>\n"
      "</root>"
    };
    REQUIRE_NOTHROW(xml.parse(source));
    DTD &xDTD = XRef<DTD>(xml.dtd());
    REQUIRE(xDTD.getType() == DTD::Type::external);
    REQUIRE(xDTD.getEntityMapper().getInternal("&example;") == "Joe Smith");
    REQUIRE(xDTD.getEntityMapper().getInternal("&example1;") == "Joe Smith 1");
  }
  SECTION("XML with a DTD_Validator with nested conditionals that are  outter INCLUDE inner IGNORE plus two entities.",
    "[XML][DTD_Validator][Parse][Conditional]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE root SYSTEM \"./files/conditional008.dtd\">\n"
      "<root>\n"
      "</root>"
    };
    REQUIRE_NOTHROW(xml.parse(source));
    DTD &xDTD = XRef<DTD>(xml.dtd());
    REQUIRE(xDTD.getType() == DTD::Type::external);
    REQUIRE(xDTD.getEntityMapper().getInternal("&example1;") == "Joe Smith 1");
    REQUIRE_THROWS_WITH(xDTD.getEntityMapper().getInternal("&example;"),
      "EntityMapper Error: Internal entity reference not found for '&example;'.");
  }
  SECTION("XML with a DTD_Validator with nested conditionals that are  outter IGNORE inner INCLUDE plus two entities.",
    "[XML][DTD_Validator][Parse][Conditional]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE root SYSTEM \"./files/conditional009.dtd\">\n"
      "<root>\n"
      "</root>"
    };
    REQUIRE_NOTHROW(xml.parse(source));
    DTD &xDTD = XRef<DTD>(xml.dtd());
    REQUIRE(xDTD.getType() == DTD::Type::external);
    REQUIRE_THROWS_WITH(xDTD.getEntityMapper().getInternal("&example1;"),
      "EntityMapper Error: Internal entity reference not found for '&example1;'.");
    REQUIRE_THROWS_WITH(xDTD.getEntityMapper().getInternal("&example;"),
      "EntityMapper Error: Internal entity reference not found for '&example;'.");
  }
  SECTION(
    "XML with a DTD_Validator with nested conditionals controlled from internally defined DTD_Validator that is parsed first (switch on).",
    "[XML][DTD_Validator][Parse][Conditional]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE root SYSTEM \"./files/conditional010.dtd\" [ <!ENTITY % debug \"INCLUDE\"> ]>\n"
      "<root>\n"
      "</root>"
    };
    REQUIRE_NOTHROW(xml.parse(source));
    DTD &xDTD = XRef<DTD>(xml.dtd());
    REQUIRE(xDTD.getType() == (DTD::Type::internal | DTD::Type::external));
    REQUIRE(xDTD.getEntityMapper().getInternal("&example;") == "Joe Smith");
    REQUIRE(xDTD.getEntityMapper().getInternal("&example1;") == "Joe Smith 1");
  }
  SECTION(
    "XML with a DTD_Validator with nested conditionals controlled from internally defined DTD_Validator that is parsed first (switch off).",
    "[XML][DTD_Validator][Parse][Conditional]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE root SYSTEM \"./files/conditional010.dtd\" [ <!ENTITY % debug \"IGNORE\"> ]>\n"
      "<root>\n"
      "</root>"
    };
    REQUIRE_NOTHROW(xml.parse(source));
    DTD &xDTD = XRef<DTD>(xml.dtd());
    REQUIRE(xDTD.getType() == (DTD::Type::internal | DTD::Type::external));
    REQUIRE_THROWS_WITH(xDTD.getEntityMapper().getInternal("&example1;"),
      "EntityMapper Error: Internal entity reference not found for '&example1;'.");
    REQUIRE_THROWS_WITH(xDTD.getEntityMapper().getInternal("&example;"),
      "EntityMapper Error: Internal entity reference not found for '&example;'.");
  }
  SECTION(
    "XML with a DTD_Validator with nested conditionals controlled from internally defined DTD_Validator that is parsed first (invalid "
    "value).",
    "[XML][DTD_Validator][Parse][Conditional]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE root SYSTEM \"./files/conditional010.dtd\" [ <!ENTITY % debug \"IGNOE\"> ]>\n"
      "<root>\n"
      "</root>"
    };
    REQUIRE_THROWS_WITH(
      xml.parse(source), "XML Syntax Error [Line: 1 Column: 23] Conditional value not INCLUDE or IGNORE.");
  }
  SECTION(
    "XML with a DTD_Validator with nested conditionals controlled from internally defined DTD_Validator that is parsed first (novalue).",
    "[XML][DTD_Validator][Parse][Conditional]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE root SYSTEM \"./files/conditional010.dtd\">\n"
      "<root>\n"
      "</root>"
    };
     REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error: Entity '%debug;' does not exist.");
  }
}

TEST_CASE("Parse XML with more than DTD_Validator declaration", "[XML][DTD_Validator][Parse]")
{
  XML xml;
  SECTION("XML with more than one DTD_Validator declaration ", "[XML][DTD_Validator][Parse]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
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
      "</note>\n"
    };
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error [Line: 8 Column: 11] More than one DOCTYPE declaration.");
  }
}