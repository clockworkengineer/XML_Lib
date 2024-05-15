//
// Unit Tests: XML_Lib_Tests_XML
//
// Description:
//

#include "XML_Lib_Tests.hpp"

using namespace XML_Lib;
TEST_CASE("Check XML top level apis.", "[XML][Top Level][API]")
{
  XML xml;
  SECTION("Check XML Lib version.", "[XML][XNode][Version]") { REQUIRE(xml.version() == "XML_Lib Version 1.0.0"); }
  SECTION("Check prolog with no parsed XML.", "[XML][Prolog][No XML]")
  {
    REQUIRE_THROWS((void)xml.prolog(), "XML Error: No XML has been parsed.");
  }
  SECTION("Check declaration with no parsed XML.", "[XML][Declaration][No XML]")
  {
    REQUIRE_THROWS((void)xml.declaration(), "XML Error: No XML has been parsed.");
  }
  SECTION("Check root with no parsed XML.", "[XML][Root][No XML]")
  {
    REQUIRE_THROWS((void)xml.root(), "XML Error: No XML has been parsed.");
  }
  SECTION("Check DTD with no parsed XML.", "[XML][DTD][No XML]")
  {
    REQUIRE_THROWS((void)xml.dtd(), "XML Error: No XML has been parsed.");
  }
  SECTION("Check stringify with no parsed XML.", "[XML][Stringify][No XML]")
  {
    BufferDestination destination;
    REQUIRE_THROWS(xml.stringify(destination), "XML Error: No XML has been parsed.");
  }
  SECTION("Access root element attribute.", "[XML][API][Attribute]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<root attr1='1'></root>\n"
    };
    xml.parse(source);
    auto &xRoot = XRef<XElement>(xml.root());
    REQUIRE(xRoot.getAttributeList().size() == 1);
    REQUIRE(xRoot["attr1"].getName() == "attr1");
    REQUIRE(xRoot["attr1"].getParsed() == "1");
  }
  SECTION("Access non existant element attribute.", "[XML][API][Attribute]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<root attr2='2'></root>\n"
    };
    xml.parse(source);
    auto &xRoot = XRef<XElement>(xml.root());
    REQUIRE(xRoot.getAttributeList().size() == 1);
    REQUIRE_THROWS_WITH(xRoot["attr1"].getName(), "XNode Error: Attribute 'attr1' does not exist.");
  }
  SECTION("Access root element multiple attributea.", "[XML][API][Attribute]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<root attr1='1' attr2='2' attr3='3'></root>\n"
    };
    xml.parse(source);
    auto &xRoot = XRef<XElement>(xml.root());
    REQUIRE(xRoot.getAttributeList().size() == 3);
    REQUIRE(xRoot["attr1"].getName() == "attr1");
    REQUIRE(xRoot["attr1"].getParsed() == "1");
    REQUIRE(xRoot["attr2"].getName() == "attr2");
    REQUIRE(xRoot["attr2"].getParsed() == "2");
    REQUIRE(xRoot["attr3"].getName() == "attr3");
    REQUIRE(xRoot["attr3"].getParsed() == "3");
  }
  SECTION("Use name for accessing elements", "[XML][API][ByName]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>"
      "<AddressBook>"
      "<Address>"
      "Flat A, West Road, Wolverhampton, W1SSX9"
      "</Address>"
      "</AddressBook>"
    };
    xml.parse(source);
    REQUIRE(XRef<XElement>(xml.root()).name() == "AddressBook");
    REQUIRE(XRef<XElement>(xml.root()["Address"]).name() == "Address");
    REQUIRE(XRef<XElement>(xml.root()["Address"]).getContents() == "Flat A, West Road, Wolverhampton, W1SSX9");
  }
  SECTION("Use name for accessing elements with other node types in play", "[XML][API][ByName]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>"
      "<AddressBook>"
      "<first/>"
      "Content "
      "Content "
      "<!-- comment -->"
      "<!-- comment -->"
      "<!-- comment -->"
      "<Address>"
      "Flat A, West Road, Wolverhampton, W1SSX9"
      "</Address>"
      "</AddressBook>"
    };
    xml.parse(source);
    REQUIRE(XRef<XElement>(xml.root()).name() == "AddressBook");
    REQUIRE(XRef<XElement>(xml.root()["Address"]).name() == "Address");
    REQUIRE(XRef<XElement>(xml.root()["Address"]).getContents() == "Flat A, West Road, Wolverhampton, W1SSX9");
  }
  SECTION("Use name for accessing nested elements", "[XML][API][ByName]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>"
      "<AddressBook>"
      "<Name><First>John</First> <Surname>Doe</Surname></Name>"
      "<Address> "
      "Flat A, West Road, Wolverhampton, W1SSX9"
      "</Address>"
      "</AddressBook>"
    };
    xml.parse(source);
    REQUIRE(XRef<XElement>(xml.root()).name() == "AddressBook");
    REQUIRE(XRef<XElement>(xml.root()["Name"]["First"]).getContents() == "John");
    REQUIRE(XRef<XElement>(xml.root()["Name"]["Surname"]).getContents() == "Doe");
    REQUIRE(XRef<XElement>(xml.root()["Name"]).getContents() == "John Doe");
    REQUIRE(XRef<XElement>(xml.root()).getContents() == "John Doe Flat A, West Road, Wolverhampton, W1SSX9");
  }
}