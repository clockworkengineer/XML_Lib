//
// Unit Tests: XML_Lib_Tests_IDestination
//
// Description: Unit tests for BufferDestination and FileDestination classes
// that implement the IDestination interfaces used in XML parsing.
//

#include "XML_Lib_Tests.hpp"
// =======================
// XML class namespace
// =======================
using namespace XML_Lib;

TEST_CASE("Creation and use of IDestination (Buffer) interface.", "[XML][Parse][BufferDestination]")
{
  SECTION("Create BufferDestination.", "[XML][Stringify][IDestination]") { REQUIRE_NOTHROW(BufferDestination()); }
  SECTION("Create BufferDestination and get buffer which should be empty.", "[XML][Stringify][BufferDestination]")
  {
    BufferDestination buffer;
    REQUIRE_FALSE(!buffer.getBuffer().empty());
  }
  SECTION("Create BufferDestination and add one character.", "[XML][Stringify][BufferDestination]")
  {
    BufferDestination buffer;
    buffer.add("i");
    REQUIRE(buffer.getBuffer().size() == 1);
  }
  SECTION(
    "Create BufferDestination and add an stringified integer and check result.", "[XML][Stringify][BufferDestination]")
  {
    BufferDestination buffer;
    buffer.add("65767");
    REQUIRE(buffer.getBuffer().size() == 5);
    REQUIRE(buffer.getBuffer() == ("65767"));
  }
}
TEST_CASE("Creation and use of IDestination (File) interface.", "[XML][Parse][FileDestination]")
{
  SECTION("Create FileDestination.", "[XML][Stringify][FileDestination]")
  {
    std::filesystem::remove(prefixPath(kGeneratedXMLFile));
    REQUIRE_NOTHROW(FileDestination(prefixPath(kGeneratedXMLFile)));
  }
  SECTION("Create FileDestination when file already exists.", "[XML][Stringify][FileDestination]")
  {
    FileDestination file(prefixPath(kGeneratedXMLFile));
    REQUIRE_NOTHROW(FileDestination(prefixPath(kGeneratedXMLFile)));
  }
  SECTION("Create FileDestination and test file exists and should be empty.", "[XML][Stringify][FileDestination]")
  {
    std::filesystem::remove(prefixPath(kGeneratedXMLFile));
    FileDestination file(prefixPath(kGeneratedXMLFile));
    REQUIRE_FALSE(!std::filesystem::exists(prefixPath(kGeneratedXMLFile)));
    std::filesystem::path filePath(prefixPath(kGeneratedXMLFile));
    REQUIRE(std::filesystem::file_size(filePath) == 0);
  }
  SECTION("Create FileDestination and add one character.", "[XML][Stringify][FileDestination]")
  {
    std::filesystem::remove(prefixPath(kGeneratedXMLFile));
    FileDestination file(prefixPath(kGeneratedXMLFile));
    file.add("t");
    std::filesystem::path filePath(prefixPath(kGeneratedXMLFile));
    REQUIRE(std::filesystem::file_size(filePath) == 1);
  }
  SECTION("Create FileDestination, add an stringified integer and check result.", "[XML][Stringify][FileDestination]")
  {
    std::filesystem::remove(prefixPath(kGeneratedXMLFile));
    FileDestination file(prefixPath(kGeneratedXMLFile));
    file.add("65767");
    std::filesystem::path filePath(prefixPath(kGeneratedXMLFile));
    REQUIRE(std::filesystem::file_size(filePath) == 5);
    std::string expected = readXMLFromFileUTF8(prefixPath(kGeneratedXMLFile));
    REQUIRE(expected == "65767");
  }
}