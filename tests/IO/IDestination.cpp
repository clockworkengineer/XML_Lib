//
// Unit Tests: XML
//
// Description: Unit tests for BufferDestination and FileDestination classes 
// that implement the IDestination interfaces used in XML parsing.
//
// ================
// Test definitions
// =================
#include "XML_Tests.hpp"
// =======================
// XML class namespace
// =======================
using namespace XML_Lib;
// ==========
// Test cases
// ==========
TEST_CASE("Creation and use of IDestination (Buffer) interface.", "[XML][Parse][BufferDestination]")
{
    SECTION("Create BufferDestination.", "[XML][Stringify][IDestination]")
    {
        REQUIRE_NOTHROW(BufferDestination());
    }
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
    SECTION("Create BufferDestination and add an stringified integer and check result.", "[XML][Stringify][BufferDestination]")
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
        std::filesystem::remove(prefixTestDataPath(kGeneratedXMLFile));
        REQUIRE_NOTHROW(FileDestination(prefixTestDataPath(kGeneratedXMLFile)));
    }
    SECTION("Create FileDestination when file already exists.", "[XML][Stringify][FileDestination]")
    {
        FileDestination file(prefixTestDataPath(kGeneratedXMLFile));
        REQUIRE_NOTHROW(FileDestination(prefixTestDataPath(kGeneratedXMLFile)));
    }
    SECTION("Create FileDestination and test file exists and should be empty.", "[XML][Stringify][FileDestination]")
    {
        std::filesystem::remove(prefixTestDataPath(kGeneratedXMLFile));
        FileDestination file(prefixTestDataPath(kGeneratedXMLFile));
        REQUIRE_FALSE(!std::filesystem::exists(prefixTestDataPath(kGeneratedXMLFile)));
        std::filesystem::path filePath(prefixTestDataPath(kGeneratedXMLFile));
        REQUIRE(std::filesystem::file_size(filePath) == 0);
    }
    SECTION("Create FileDestination and add one character.", "[XML][Stringify][FileDestination]")
    {
        std::filesystem::remove(prefixTestDataPath(kGeneratedXMLFile));
        FileDestination file(prefixTestDataPath(kGeneratedXMLFile));
        file.add("t");
        std::filesystem::path filePath(prefixTestDataPath(kGeneratedXMLFile));
        REQUIRE(std::filesystem::file_size(filePath) == 1);
    }
    SECTION("Create FileDestination, add an stringified integer and check result.", "[XML][Stringify][FileDestination]")
    {
        std::filesystem::remove(prefixTestDataPath(kGeneratedXMLFile));
        FileDestination file(prefixTestDataPath(kGeneratedXMLFile));
        file.add("65767");
        std::filesystem::path filePath(prefixTestDataPath(kGeneratedXMLFile));
        REQUIRE(std::filesystem::file_size(filePath) == 5);
        std::string expected = readXMLFromFileUTF8(prefixTestDataPath(kGeneratedXMLFile));
        REQUIRE(expected == "65767");
    }
}