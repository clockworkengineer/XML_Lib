//
// Unit Tests: XML
//
// Description:
//

#include "XML_Tests.hpp"

using namespace XML_Lib;

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
    REQUIRE(XRef<XElement>(xml.root()).name() == "AddressBook");
    REQUIRE(XRef<XElement>(xml.root()["Address"]).name() == "Address");
    REQUIRE(XRef<XElement>(xml.root()["Address"]).getContents() == "Flat A, West Road, Wolverhampton, W1SSX9");
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
    REQUIRE(xml.root().getChildren()[0]->getType() == Variant::Type::content);
    REQUIRE(XRef<XContent>(*xml.root().getChildren()[0]).isWhiteSpace() == true);
    REQUIRE(xml.root().getChildren()[1]->getChildren()[0]->getType() == Variant::Type::content);
    REQUIRE(XRef<XContent>((*xml.root().getChildren()[1])[0]).isWhiteSpace() == false);
    REQUIRE(xml.root().getChildren()[2]->getChildren()[0]->getType() == Variant::Type::content);
    REQUIRE(XRef<XContent>((*xml.root().getChildren()[2])[0]).isWhiteSpace() == true);
    REQUIRE(xml.root().getChildren()[3]->getChildren()[0]->getType() == Variant::Type::content);
    REQUIRE(XRef<XContent>((*xml.root().getChildren()[3])[0]).isWhiteSpace() == false);
    REQUIRE(xml.root().getChildren()[3]->getChildren()[1]->getType() == Variant::Type::entity);
    REQUIRE(xml.root().getChildren()[3]->getChildren()[2]->getType() == Variant::Type::content);
    REQUIRE(XRef<XContent>((*xml.root().getChildren()[3])[2]).isWhiteSpace() == false);
    REQUIRE(xml.root().getChildren()[4]->getChildren()[0]->getType() == Variant::Type::content);
    REQUIRE(XRef<XContent>((*xml.root().getChildren()[4])[0]).isWhiteSpace() == false);
    REQUIRE(xml.root().getChildren()[4]->getChildren()[1]->getType() == Variant::Type::cdata);
    REQUIRE(xml.root().getChildren()[4]->getChildren()[2]->getType() == Variant::Type::content);
    REQUIRE(XRef<XContent>((*xml.root().getChildren()[4])[2]).isWhiteSpace() == false);
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
    REQUIRE(XRef<XElement>(xml.root()).name() == "AddressBook");
    REQUIRE(XRef<XElement>(xml.root()["Address"]).name() == "Address");
    REQUIRE(XRef<XElement>(xml.root()["Address"]).getContents() == "Flat A, West Road, Wolverhampton, W1SSX9");
  }
  SECTION("Parse/Stringify both with R-Value reference (File).", "[XML][MLNode][R-Value Reference]")
  {
    std::filesystem::remove(prefixPath(kGeneratedXMLFile));
    xml.parse(FileSource{ prefixPath(kSingleXMLFile) });
    xml.stringify(FileDestination{ prefixPath(kGeneratedXMLFile) });
    REQUIRE(readXMLFromFileUTF8(prefixPath(kGeneratedXMLFile)) == readXMLFromFileUTF8(prefixPath(kSingleXMLFile)));
  }
}