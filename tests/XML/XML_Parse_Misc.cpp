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
TEST_CASE("Parse UTF-16 encoded files.", "[XML][Parse][UTF16]")
{
  std::string xmlString;
  SECTION("Parse UTF16 encoded file LE ", "[XML][Parse][UTF16]")
  {
    BufferSource source(readXMLFromFileUTF16("./testData/testfile008.xml"));
    XML xml;
    REQUIRE_NOTHROW(xml.parse(source));
  }
  SECTION("Parse UTF16 encoded file BE ", "[XML][Parse][UTF16]")
  {
    BufferSource source(readXMLFromFileUTF16("./testData/testfile009.xml"));
    XML xml;
    REQUIRE_NOTHROW(xml.parse(source));
  }
}
TEST_CASE("Use name for accessing elements", "[XML][Access][ByName]")
{
  std::string xmlString;
  SECTION("Address book access", "[XML][Access][ByName]")
  {
    xmlString = "<?xml version=\"1.0\"?>"
                "<AddressBook>"
                "<Address>"
                "Flat A, West Road, Wolverhampton, W1SSX9"
                "</Address>"
                "</AddressBook>";
    BufferSource source{xmlString};
    XML xml;
    xml.parse(source);
    REQUIRE(XMLNodeRef<XMLNodeElement>(xml.root()).name() == "AddressBook");
    REQUIRE(XMLNodeRef<XMLNodeElement>(xml.root()["Address"]).name() == "Address");
    REQUIRE(XMLNodeRef<XMLNodeElement>(xml.root()["Address"]).getContents() == "Flat A, West Road, Wolverhampton, W1SSX9");
  }
}
TEST_CASE("Make sure whitespace is whitespace.", "[XML][Access][ByName]")
{
  std::string xmlString;
  SECTION("Content node only whitespace if it contains ONLY whitespace.", "[XML][Parse][Whitespace]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<AddressBook>\n"
                "<Address>\n  \n"
                "Flat A, West Road, Wolverhampton, W1SSX9"
                "</Address>"
                "<Address>\n"
                "        "
                "</Address>"
                "<Address>\n"
                "&amp;        "
                "</Address>"
                "<Address>\n"
                "<![CDATA[<message> Welcome to TutorialsPoint </message>   ]]>        "
                "</Address>"
                "<Address>\n"
                "&amp;        "
                "</Address>"
                "</AddressBook>";
    BufferSource source{xmlString};
    XML xml;
    xml.parse(source);
    REQUIRE(XMLNodeRef<XMLNode>(*xml.root().children[0]).getNodeType() == XMLNodeType::content);
    REQUIRE(XMLNodeRef<XMLNodeContent>(*xml.root().children[0]).isWhiteSpace() == true);
    REQUIRE(XMLNodeRef<XMLNode>((*xml.root().children[1])[0]).getNodeType() == XMLNodeType::content);
    REQUIRE(XMLNodeRef<XMLNodeContent>((*xml.root().children[1])[0]).isWhiteSpace() == false);
    REQUIRE(XMLNodeRef<XMLNode>((*xml.root().children[2])[0]).getNodeType() == XMLNodeType::content);
    REQUIRE(XMLNodeRef<XMLNodeContent>((*xml.root().children[2])[0]).isWhiteSpace() == true);
    REQUIRE(XMLNodeRef<XMLNode>((*xml.root().children[3])[0]).getNodeType() == XMLNodeType::content);
    REQUIRE(XMLNodeRef<XMLNodeContent>((*xml.root().children[3])[0]).isWhiteSpace() == false);
    REQUIRE(XMLNodeRef<XMLNode>((*xml.root().children[3])[1]).getNodeType() == XMLNodeType::entity);
    REQUIRE(XMLNodeRef<XMLNode>((*xml.root().children[3])[2]).getNodeType() == XMLNodeType::content);
    REQUIRE(XMLNodeRef<XMLNodeContent>((*xml.root().children[3])[2]).isWhiteSpace() == false);
    REQUIRE(XMLNodeRef<XMLNode>((*xml.root().children[4])[0]).getNodeType() == XMLNodeType::content);
    REQUIRE(XMLNodeRef<XMLNodeContent>((*xml.root().children[4])[0]).isWhiteSpace() == false);
    REQUIRE(XMLNodeRef<XMLNode>((*xml.root().children[4])[1]).getNodeType() == XMLNodeType::cdata);
    REQUIRE(XMLNodeRef<XMLNode>((*xml.root().children[4])[2]).getNodeType() == XMLNodeType::content);
    REQUIRE(XMLNodeRef<XMLNodeContent>((*xml.root().children[4])[2]).isWhiteSpace() == false);
  }
}