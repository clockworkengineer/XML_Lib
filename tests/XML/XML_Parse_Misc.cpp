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
using namespace XML_Lib;
// ==========
// Test cases
// ==========
TEST_CASE("Parse UTF-16 encoded files.", "[XML][Parse][UTF16]")
{
  std::string xmlString;
  SECTION("Parse UTF16 encoded file LE ", "[XML][Parse][UTF16]")
  {
    BufferSource source(readXMLFromFileUTF16("./files/testfile008.xml"));
    XML xml;
    REQUIRE_NOTHROW(xml.parse(source));
  }
  SECTION("Parse UTF16 encoded file BE ", "[XML][Parse][UTF16]")
  {
    BufferSource source(readXMLFromFileUTF16("./files/testfile009.xml"));
    XML xml;
    REQUIRE_NOTHROW(xml.parse(source));
  }
}
TEST_CASE("Use name for accessing elements", "[XML][Access][ByName]")
{
  std::string xmlString;
  SECTION("Address book access", "[XML][Access][ByName]")
  {
    xmlString =
      "<?xml version=\"1.0\"?>"
      "<AddressBook>"
      "<Address>"
      "Flat A, West Road, Wolverhampton, W1SSX9"
      "</Address>"
      "</AddressBook>";
    BufferSource source{ xmlString };
    XML xml;
    xml.parse(source);
    REQUIRE(XNodeRef<XNodeElement>(xml.root()).name() == "AddressBook");
    REQUIRE(XNodeRef<XNodeElement>(xml.root()["Address"]).name() == "Address");
    REQUIRE(XNodeRef<XNodeElement>(xml.root()["Address"]).getContents() == "Flat A, West Road, Wolverhampton, W1SSX9");
  }
}
TEST_CASE("Make sure whitespace is whitespace.", "[XML][Access][ByName]")
{
  std::string xmlString;
  SECTION("Content node only whitespace if it contains ONLY whitespace.", "[XML][Parse][Whitespace]")
  {
    xmlString =
      "<?xml version=\"1.0\"?>\n"
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
    BufferSource source{ xmlString };
    XML xml;
    xml.parse(source);
    REQUIRE(XNodeRef<XNode>(*xml.root().children[0]).getNodeType() == XNodeType::content);
    REQUIRE(XNodeRef<XNodeContent>(*xml.root().children[0]).isWhiteSpace() == true);
    REQUIRE(XNodeRef<XNode>((*xml.root().children[1])[0]).getNodeType() == XNodeType::content);
    REQUIRE(XNodeRef<XNodeContent>((*xml.root().children[1])[0]).isWhiteSpace() == false);
    REQUIRE(XNodeRef<XNode>((*xml.root().children[2])[0]).getNodeType() == XNodeType::content);
    REQUIRE(XNodeRef<XNodeContent>((*xml.root().children[2])[0]).isWhiteSpace() == true);
    REQUIRE(XNodeRef<XNode>((*xml.root().children[3])[0]).getNodeType() == XNodeType::content);
    REQUIRE(XNodeRef<XNodeContent>((*xml.root().children[3])[0]).isWhiteSpace() == false);
    REQUIRE(XNodeRef<XNode>((*xml.root().children[3])[1]).getNodeType() == XNodeType::entity);
    REQUIRE(XNodeRef<XNode>((*xml.root().children[3])[2]).getNodeType() == XNodeType::content);
    REQUIRE(XNodeRef<XNodeContent>((*xml.root().children[3])[2]).isWhiteSpace() == false);
    REQUIRE(XNodeRef<XNode>((*xml.root().children[4])[0]).getNodeType() == XNodeType::content);
    REQUIRE(XNodeRef<XNodeContent>((*xml.root().children[4])[0]).isWhiteSpace() == false);
    REQUIRE(XNodeRef<XNode>((*xml.root().children[4])[1]).getNodeType() == XNodeType::cdata);
    REQUIRE(XNodeRef<XNode>((*xml.root().children[4])[2]).getNodeType() == XNodeType::content);
    REQUIRE(XNodeRef<XNodeContent>((*xml.root().children[4])[2]).isWhiteSpace() == false);
  }
}
TEST_CASE("Check R-Value reference parse/stringify.", "[XML][XMLNode][R-Value Reference]")
{
  XML xml;
  SECTION("Parse with R-Value reference (Buffer).", "[XML][XMLNode][R-Value Reference]")
  {
    xml.parse(
      BufferSource{ "<?xml version=\"1.0\"?>"
                    "<AddressBook>"
                    "<Address>"
                    "Flat A, West Road, Wolverhampton, W1SSX9"
                    "</Address>"
                    "</AddressBook>" });
    REQUIRE(XNodeRef<XNodeElement>(xml.root()).name() == "AddressBook");
    REQUIRE(XNodeRef<XNodeElement>(xml.root()["Address"]).name() == "Address");
    REQUIRE(XNodeRef<XNodeElement>(xml.root()["Address"]).getContents() == "Flat A, West Road, Wolverhampton, W1SSX9");
  }
  SECTION("Parse/Stringify both with R-Value reference (File).", "[XML][MLNode][R-Value Reference]")
  {
    std::filesystem::remove(prefixPath(kGeneratedXMLFile));
    xml.parse(FileSource{ prefixPath(kSingleXMLFile) });
    xml.stringify(FileDestination{ prefixPath(kGeneratedXMLFile) });
    REQUIRE(readXMLFromFileUTF8(prefixPath(kGeneratedXMLFile)) == readXMLFromFileUTF8(prefixPath(kSingleXMLFile)));
  }
}