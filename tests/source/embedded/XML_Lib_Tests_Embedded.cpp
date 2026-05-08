#include "XML_Lib_Tests.hpp"
#include <filesystem>

#ifdef XML_LIB_EMBEDDED

TEST_CASE("Embedded minimal XML parse/stringify round-trip", "[XML][Embedded][Minimal]")
{
  const std::string xmlString = "<root><child attr=\"1\">value</child></root>";
  XML xml;
  REQUIRE_NOTHROW(xml.parse(BufferSource{ xmlString }));

  BufferDestination destination;
  REQUIRE_NOTHROW(xml.stringify(destination));
  REQUIRE(destination.size() > 0);
  REQUIRE(destination.toString().find("<root>") != std::string::npos);
}

TEST_CASE("Embedded source wrapper parse from buffer and file", "[XML][Embedded][Source]")
{
  const std::string xmlString = "<root><leaf>embedded</leaf></root>";
  BufferSource bufferSource{ xmlString };
  XML xml;
  REQUIRE_NOTHROW(xml.parse(bufferSource));
  REQUIRE(xml.root().getChildren().size() == 1);

  const std::string fileName = generateRandomFileName();
  XML::toFile(fileName, xmlString, XML::Format::utf8);
  FileSource fileSource{ fileName };
  XML xmlFromFile;
  REQUIRE_NOTHROW(xmlFromFile.parse(fileSource));
  REQUIRE(xmlFromFile.root().getChildren().size() == 1);
}

TEST_CASE("Embedded library artifact exists and is non-empty", "[XML][Embedded][Size]")
{
  const auto cwd = std::filesystem::current_path();
  std::filesystem::path libPath = cwd / "libXML_Lib.a";
  if (!std::filesystem::exists(libPath)) {
    libPath = cwd / ".." / "libXML_Lib.a";
  }
  if (!std::filesystem::exists(libPath)) {
    libPath = cwd / "tests" / ".." / "libXML_Lib.a";
  }
  REQUIRE(std::filesystem::exists(libPath));
  REQUIRE(std::filesystem::file_size(libPath) > 0);
}

#endif
