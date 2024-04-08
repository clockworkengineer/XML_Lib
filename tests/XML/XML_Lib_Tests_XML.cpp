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
    REQUIRE_THROWS(xml.prolog(), "XML Error: No XML has been parsed.");
  }
  SECTION("Check declaration with no parsed XML.", "[XML][Declaration][No XML]")
  {
    REQUIRE_THROWS(xml.declaration(), "XML Error: No XML has been parsed.");
  }
  SECTION("Check root with no parsed XML.", "[XML][Root][No XML]")
  {
    REQUIRE_THROWS(xml.root(), "XML Error: No XML has been parsed.");
  }
  SECTION("Check DTD with no parsed XML.", "[XML][DTD][No XML]")
  {
    REQUIRE_THROWS(xml.dtd(), "XML Error: No XML has been parsed.");
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
}