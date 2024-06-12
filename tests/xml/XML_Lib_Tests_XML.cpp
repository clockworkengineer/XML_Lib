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
  SECTION("Check XML Lib version.", "[XML][XNode][Version]") { REQUIRE(xml.version() == "XML_Lib Version 1.1.0"); }
  SECTION("Check prolog with no parsed XML.", "[XML][Prolog][No XML]")
  {
    XML xml1;
    REQUIRE_THROWS((void)xml1.prolog(), "XML Error: No XML has been parsed.");
  }
  SECTION("Check declaration with no parsed XML.", "[XML][Declaration][No XML]")
  {
    XML xml1;
    REQUIRE_THROWS((void)xml1.declaration(), "XML Error: No XML has been parsed.");
  }
  SECTION("Check root with no parsed XML.", "[XML][Root][No XML]")
  {
    XML xml1;
    REQUIRE_THROWS((void)xml1.root(), "XML Error: No XML has been parsed.");
  }
  SECTION("Check DTD with no parsed XML.", "[XML][DTD][No XML]")
  {
    XML xml1;
    REQUIRE_THROWS((void)xml1.dtd(), "XML Error: No XML has been parsed.");
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
    REQUIRE(xRoot.getAttributes().size() == 1);
    REQUIRE(xRoot["attr1"].getName() == "attr1");
    REQUIRE(xRoot["attr1"].getParsed() == "1");
  }
  SECTION("Access non existent element attribute.", "[XML][API][Attribute]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<root attr2='2'></root>\n"
    };
    xml.parse(source);
    auto &xRoot = XRef<XElement>(xml.root());
    REQUIRE(xRoot.getAttributes().size() == 1);
    REQUIRE_THROWS_WITH(xRoot["attr1"].getName(), "XNode Error: Attribute 'attr1' does not exist.");
  }
  SECTION("Access root element multiple attributes.", "[XML][API][Attribute]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<root attr1='1' attr2='2' attr3='3'></root>\n"
    };
    xml.parse(source);
    auto &xRoot = XRef<XElement>(xml.root());
    REQUIRE(xRoot.getAttributes().size() == 3);
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
TEST_CASE("Check XML creation/read apis.", "[XML][Creation][API]")
{
  SECTION("Create XML from passed string to constructor.", "[XML][Creation][Constructor]")
  {
    const XML xml{ "<root>test content</root>" };
    BufferDestination destination;
    xml.stringify(destination);
    REQUIRE(
      destination.toString() == R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?><root>test content</root>)");
  }
  SECTION("Create XML from assigned string.", "[XML][Creation][Assignment]")
  {
    XML xml;
    xml = "<root>test content</root>";
    BufferDestination destination;
    xml.stringify(destination);
    REQUIRE(
      destination.toString() == R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?><root>test content</root>)");
  }
  SECTION("Create XML and read attributes.", "[XML][Attribute][Read]")
  {
    XML xml;
    xml = "<root first='1' second='2'>test content</root>";
    BufferDestination destination;
    REQUIRE(XRef<XElement>(xml.root())["first"].getUnparsed() == "1");
    REQUIRE(XRef<XElement>(xml.root())["second"].getUnparsed() == "2");
  }
  SECTION("Create XML and read attributes with wrong name", "[XML][Attribute][Read]")
  {
    XML xml;
    xml = "<root first='1' second='2'>test content</root>";
    BufferDestination destination;
    REQUIRE_THROWS_WITH(XRef<XElement>(xml.root())["frst"], "XNode Error: Attribute 'frst' does not exist.");
  }
  SECTION("Create XML and read attributes and namespaces", "[XML][Attribute][Read]")
  {
    XML xml;
    xml = "<root first='1' second='2' xmlns:f='http://www.w3.org/TR/html4/'>test content</root>";
    BufferDestination destination;
    REQUIRE(XRef<XElement>(xml.root())["first"].getUnparsed() == "1");
    REQUIRE(XRef<XElement>(xml.root())["second"].getUnparsed() == "2");
    REQUIRE(XRef<XElement>(xml.root())["xmlns:f"].getUnparsed() == "http://www.w3.org/TR/html4/");
  }
  SECTION("A root document defining one namespace that is inherited by a child", "[XML][Parse][Namespace]")
  {
    XML xml;
    BufferSource source{
      "<root xmlns:h='http://www.w3.org/TR/html4/'>\n"
      "<h:table><h:tr><h:td>Apples</h:td><h:td>Bananas</h:td></h:tr></h:table>\n"
      "</root>\n"
    };
    REQUIRE_NOTHROW(xml.parse(source));
    auto &xRoot = XRef<XElement>(xml.root());
    REQUIRE(xRoot[0].name() == "h:table");
    REQUIRE(xRoot[0].getNameSpaces().size() == 1);
    REQUIRE(xRoot[0]["xmlns:h"].getUnparsed() == "http://www.w3.org/TR/html4/");
    REQUIRE(xRoot[0][0].getNameSpaces().size() == 1);
    REQUIRE(xRoot[0][0].name() == "h:tr");
    REQUIRE(xRoot[0][0]["xmlns:h"].getUnparsed() == "http://www.w3.org/TR/html4/");
    REQUIRE(xRoot[0][0][0].getNameSpaces().size() == 1);
    REQUIRE(xRoot[0][0][0].name() == "h:td");
    REQUIRE(xRoot[0][0][0].getContents() == "Apples");
    REQUIRE(xRoot[0][0][0]["xmlns:h"].getUnparsed() == "http://www.w3.org/TR/html4/");
    REQUIRE(xRoot[0][0][1].name() == "h:td");
    REQUIRE(xRoot[0][0][1].getContents() == "Bananas");
    REQUIRE(xRoot[0][0][1]["xmlns:h"].getUnparsed() == "http://www.w3.org/TR/html4/");
  }
}