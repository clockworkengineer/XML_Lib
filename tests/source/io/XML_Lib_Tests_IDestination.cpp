#include "XML_Lib_Tests.hpp"

TEST_CASE("Creation and use of IDestination (Buffer) interface.", "[XML][Parse][BufferDestination]")
{
  SECTION("Create BufferDestination.", "[XML][Stringify][IDestination]") { REQUIRE_NOTHROW(BufferDestination()); }
  SECTION("Create BufferDestination and get buffer which should be empty.", "[XML][Stringify][BufferDestination]")
  {
    BufferDestination buffer;
    REQUIRE(buffer.size() == 0);
  }
  SECTION("Create BufferDestination and add one character.", "[XML][Stringify][BufferDestination]")
  {
    BufferDestination buffer;
    buffer.add("i");
    REQUIRE(buffer.size() == 1);
  }
  SECTION(
    "Create BufferDestination and add an stringified integer and check result.", "[XML][Stringify][BufferDestination]")
  {
    BufferDestination buffer;
    buffer.add("65767");
    REQUIRE(buffer.size() == 5);
    REQUIRE(buffer.toString() == ("65767"));
  }
}
TEST_CASE("Creation and use of IDestination (File) interface.", "[XML][Parse][FileDestination]")
{
  SECTION("Create FileDestination.", "[XML][Stringify][FileDestination]")
  {
    std::string generatedFileName{ generateRandomFileName() };
    REQUIRE_NOTHROW(FileDestination(generatedFileName));
    std::filesystem::remove(generatedFileName);
  }
  SECTION("Create FileDestination when file already exists.", "[XML][Stringify][FileDestination]")
  {
    std::string generatedFileName{ generateRandomFileName() };
    REQUIRE_NOTHROW(FileDestination(generatedFileName));
    REQUIRE_NOTHROW(FileDestination(generatedFileName));
    std::filesystem::remove(generatedFileName);
  }
  SECTION("Create FileDestination and test file exists and should be empty.", "[XML][Stringify][FileDestination]")
  {
    std::string generatedFileName{ generateRandomFileName() };
    REQUIRE_NOTHROW(FileDestination(generatedFileName));
    REQUIRE_FALSE(!std::filesystem::exists(generatedFileName));
    REQUIRE(std::filesystem::file_size(generatedFileName) == 0);
    std::filesystem::remove(generatedFileName);
  }
  SECTION("Create FileDestination and add one character.", "[XML][Stringify][FileDestination]")
  {
    std::string generatedFileName{ generateRandomFileName() };
    FileDestination file(generatedFileName);
    file.add("t");
    std::filesystem::path filePath(generatedFileName);
    file.close();
    REQUIRE(std::filesystem::file_size(filePath) == 1);
    std::filesystem::remove(generatedFileName);
  }
  SECTION("Create FileDestination, add an stringified integer and check result.", "[XML][Stringify][FileDestination]")
  {
    std::string generatedFileName{ generateRandomFileName() };
    FileDestination file(generatedFileName);
    file.add("65767");
    file.close();
    REQUIRE(std::filesystem::file_size(generatedFileName) == 5);
    std::string expected = XML::fromFile(generatedFileName);
    REQUIRE(expected == "65767");
    std::filesystem::remove(generatedFileName);
  }
}