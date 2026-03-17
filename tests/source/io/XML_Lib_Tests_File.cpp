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
    FileSource source{ prefixTestDataPath(file) };
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
    NRef<Declaration>(xml.declaration()).setEncoding("UTF-16");
    BufferDestination utf8xml;
    xml.stringify(utf8xml);
    std::string generatedFileName{ generateRandomFileName() };
    XML::toFile(generatedFileName, utf8xml.toString(), XML::Format::utf16BE);
    REQUIRE(XML::getFileFormat(generatedFileName) == XML::Format::utf16BE);
    xml.parse(BufferSource(XML::fromFile(generatedFileName)));
    REQUIRE(NRef<Declaration>(xml.declaration()).encoding() == "UTF-16");
    std::filesystem::remove(generatedFileName);
  }
  SECTION(
    "Check UTF-8 file with byte order mark load into buffer, parse then save it as UTF-16LE.", "[XML][File][Format]")
  {
    REQUIRE(XML::getFileFormat(prefixTestDataPath("testfile017.xml")) == XML::Format::utf8BOM);
    REQUIRE_NOTHROW(xml.parse(BufferSource(xml.fromFile(prefixTestDataPath("testfile017.xml")))));
    NRef<Declaration>(xml.declaration()).setEncoding("UTF-16");
    BufferDestination utf8xml;
    xml.stringify(utf8xml);
    std::string generatedFileName{ generateRandomFileName() };
    XML::toFile(generatedFileName, utf8xml.toString(), XML::Format::utf16LE);
    REQUIRE(XML::getFileFormat(generatedFileName) == XML::Format::utf16LE);
    xml.parse(BufferSource(XML::fromFile(generatedFileName)));
    REQUIRE(NRef<Declaration>(xml.declaration()).encoding() == "UTF-16");
    std::filesystem::remove(generatedFileName);
  }
  SECTION(
    "Check UTF-16BE file with byte order mark load into buffer, parse then save it as UTF-8.", "[XML][File][Format]")
  {
    REQUIRE(XML::getFileFormat(prefixTestDataPath("testfile018.xml")) == XML::Format::utf16BE);
    REQUIRE_NOTHROW(xml.parse(BufferSource(xml.fromFile(prefixTestDataPath("testfile018.xml")))));
    NRef<Declaration>(xml.declaration()).setEncoding("UTF-8");
    BufferDestination utf16xml;
    xml.stringify(utf16xml);
    std::string generatedFileName{ generateRandomFileName() };
    XML::toFile(generatedFileName, utf16xml.toString(), XML::Format::utf8BOM);
    REQUIRE(XML::getFileFormat(generatedFileName) == XML::Format::utf8BOM);
    xml.parse(BufferSource(XML::fromFile(generatedFileName)));
    REQUIRE(NRef<Declaration>(xml.declaration()).encoding() == "UTF-8");
    std::filesystem::remove(generatedFileName);
  }
  SECTION(
    "Check UTF-16LE file with byte order mark load into buffer, parse then save it as UTF-8.", "[XML][File][Format]")
  {
    REQUIRE(XML::getFileFormat(prefixTestDataPath("testfile019.xml")) == XML::Format::utf16LE);
    REQUIRE_NOTHROW(xml.parse(BufferSource(xml.fromFile(prefixTestDataPath("testfile019.xml")))));
    NRef<Declaration>(xml.declaration()).setEncoding("UTF-8");
    BufferDestination utf16xml;
    xml.stringify(utf16xml);
    std::string generatedFileName{ generateRandomFileName() };
    XML::toFile(generatedFileName, utf16xml.toString(), XML::Format::utf8BOM);
    REQUIRE(XML::getFileFormat(generatedFileName) == XML::Format::utf8BOM);
    xml.parse(BufferSource(XML::fromFile(generatedFileName)));
    REQUIRE(NRef<Declaration>(xml.declaration()).encoding() == "UTF-8");
    std::filesystem::remove(generatedFileName);
  }
  SECTION("Parse non-existent file throws FileSource::Error", "[XML][File][Error]")
  {
    REQUIRE_THROWS_AS(xml.parse(FileSource(prefixTestDataPath("doesntexist.xml"))), FileSource::Error);
  }
  SECTION("Parse empty file throws SyntaxError", "[XML][File][Error]")
  {
    // If empty.xml exists and is empty, this should throw a SyntaxError
    if (std::filesystem::exists(prefixTestDataPath("empty.xml"))
        && std::filesystem::file_size(prefixTestDataPath("empty.xml")) == 0) {
      REQUIRE_THROWS_AS(xml.parse(FileSource(prefixTestDataPath("empty.xml"))), SyntaxError);
    }
  }
  SECTION("Write to file and read back (roundtrip)", "[XML][File][Output]")
  {
    std::string xmlString = "<root><child>value</child></root>";
    std::string generatedFileName{ generateRandomFileName() };
    XML::toFile(generatedFileName, xmlString, XML::Format::utf8);
    REQUIRE(XML::getFileFormat(generatedFileName) == XML::Format::utf8);
    auto readBack = XML::fromFile(generatedFileName);
    REQUIRE(readBack.find("<child>value</child>") != std::string::npos);
    std::filesystem::remove(generatedFileName);
  }
  SECTION("FileDestination throws on unwritable location", "[XML][File][Output][Error]")
  {
    // Try to write to a location that should not be writable (root on Unix, or invalid path on Windows)
    std::string badFile = "/this/path/should/not/exist/test.xml";
#ifdef _WIN32
    badFile = "Z:/this/path/should/not/exist/test.xml";
#endif
    REQUIRE_THROWS_AS(FileDestination(badFile), FileDestination::Error);
  }
}