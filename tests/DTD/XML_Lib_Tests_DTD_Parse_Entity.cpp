//
// Unit Tests: XML_Lib_Tests_DTD_Parse_Entity
//
// Description: Unit tests for DTD parsing.
//

#include "XML_Lib_Tests.hpp"

using namespace XML_Lib;

TEST_CASE("Parse XML with internal DTD that contains entity definitions and uses", "[XML][DTD][Parse][Entity]")
{
  XML xml;
  SECTION("XML DTD with entity definitions and uses", "[XML][DTD][Parse]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
      "<!DOCTYPE note [\n"
      "<!ENTITY nbsp \"&#xA0;\">\n"
      "<!ENTITY writer \"Writer: Donald Duck.\">\n"
      "<!ENTITY copyright \"Copyright: W3Schools.\">]>\n"
      "<note>\n"
      "<to>Tove</to><from>Jani</from><heading>Reminder</heading>\n"
      "<body>Don't forget me this weekend!</body><footer>&writer;&nbsp;&copyright;</footer>\n"
      "</note>\n"
    };
    REQUIRE_NOTHROW(xml.parse(source));
  }
  SECTION("XML DTD with entity internal definitions and uses. Check translation of entity values",
    "[XML][DTD][Parse][Entity]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
      "<!DOCTYPE note [\n"
      "<!ENTITY nbsp \"&#xA0;\">\n"
      "<!ENTITY writer \"Writer: Donald Duck.\">\n"
      "<!ENTITY copyright \"Copyright: W3Schools.\">]>\n"
      "<note>\n"
      "<to>Tove</to><from>Jani</from><heading>Reminder</heading>\n"
      "<body>Don't forget me this weekend!</body><footer>&writer;&nbsp;&copyright;</footer>\n"
      "</note>\n"
    };
    xml.parse(source);
    XDTD &xDTD = XRef<XDTD>(xml.dtd());
    REQUIRE_FALSE(!xml.prolog().getChildren()[2].isDTD());
    REQUIRE(xDTD.getType() == XDTD::Type::internal);
    REQUIRE_FALSE(!xml.root().isRoot());
    REQUIRE(XRef<XElement>(xml.root())[4].name() == "footer");
    REQUIRE(XRef<XElement>(xml.root())[4].getContents()
            == reinterpret_cast<const char *>(u8"Writer: Donald Duck.\u00A0Copyright: W3Schools."));
  }
  SECTION("XML DTD with entity and how it deals with entity character expansion case 1)", "[XML][DTD][Parse][Entity]")
  {

    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
      "<!DOCTYPE note [\n"
      "<!ENTITY example \"<p>An ampersand (&#38;#38;) may be escaped numerically (&#38;#38;#38;) or with a general "
      "entity (&amp;amp;).</p>\">]>\n"
      "<note>&example;</note>\n"
    };
    xml.parse(source);
    XDTD &xDTD = XRef<XDTD>(xml.dtd());
    REQUIRE_FALSE(!xml.prolog().getChildren()[2].isDTD());
    REQUIRE(xDTD.getType() == XDTD::Type::internal);
    REQUIRE(
      xDTD.getEntityMapper().getInternal("&example;")
      == "<p>An ampersand (&#38;) may be escaped numerically (&#38;#38;) or with a general entity (&amp;amp;).</p>");
    REQUIRE(XRef<XElement>(xml.root())[0].name() == "p");
    REQUIRE(XRef<XElement>(xml.root())[0].getContents()
            == "An ampersand (&) may be escaped numerically (&#38;) or with a general entity (&amp;).");
  }
  SECTION("XML DTD with entity and how it deals with entity character expansion case 2)", "[XML][DTD][Parse][Entity]")
  {
    BufferSource source{
      "<?xml version='1.0'?>\n"
      "<!DOCTYPE test [\n"
      "<!ELEMENT test (#PCDATA) >\n"
      "<!ENTITY % xx '&#37;zz;'>\n"
      "<!ENTITY % zz '&#60;!ENTITY tricky \"error-prone\" >' >\n"
      "%xx;\n"
      "]>\n"
      "<test>This sample shows a &tricky; method.</test>\n"
    };
    xml.parse(source);
    XDTD &xDTD = XRef<XDTD>(xml.dtd());
    REQUIRE_FALSE(!xml.prolog().getChildren()[2].isDTD());
    REQUIRE(xDTD.getType() == XDTD::Type::internal);
    REQUIRE(xDTD.getEntityMapper().getInternal("%xx;") == "%zz;");
    REQUIRE(xDTD.getEntityMapper().getInternal("%zz;") == "<!ENTITY tricky \"error-prone\" >");
    REQUIRE(XRef<XElement>(xml.root()).name() == "test");
    REQUIRE(XRef<XElement>(xml.root()).getContents() == "This sample shows a error-prone method.");
  }
  SECTION("XML DTD with entity and how it deals with entity character expansion case 3)", "[XML][DTD][Parse][Entity]")
  {
    BufferSource source{
      "<?xml version='1.0'?>\n"
      "<!DOCTYPE foo [\n"
      "<!ENTITY x \"&lt;\">\n"
      "]>\n"
      "<foo attr=\"&x;\"/>\n"
    };
    xml.parse(source);
    XDTD &xDTD = XRef<XDTD>(xml.dtd());
    REQUIRE_FALSE(!xml.prolog().getChildren()[2].isDTD());
    REQUIRE(xDTD.getType() == XDTD::Type::internal);
    REQUIRE(xDTD.getEntityMapper().getInternal("&x;") == "&lt;");
    REQUIRE(XRef<XElement>(xml.root()).getAttributeList().size() == 1);
    auto &attribute = XRef<XElement>(xml.root())["attr"];
    REQUIRE(attribute.getName() == "attr");
    REQUIRE(attribute.getUnparsed() == "&x;");
    REQUIRE(attribute.getParsed() == "&lt;");
  }
  // This should throw an error as & ' " < >  not allowed to be assigned to attribute directly (NEED TO FIX)
  SECTION("XML DTD with entity and how it deals with entity character expansion case 4)", "[XML][DTD][Parse][Entity]")
  {

    BufferSource source{
      "<?xml version='1.0'?>\n"
      "<!DOCTYPE foo [\n"
      "<!ENTITY x \"&#60;\">\n"
      "]>\n"
      "<foo attr=\"&x;\"/>\n"
    };

    REQUIRE_THROWS_WITH(xml.parse(source),
      "XML Syntax Error [Line: 5 Column: 21] Attribute value contains invalid character '<', '\"', ''' or '&'.");
  }
  SECTION("XML DTD with entity used within an entity.", "[XML][DTD][Parse][Entity]")
  {
    BufferSource source{
      "<?xml version='1.0'?>\n"
      "<!DOCTYPE author [\n"
      "<!ELEMENT author (#PCDATA)>\n"
      "<!ENTITY email \"josmith@theworldaccordingtojosmith.com\">\n"
      "<!--the following use of a general entity is legal \n"
      "if it is used in the XML document-->\n"
      "<!ENTITY js \"Jo Smith &email;\">\n"
      "]>\n"
      "<author>&js;</author>\n"
    };
    xml.parse(source);
    XDTD &xDTD = XRef<XDTD>(xml.dtd());
    REQUIRE_FALSE(!xml.prolog().getChildren()[2].isDTD());
    REQUIRE(xDTD.getType() == XDTD::Type::internal);
    REQUIRE(xDTD.getEntityMapper().getInternal("&js;") == "Jo Smith &email;");
    REQUIRE(XRef<XElement>(xml.root()).name() == "author");
    REQUIRE_FALSE(!xml.root().getChildren()[0].isEntity());
    REQUIRE(XRef<XEntityReference>(XRef<XElement>(xml.root()).getChildren()[0]).getContents()
            == "Jo Smith josmith@theworldaccordingtojosmith.com");
    REQUIRE(XRef<XElement>(xml.root()).getContents() == "Jo Smith josmith@theworldaccordingtojosmith.com");
  }
  SECTION("XML DTD with entity used within an entity with recursion.", "[XML][DTD][Parse]")
  {
    BufferSource source{
      "<?xml version='1.0'?>\n"
      "<!DOCTYPE author [\n"
      "<!ELEMENT author (#PCDATA)>\n"
      "<!ENTITY email \"josmith@theworldaccordingtojosmith.com\">\n"
      "<!--the two entity"
      " statements are infinitely recursive-->\n"
      "<!ENTITY email \"user@user.com &js;\">\n"
      "<!ENTITY js \"Jo Smith &email;\">\n"
      "]>\n"
      "<author>&js;</author>\n"
    };
    REQUIRE_THROWS_WITH(
      xml.parse(source), "XML Syntax Error: Entity '&email;' contains recursive definition which is not allowed.");
  }
  SECTION("XML DTD with entity that contains a character reference that is parsed straight away.",
    "[XML][DTD][Parse][Entity]")
  {
    BufferSource source{
      "<?xml version='1.0'?>\n"
      "<!DOCTYPE author [\n"
      "<!ELEMENT author (#PCDATA)>\n"
      "<!ENTITY email \"josmith&#x40;theworldaccordingtojosmith.com\">\n"
      "<!ENTITY js \"Jo Smith &email;\">\n"
      "]>\n"
      "<author>&js;</author>\n"
    };
    xml.parse(source);
    XDTD &xDTD = XRef<XDTD>(xml.dtd());
    REQUIRE_FALSE(!xml.prolog().getChildren()[2].isDTD());
    REQUIRE(xDTD.getType() == XDTD::Type::internal);
    REQUIRE(xDTD.getEntityMapper().getInternal("&email;") == "josmith@theworldaccordingtojosmith.com");
  }
  SECTION("XML DTD with entity that is defined externally (file name.txt).", "[XML][DTD][Parse][Entity]")
  {
    BufferSource source{
      "<?xml version='1.0'?>\n"
      "<!DOCTYPE foo [\n"
      "<!ELEMENT foo ANY>\n"
      "<!ENTITY name SYSTEM \"./files/name.txt\">\n"
      "]>\n"
      "<foo>Hello &name;</foo>\n"
    };
    xml.parse(source);
    XDTD &xDTD = XRef<XDTD>(xml.dtd());
    REQUIRE_FALSE(!xml.prolog().getChildren()[2].isDTD());
    REQUIRE(xDTD.getType() == XDTD::Type::internal);
    REQUIRE(xDTD.getRootName() == XRef<XElement>(xml.root()).name());
    REQUIRE(XRef<XElement>(xml.root()).name() == "foo");
    REQUIRE(XRef<XElement>(xml.root()).getContents() == "Hello John Joe Doe");
  }
  SECTION("XML DTD with entity that is defined externally (file that does not exist).", "[XML][DTD][Parse][Entity]")
  {
    BufferSource source{
      "<?xml version='1.0'?>\n"
      "<!DOCTYPE foo [\n"
      "<!ELEMENT foo ANY>\n"
      "<!ENTITY name SYSTEM \"./files/unknown.txt\">\n"
      "]>\n"
      "<foo>Hello &name;</foo>\n"
    };
    REQUIRE_THROWS_WITH(
      xml.parse(source), "XML Syntax Error: Entity '&name;' source file './files/unknown.txt' does not exist.");
  }
  SECTION("XML with internal DTD with parameter entities to parse  (internal cannot appear within tags).",
    "[XML][DTD][Parse][Entity]")
  {
    BufferSource source{
      "<!DOCTYPE REPORT [\n"
      "<!ENTITY % empty_report \"<!ELEMENT REPORT EMPTY>\">"
      "%empty_report;\n"
      "]>\n"
      "<REPORT></REPORT>\n"
    };
    REQUIRE_NOTHROW(xml.parse(source));
  }
  SECTION(
    "XML with internal DTD with parameter entities to parse and check values (internal cannot appear within tags).",
    "[XML][DTD][Parse][Entity]")
  {
    BufferSource source{
      "<!DOCTYPE REPORT [\n"
      "<!ENTITY % empty_report \"<!ELEMENT REPORT EMPTY>\">"
      "%empty_report;\n"
      "]>\n"
      "<REPORT></REPORT>\n"
    };
    xml.parse(source);
    XDTD &xDTD = XRef<XDTD>(xml.dtd());
    REQUIRE_FALSE(!xml.prolog().getChildren()[1].isDTD());
    REQUIRE(xDTD.getType() == XDTD::Type::internal);
    REQUIRE(xDTD.getRootName() == XRef<XElement>(xml.root()).name());
    REQUIRE(xDTD.getRootName() == "REPORT");
    REQUIRE(xDTD.getEntityMapper().getInternal("%empty_report;") == "<!ELEMENT REPORT EMPTY>");
    REQUIRE(xDTD.getElement("REPORT").name == "REPORT");
    REQUIRE(xDTD.getElement("REPORT").content.getParsed() == "EMPTY");
  }
  SECTION("XML with external DTD with parameter entities to parse.", "[XML][DTD][Parse][Entity]")
  {
    BufferSource source{
      "<!DOCTYPE REPORT SYSTEM \"./files/report01.dtd\">\n"
      "<REPORT></REPORT>\n"
    };
    REQUIRE_NOTHROW(xml.parse(source));
    XDTD &xDTD = XRef<XDTD>(xml.dtd());
    REQUIRE(xDTD.getType() == XDTD::Type::external);
  }

  SECTION("XML with external DTD with both types of entities to parse an check values", "[XML][DTD][Parse][Entity]")
  {
    BufferSource source{
      "<!DOCTYPE REPORT SYSTEM \"./files/report01.dtd\">"
      "<REPORT></REPORT>\n"
    };
    xml.parse(source);
    XDTD &xDTD = XRef<XDTD>(xml.dtd());
    REQUIRE_FALSE(!xml.prolog().getChildren()[1].isDTD());
    REQUIRE(xDTD.getType() == XDTD::Type::external);
    REQUIRE(xDTD.getRootName() == XRef<XElement>(xml.root()).name());
    REQUIRE(xDTD.getRootName() == "REPORT");
    REQUIRE(xDTD.getEntityMapper().getInternal("%contact;") == "phone");
    REQUIRE(xDTD.getEntityMapper().getInternal("%area;") == "name, street, pincode, city");
    REQUIRE(xDTD.getElement("REPORT").name == "REPORT");
    REQUIRE(xDTD.getElement("REPORT").content.getUnparsed() == "(residence|apartment|office|shop)*");
    REQUIRE(xDTD.getElement("residence").name == "residence");
    REQUIRE(xDTD.getElement("residence").content.getUnparsed() == "(name, street, pincode, city, phone)");
    REQUIRE(xDTD.getElement("apartment").name == "apartment");
    REQUIRE(xDTD.getElement("apartment").content.getUnparsed() == "(name, street, pincode, city, phone)");
    REQUIRE(xDTD.getElement("office").name == "office");
    REQUIRE(xDTD.getElement("office").content.getUnparsed() == "(name, street, pincode, city, phone)");
    REQUIRE(xDTD.getElement("shop").name == "shop");
    REQUIRE(xDTD.getElement("shop").content.getUnparsed() == "(name, street, pincode, city, phone)");
  }
  SECTION("XML DTD with parameter entity within general entity.", "[XML][DTD][Parse][Entity]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
      "<!DOCTYPE note SYSTEM \"./files/note002.dtd\">\n"
      "<note>"
      "&signature;"
      "</note>\n"
    };
    xml.parse(source);
    XDTD &xDTD = XRef<XDTD>(xml.dtd());
    REQUIRE_FALSE(!xml.prolog().getChildren()[2].isDTD());
    REQUIRE(xDTD.getType() == XDTD::Type::external);
    REQUIRE_FALSE(!xml.root().isRoot());
    REQUIRE(xDTD.getEntityMapper().getInternal("&signature;") == "© 1999 Yoyodyne, Inc. &legal;");
    REQUIRE(XRef<XElement>(xml.root()).getContents() == "© 1999 Yoyodyne, Inc. All Rights Reserved.");
  }
}