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
  SECTION("BufferDestination clear empties buffer.", "[XML][Stringify][BufferDestination][Clear]")
  {
    BufferDestination buffer;
    buffer.add("abc");
    buffer.clear();
    REQUIRE(buffer.size() == 0);
    REQUIRE(buffer.toString().empty());
  }
  SECTION("BufferDestination handles large writes.", "[XML][Stringify][BufferDestination][Large]")
  {
    BufferDestination buffer;
    std::string large(10000, 'x');
    buffer.add(large);
    REQUIRE(buffer.size() == 10000);
    REQUIRE(buffer.toString() == large);
  }
  SECTION("FileDestination clear truncates file.", "[XML][Stringify][FileDestination][Clear]")
  {
    std::string generatedFileName{ generateRandomFileName() };
    FileDestination file(generatedFileName);
    file.add("abc");
    file.clear();
    file.add("z");
    file.close();
    REQUIRE(std::filesystem::file_size(generatedFileName) == 1);
    std::string content = XML::fromFile(generatedFileName);
    REQUIRE(content == "z");
    std::filesystem::remove(generatedFileName);
  }
  SECTION("FileDestination getFileName returns correct name.", "[XML][Stringify][FileDestination][API]")
  {
    std::string generatedFileName{ generateRandomFileName() };
    FileDestination file(generatedFileName);
    REQUIRE(file.getFileName() == generatedFileName);
    file.close();
    std::filesystem::remove(generatedFileName);
  }
  SECTION("FileDestination throws on unwritable location (again)", "[XML][Stringify][FileDestination][Error]")
  {
    std::string badFile = "/this/path/should/not/exist/test2.xml";
#ifdef _WIN32
    badFile = "Z:/this/path/should/not/exist/test2.xml";
#endif
    REQUIRE_THROWS_AS(FileDestination(badFile), FileDestination::Error);
  }
}