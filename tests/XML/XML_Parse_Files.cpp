//
// Unit Tests: XML
//
// Description:
//

#include "XML_Tests.hpp"

using namespace XML_Lib;

TEST_CASE("Sample XML files to read and parse.", "[XML][Parse]")
{
  auto testFile = GENERATE(values<std::string>({ "./files/testfile001.xml",
    "./files/testfile002.xml",
    "./files/testfile003.xml",
    "./files/testfile005.xml",
    "./files/testfile007.xml",
    "./files/testfile010.xml",
    "./files/testfile011.xml" }));
  SECTION("Load file into buffer and parse.", "[XML][Parse]")
  {
    BufferSource source{ readXMLFromFileUTF8(testFile) };
    XML xml;
    REQUIRE_NOTHROW(xml.parse(source));
  }
  SECTION("Parse XML from file.", "[XML][Parse]")
  {
    FileSource source{ testFile };
    XML xml;
    REQUIRE_NOTHROW(xml.parse(source));
  }
}