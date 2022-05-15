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
TEST_CASE("Sample XML files to read and parse.", "[XML][Parse]")
{
  auto testFile = GENERATE(values<std::string>({"./testData/testfile001.xml",
                                                "./testData/testfile002.xml",
                                                "./testData/testfile003.xml",
                                                "./testData/testfile005.xml",
                                                "./testData/testfile007.xml",
                                                "./testData/testfile010.xml",
                                                "./testData/testfile011.xml"}));
  SECTION("Load file into buffer and parse.", "[XML][Parse]")
  {
    BufferSource source { readXMLFromFileUTF8(testFile) };
    XML xml;
    REQUIRE_NOTHROW(xml.parse(source));
  }
  SECTION("Parse XML from file.", "[XML][Parse]")
  {
    FileSource source { testFile };
    XML xml;
    REQUIRE_NOTHROW(xml.parse(source));
  }
}