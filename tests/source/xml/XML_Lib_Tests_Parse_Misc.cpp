#include "XML_Lib_Tests.hpp"

TEST_CASE("Parse UTF-16 encoded files.", "[XML][Parse][UTF16]")
{
  const XML xml;
  SECTION("Parse UTF16 encoded file LE ", "[XML][Parse][UTF16]")
  {
    BufferSource source(XML::fromFile(prefixTestDataPath("testfile008.xml")));
    REQUIRE_NOTHROW(xml.parse(source));
  }
  SECTION("Parse UTF16 encoded file BE ", "[XML][Parse][UTF16]")
  {
    BufferSource source(XML::fromFile(prefixTestDataPath("testfile009.xml")));
    REQUIRE_NOTHROW(xml.parse(source));
  }
}
TEST_CASE("Make sure whitespace is whitespace.", "[XML][Access][ByName]")
{
  SECTION("Content node only whitespace if it contains ONLY whitespace.", "[XML][Parse][Whitespace]")
  {
    XML xml;
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
    REQUIRE_FALSE(!isA<Content>(xRootChildren[0]));
    REQUIRE(NRef<Content>(xml.root().getChildren()[0]).isWhiteSpace() == true);
    REQUIRE_FALSE(!isA<Content>(xRootChildren[1].getChildren()[0]));
    REQUIRE(NRef<Content>((xRootChildren[1])[0]).isWhiteSpace() == false);
    REQUIRE_FALSE(!isA<Content>(xRootChildren[2].getChildren()[0]));
    REQUIRE(NRef<Content>((xRootChildren[2])[0]).isWhiteSpace() == true);
    REQUIRE_FALSE(!isA<Content>(xRootChildren[3].getChildren()[0]));
    REQUIRE(NRef<Content>((xRootChildren[3])[0]).isWhiteSpace() == false);
    REQUIRE_FALSE(!isA<EntityReference>(xRootChildren[3].getChildren()[1]));
    REQUIRE_FALSE(!isA<Content>(xRootChildren[3].getChildren()[2]));
    REQUIRE(NRef<Content>((xRootChildren[3])[2]).isWhiteSpace() == false);
    REQUIRE_FALSE(!isA<Content>(xRootChildren[4].getChildren()[0]));
    REQUIRE(NRef<Content>((xRootChildren[4])[0]).isWhiteSpace() == false);
    REQUIRE_FALSE(!isA<CDATA>(xRootChildren[4].getChildren()[1]));
    REQUIRE_FALSE(!isA<Content>(xRootChildren[4].getChildren()[2]));
    REQUIRE(NRef<Content>((xRootChildren[4])[2]).isWhiteSpace() == false);
  }
}
TEST_CASE("Check R-Value reference parse/stringify.", "[XML][Node][R-Value Reference]")
{
  const XML xml;
  SECTION("Parse with R-Value reference (Buffer).", "[XML][Node][R-Value Reference]")
  {
    xml.parse(
      BufferSource{ "<?xml version=\"1.0\"?>"
                    "<AddressBook>"
                    "<Address>"
                    "Flat A, West Road, Wolverhampton, W1SSX9"
                    "</Address>"
                    "</AddressBook>" });
    REQUIRE(NRef<Element>(xml.root()).name() == "AddressBook");
    REQUIRE(NRef<Element>(xml.root()["Address"]).name() == "Address");
    REQUIRE(NRef<Element>(xml.root()["Address"]).getContents() == "Flat A, West Road, Wolverhampton, W1SSX9");
  }
  SECTION("Parse/Stringify both with R-Value reference (File).", "[XML][MLNode][R-Value Reference]")
  {
    std::string generatedFileName{ generateRandomFileName() };
    xml.parse(FileSource{ prefixTestDataPath(kSingleXMLFile) });
    xml.stringify(FileDestination{ generatedFileName });
    REQUIRE(XML::fromFile(generatedFileName) == XML::fromFile(prefixTestDataPath(kSingleXMLFile)));
    std::filesystem::remove(generatedFileName);
  }
}