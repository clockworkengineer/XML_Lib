//
// Unit Tests: XML
//
// Description:
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
TEST_CASE("Parse XML elements with attached attributes", "[XML][Parse][Attributes]")
{
  std::string xmlString;
  SECTION("Root attribute with one attached attribute number", "[XML][Parse][[Attributes]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<AddressBook number='15'>\n"
                "</AddressBook>\n";
    BufferSource source { xmlString };
    XML xml;
    xml.parse(source);
    REQUIRE(xml.prolog().getAttribute("version").value.parsed == "1.0");
    REQUIRE(xml.prolog().getAttribute("encoding").value.parsed == "UTF-8");
    REQUIRE(xml.prolog().getAttribute("standalone").value.parsed == "no");
    REQUIRE(xml.prolog().children.size() == 3);
    REQUIRE(xml.prolog()[0].elementName == "AddressBook");
    REQUIRE(xml.prolog()[0].getAttributeList().size() == 1);
    REQUIRE(xml.prolog()[0].getAttribute("number").value.parsed == "15");
  }
  SECTION("Root attribute with 3 attached attributes number, away, flat", "[XML][Parse][[Attributes]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                " <AddressBook number='15' away=\"yes\" flat='no'>\n"
                " </AddressBook>\n";
    BufferSource source { xmlString };
    XML xml;
    xml.parse(source);
    REQUIRE(xml.prolog().getAttribute("version").value.parsed == "1.0");
    REQUIRE(xml.prolog().getAttribute("encoding").value.parsed == "UTF-8");
    REQUIRE(xml.prolog().getAttribute("standalone").value.parsed == "no");
    REQUIRE(xml.prolog().children.size() == 3);
    REQUIRE(xml.prolog()[0].elementName == "AddressBook");
    REQUIRE(xml.prolog()[0].getAttributeList().size() == 3);
    REQUIRE(xml.prolog()[0].getAttribute("number").value.parsed == "15");
    REQUIRE(xml.prolog()[0].getAttribute("away").value.parsed == "yes");
    REQUIRE(xml.prolog()[0].getAttribute("flat").value.parsed == "no");
  }
  SECTION("Empty elements with attributes are allowed.", "[XML][Parse][[Attributes]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<AddressBook number='15'>\n"
                "</AddressBook>\n";
    BufferSource source { xmlString };
    XML xml;
    REQUIRE_NOTHROW(xml.parse(source));
  }
  SECTION("Element with duplicate attributes not allowed.", "[XML][Parse][[Attributes]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<AddressBook number='15' colour='red' number='16'>\n"
                " </AddressBook>\n";
    BufferSource source { xmlString };
    XML xml;
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error [Line: 2 Column: 54] Attribute defined more than once within start tag.");
  }
}