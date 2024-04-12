//
// Unit Tests: XML_Lib_Tests_Parse_Files
//
// Description:
//

#include "XML_Lib_Tests.hpp"

using namespace XML_Lib;

TEST_CASE("Sample XML files to read and parse.", "[XML][Parse][File]")
{
  auto testFile = GENERATE(values<std::string>({ "./files/testfile001.xml",
    "./files/testfile002.xml",
    "./files/testfile003.xml",
    "./files/testfile005.xml",
    "./files/testfile007.xml",
    "./files/testfile010.xml",
    "./files/testfile011.xml",
    "./files/testfile012.xml",
    "./files/testfile013.xml",
    "./files/testfile014.xml",
    "./files/testfile015.xml" }));
  XML xml;
  SECTION("Load file into buffer and parse.", "[XML][Parse]")
  {
    BufferSource source{ readXMLFromFileUTF8(testFile) };
    REQUIRE_NOTHROW(xml.parse(source));
  }
  SECTION("Parse XML from file.", "[XML][Parse][File]")
  {
    FileSource source{ testFile };
    REQUIRE_NOTHROW(xml.parse(source));
  }
}

TEST_CASE("Check file format API.", "[XML][File][Format]")
{
  XML xml;
  SECTION("Check UTF-8 file.", "[XML][File][Format]")
  {
    auto format = XML::getFileFormat("./files/testfile016.xml");
    REQUIRE(format == XML::Format::utf8);
  }
  SECTION("Check UTF-8 file with byte order mark.", "[XML][File][Format]")
  {
    auto format = XML::getFileFormat("./files/testfile017.xml");
    REQUIRE(format == XML::Format::utf8BOM);
  }
  SECTION("Check UTF-16BE file with byte order mark.", "[XML][File][Format]")
  {
    auto format = XML::getFileFormat("./files/testfile018.xml");
    REQUIRE(format == XML::Format::utf16BE);
  }
  SECTION("Check UTF-16LE file with byte order mark.", "[XML][File][Format]")
  {
    auto format = XML::getFileFormat("./files/testfile019.xml");
    REQUIRE(format == XML::Format::utf16LE);
  }
  SECTION("Check UTF-8 file and parse.", "[XML][File][Format]")
  {
    auto format = XML::getFileFormat("./files/testfile016.xml");
    REQUIRE(format == XML::Format::utf8);
    REQUIRE_NOTHROW(xml.parse(FileSource("./files/testfile016.xml")));
  }
  SECTION("Check UTF-8 file with byte order mark and parse.", "[XML][File][Format]")
  {
    auto format = XML::getFileFormat("./files/testfile017.xml");
    REQUIRE(format == XML::Format::utf8BOM);
    REQUIRE_THROWS_WITH(xml.parse(FileSource("./files/testfile017.xml")),
      "XML Syntax Error [Line: 1 Column: 1] Content detected before root element.");
  }
  SECTION("Check UTF-16BE file with byte order mark.", "[XML][File][Format]")
  {
    auto format = XML::getFileFormat("./files/testfile018.xml");
    REQUIRE(format == XML::Format::utf16BE);
    REQUIRE_THROWS_WITH(xml.parse(FileSource("./files/testfile017.xml")),
      "XML Syntax Error [Line: 1 Column: 1] Content detected before root element.");
  }
  SECTION("Check UTF-16LE file with byte order mark.", "[XML][File][Format]")
  {
    auto format = XML::getFileFormat("./files/testfile019.xml");
    REQUIRE(format == XML::Format::utf16LE);
    REQUIRE_THROWS_WITH(xml.parse(FileSource("./files/testfile017.xml")),
      "XML Syntax Error [Line: 1 Column: 1] Content detected before root element.");
  }
}