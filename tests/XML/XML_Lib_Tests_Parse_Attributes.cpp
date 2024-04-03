//
// Unit Tests: XML_Lib_Tests_Parse_Attributes
//
// Description: Parse XML elements with attached attributes.
//

#include "XML_Lib_Tests.hpp"

using namespace XML_Lib;

TEST_CASE("Parse XML elements with attached attributes", "[XML][Parse][Attributes]")
{
  XML xml;
  SECTION("Root element with one attached attribute number", "[XML][Parse][[Attributes]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<AddressBook number='15'>\n"
      "</AddressBook>\n"
    };
    xml.parse(source);
    auto &xDeclaration = XRef<XDeclaration>(xml.declaration());
    auto &xRoot = XRef<XElement>(xml.root());
    REQUIRE(xDeclaration.version() == "1.0");
    REQUIRE(xDeclaration.encoding() == "UTF-8");
    REQUIRE(xDeclaration.standalone() == "no");
    REQUIRE(xml.prolog().getChildren().size() == 4);
    REQUIRE(xRoot.name() == "AddressBook");
    REQUIRE(xRoot.getAttributeList().size() == 1);
    REQUIRE(xRoot.getAttribute("number").getParsed() == "15");
  }
  SECTION("Root element with 3 attached attributes number, away, flat", "[XML][Parse][[Attributes]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      " <AddressBook number='15' away=\"yes\" flat='no'>\n"
      " </AddressBook>\n"
    };
    xml.parse(source);
    auto &xDeclaration = XRef<XDeclaration>(xml.declaration());
    auto &xRoot = XRef<XElement>(xml.root());
    REQUIRE(xDeclaration.version() == "1.0");
    REQUIRE(xDeclaration.encoding() == "UTF-8");
    REQUIRE(xDeclaration.standalone() == "no");
    REQUIRE(xml.prolog().getChildren().size() == 4);
    REQUIRE(xRoot.name() == "AddressBook");
    REQUIRE(xRoot.getAttributeList().size() == 3);
    REQUIRE(xRoot.getAttribute("number").getParsed() == "15");
    REQUIRE(xRoot.getAttribute("away").getParsed() == "yes");
    REQUIRE(xRoot.getAttribute("flat").getParsed() == "no");
  }
  SECTION("Empty elements with attributes are allowed.", "[XML][Parse][[Attributes]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<AddressBook number='15'>\n"
      "</AddressBook>\n"
    };
    REQUIRE_NOTHROW(xml.parse(source));
  }
  SECTION("Element with duplicate attributes not allowed.", "[XML][Parse][[Attributes]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<AddressBook number='15' colour='red' number='16'>\n"
      " </AddressBook>\n"
    };
    REQUIRE_THROWS_WITH(
      xml.parse(source), "XML Syntax Error: Attribute 'number' defined more than once within start tag.");
  }
  SECTION("Element that has an attribute with no value.", "[XML][Parse][[Attributes]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<AddressBook number=>\n"
      "</AddressBook>\n"
    };
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error [Line: 2 Column: 25] Invalid attribute value.");
  }
  SECTION("Element that has an invalid attribute name.", "[XML][Parse][[Attributes]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<AddressBook 1number='15'>\n"
      "</AddressBook>\n"
    };
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error [Line: 2 Column: 25] Invalid name '1number' encountered.");
  }
  SECTION("Element that has an attribute that uses double quotes for value.", "[XML][Parse][[Attributes]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<AddressBook number=\"15\">\n"
      "</AddressBook>\n"
    };
    REQUIRE_NOTHROW(xml.parse(source));
  }
  SECTION("Element that has an attribute that uses double quotes in value.", "[XML][Parse][[Attributes]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<gangster name='George \"Shotgun\" Ziegler'></gangster>\n"
    };
    REQUIRE_NOTHROW(xml.parse(source));
    REQUIRE(XRef<XElement>(xml.root()).getAttribute("name").getParsed() == "George \"Shotgun\" Ziegler");
  }
  SECTION("Element that has an attribute that uses single quotes in value.", "[XML][Parse][[Attributes]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<gangster name=\"George 'Shotgun' Ziegler\"></gangster>\n"
    };
    REQUIRE_NOTHROW(xml.parse(source));
    REQUIRE(XRef<XElement>(xml.root()).getAttribute("name").getParsed() == "George 'Shotgun' Ziegler");
  }
  SECTION("Element that has an attribute that uses single quotes (&quot;) in value.", "[XML][Parse][[Attributes]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<gangster name=\"George &quot;Shotgun&quot; Ziegler\"></gangster>\n"
    };
    REQUIRE_NOTHROW(xml.parse(source));
    REQUIRE(XRef<XElement>(xml.root()).getAttribute("name").getParsed() == "George &#x22;Shotgun&#x22; Ziegler");
  }
}