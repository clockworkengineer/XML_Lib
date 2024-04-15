//
// Unit Tests: XML_Lib_Tests_Parse_Misc
//
// Description:
//

#include "XML_Lib_Tests.hpp"

using namespace XML_Lib;

TEST_CASE("Parse UTF-16 encoded files.", "[XML][Parse][UTF16]")
{
  XML xml;
  SECTION("Parse UTF16 encoded file LE ", "[XML][Parse][UTF16]")
  {
    BufferSource source(XML::fromFile("./files/testfile008.xml"));
    REQUIRE_NOTHROW(xml.parse(source));
  }
  // SECTION("Parse UTF16 encoded file BE ", "[XML][Parse][UTF16]")
  // {
  //   BufferSource source(XML::fromFile("./files/testfile009.xml"));
  //   REQUIRE_NOTHROW(xml.parse(source));
  // }
}
TEST_CASE("Make sure whitespace is whitespace.", "[XML][Access][ByName]")
{
  XML xml;
  SECTION("Content node only whitespace if it contains ONLY whitespace.", "[XML][Parse][Whitespace]")
  {
    BufferSource source{
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
      "</AddressBook>"
    };

    xml.parse(source);
    auto &xRootChildren = xml.root().getChildren();
    REQUIRE_FALSE(!xRootChildren[0].isContent());
    REQUIRE(XRef<XContent>(xml.root().getChildren()[0]).isWhiteSpace() == true);
    REQUIRE_FALSE(!xRootChildren[1].getChildren()[0].isContent());
    REQUIRE(XRef<XContent>((xRootChildren[1])[0]).isWhiteSpace() == false);
    REQUIRE_FALSE(!xRootChildren[2].getChildren()[0].isContent());
    REQUIRE(XRef<XContent>((xRootChildren[2])[0]).isWhiteSpace() == true);
    REQUIRE_FALSE(!xRootChildren[3].getChildren()[0].isContent());
    REQUIRE(XRef<XContent>((xRootChildren[3])[0]).isWhiteSpace() == false);
    REQUIRE_FALSE(!xRootChildren[3].getChildren()[1].isEntity());
    REQUIRE_FALSE(!xRootChildren[3].getChildren()[2].isContent());
    REQUIRE(XRef<XContent>((xRootChildren[3])[2]).isWhiteSpace() == false);
    REQUIRE_FALSE(!xRootChildren[4].getChildren()[0].isContent());
    REQUIRE(XRef<XContent>((xRootChildren[4])[0]).isWhiteSpace() == false);
    REQUIRE_FALSE(!xRootChildren[4].getChildren()[1].isCDATA());
    REQUIRE_FALSE(!xRootChildren[4].getChildren()[2].isContent());
    REQUIRE(XRef<XContent>((xRootChildren[4])[2]).isWhiteSpace() == false);
  }
}
TEST_CASE("Check R-Value reference parse/stringify.", "[XML][XNode][R-Value Reference]")
{
  XML xml;
  SECTION("Parse with R-Value reference (Buffer).", "[XML][XNode][R-Value Reference]")
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
    REQUIRE(XML::fromFile(prefixPath(kGeneratedXMLFile)) == XML::fromFile(prefixPath(kSingleXMLFile)));
  }
}