#include "XML_Lib_Tests.hpp"

TEST_CASE("Sample XML files to read and parse.", "[XML][Parse]")
{
  TEST_FILE_LIST(file);
  XML xml;
  SECTION("Load file into buffer and parse.", "[XML][Parse][Buffer]")
  {
    BufferSource source{ XML::fromFile(prefixTestDataPath((file))) };
    REQUIRE_NOTHROW(xml.parse(source));
  }
  SECTION("Parse XML from file.", "[XML][Parse][File]")
  {
    FileSource source{  prefixTestDataPath(file) };
    REQUIRE_NOTHROW(xml.parse(source));
  }
}

TEST_CASE("Check file format API.", "[XML][File][Format]")
{
  XML xml;
  SECTION("Check UTF-8 file.", "[XML][File][Format]")
  {
    REQUIRE(XML::getFileFormat(prefixTestDataPath("testfile016.xml")) == XML::Format::utf8);
  }
  SECTION("Check UTF-8 file with byte order mark.", "[XML][File][Format]")
  {
    REQUIRE(XML::getFileFormat(prefixTestDataPath("testfile017.xml")) == XML::Format::utf8BOM);
  }
  SECTION("Check UTF-16BE file with byte order mark.", "[XML][File][Format]")
  {
    REQUIRE(XML::getFileFormat(prefixTestDataPath("testfile018.xml")) == XML::Format::utf16BE);
  }
  SECTION("Check UTF-16LE file with byte order mark.", "[XML][File][Format]")
  {
    REQUIRE(XML::getFileFormat(prefixTestDataPath("testfile019.xml")) == XML::Format::utf16LE);
  }
  SECTION("Check UTF-8 file and parse.", "[XML][File][Format]")
  {
    REQUIRE(XML::getFileFormat(prefixTestDataPath("testfile016.xml")) == XML::Format::utf8);
    REQUIRE_NOTHROW(xml.parse(FileSource(prefixTestDataPath("testfile016.xml"))));
  }
  SECTION("Check UTF-8 file with byte order mark and parse.", "[XML][File][Format]")
  {
    REQUIRE(XML::getFileFormat(prefixTestDataPath("testfile017.xml")) == XML::Format::utf8BOM);
    REQUIRE_THROWS_WITH(xml.parse(FileSource(prefixTestDataPath("testfile017.xml"))),
      "XML Syntax Error [Line: 1 Column: 1] Content detected before root element.");
  }
  SECTION("Check UTF-16BE file with byte order mark and parse.", "[XML][File][Format]")
  {
    REQUIRE(XML::getFileFormat(prefixTestDataPath("testfile018.xml")) == XML::Format::utf16BE);
    REQUIRE_THROWS_WITH(xml.parse(FileSource(prefixTestDataPath("testfile017.xml"))),
      "XML Syntax Error [Line: 1 Column: 1] Content detected before root element.");
  }
  SECTION("Check UTF-16LE file with byte order mark and parse.", "[XML][File][Format]")
  {
    REQUIRE(XML::getFileFormat(prefixTestDataPath("testfile019.xml")) == XML::Format::utf16LE);
    REQUIRE_THROWS_WITH(xml.parse(FileSource(prefixTestDataPath("testfile017.xml"))),
      "XML Syntax Error [Line: 1 Column: 1] Content detected before root element.");
  }
  SECTION("Check UTF-8 file with byte order mark load into buffer and parse.", "[XML][File][Format]")
  {
    REQUIRE(XML::getFileFormat(prefixTestDataPath("testfile017.xml")) == XML::Format::utf8BOM);
    REQUIRE_NOTHROW(xml.parse(BufferSource(xml.fromFile(prefixTestDataPath("testfile017.xml")))));
  }
  SECTION("Check UTF-16BE file with byte order mark load into buffer and parse.", "[XML][File][Format]")
  {
    REQUIRE(XML::getFileFormat(prefixTestDataPath("testfile018.xml")) == XML::Format::utf16BE);
    REQUIRE_NOTHROW(xml.parse(BufferSource(xml.fromFile(prefixTestDataPath("testfile018.xml")))));
  }
  SECTION("Check UTF-16LE file with byte order mark load into buffer and parse.", "[XML][File][Format]")
  {
    REQUIRE(XML::getFileFormat(prefixTestDataPath("testfile019.xml")) == XML::Format::utf16LE);
    REQUIRE_NOTHROW(xml.parse(BufferSource(xml.fromFile(prefixTestDataPath("testfile019.xml")))));
  }
  SECTION(
    "Check UTF-8 file with byte order mark load into buffer, parse then save it as UTF-16BE.", "[XML][File][Format]")
  {
    REQUIRE(XML::getFileFormat(prefixTestDataPath("testfile017.xml")) == XML::Format::utf8BOM);
    REQUIRE_NOTHROW(xml.parse(BufferSource(xml.fromFile(prefixTestDataPath("testfile017.xml")))));
    XRef<Declaration>(xml.declaration()).setEncoding("UTF-16");
    BufferDestination utf8xml;
    xml.stringify(utf8xml);
    XML::toFile(kGeneratedXMLFile, utf8xml.toString(), XML::Format::utf16BE);
    REQUIRE(XML::getFileFormat(kGeneratedXMLFile) == XML::Format::utf16BE);
    xml.parse(BufferSource(XML::fromFile(kGeneratedXMLFile)));
    REQUIRE(XRef<Declaration>(xml.declaration()).encoding() == "UTF-16");
  }
  SECTION(
    "Check UTF-8 file with byte order mark load into buffer, parse then save it as UTF-16LE.", "[XML][File][Format]")
  {
    REQUIRE(XML::getFileFormat(prefixTestDataPath("testfile017.xml")) == XML::Format::utf8BOM);
    REQUIRE_NOTHROW(xml.parse(BufferSource(xml.fromFile(prefixTestDataPath("testfile017.xml")))));
    XRef<Declaration>(xml.declaration()).setEncoding("UTF-16");
    BufferDestination utf8xml;
    xml.stringify(utf8xml);
    XML::toFile(kGeneratedXMLFile, utf8xml.toString(), XML::Format::utf16LE);
    REQUIRE(XML::getFileFormat(kGeneratedXMLFile) == XML::Format::utf16LE);
    xml.parse(BufferSource(XML::fromFile(kGeneratedXMLFile)));
    REQUIRE(XRef<Declaration>(xml.declaration()).encoding() == "UTF-16");
  }
  SECTION(
    "Check UTF-16BE file with byte order mark load into buffer, parse then save it as UTF-8.", "[XML][File][Format]")
  {
    REQUIRE(XML::getFileFormat(prefixTestDataPath("testfile018.xml")) == XML::Format::utf16BE);
    REQUIRE_NOTHROW(xml.parse(BufferSource(xml.fromFile(prefixTestDataPath("testfile018.xml")))));
    XRef<Declaration>(xml.declaration()).setEncoding("UTF-8");
    BufferDestination utf16xml;
    xml.stringify(utf16xml);
    XML::toFile(kGeneratedXMLFile, utf16xml.toString(), XML::Format::utf8BOM);
    REQUIRE(XML::getFileFormat(kGeneratedXMLFile) == XML::Format::utf8BOM);
    xml.parse(BufferSource(XML::fromFile(kGeneratedXMLFile)));
    REQUIRE(XRef<Declaration>(xml.declaration()).encoding() == "UTF-8");
  }
  SECTION(
    "Check UTF-16LE file with byte order mark load into buffer, parse then save it as UTF-8.", "[XML][File][Format]")
  {
    REQUIRE(XML::getFileFormat(prefixTestDataPath("testfile019.xml")) == XML::Format::utf16LE);
    REQUIRE_NOTHROW(xml.parse(BufferSource(xml.fromFile(prefixTestDataPath("testfile019.xml")))));
    XRef<Declaration>(xml.declaration()).setEncoding("UTF-8");
    BufferDestination utf16xml;
    xml.stringify(utf16xml);
    XML::toFile(kGeneratedXMLFile, utf16xml.toString(), XML::Format::utf8BOM);
    REQUIRE(XML::getFileFormat(kGeneratedXMLFile) == XML::Format::utf8BOM);
    xml.parse(BufferSource(XML::fromFile(kGeneratedXMLFile)));
    REQUIRE(XRef<Declaration>(xml.declaration()).encoding() == "UTF-8");
  }
}