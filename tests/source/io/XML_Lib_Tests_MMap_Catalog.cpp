#include "XML_Lib_Tests.hpp"
#include "XML.hpp"
#include "XMLReader.hpp"
#include "XMLWriter.hpp"
#include "XML_Ranges.hpp"
#include "OASIS_Catalog.hpp"
#include "io/XML_MMapSource.hpp"

#include <filesystem>
#include <fstream>
#include <string>

using namespace XML_Lib;

TEST_CASE("MMapSource memory-mapped file reading", "[io][mmap]")
{
  const std::string tempFile = generateRandomFileName();
  const std::string xmlContent =
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
    "<database>\n"
    "  <record id=\"1\">Record One</record>\n"
    "  <record id=\"2\">Record Two</record>\n"
    "</database>\n";

  // Write content to temporary file
  {
    std::ofstream out(tempFile, std::ios::binary);
    out << xmlContent;
  }

  SECTION("Verify basic MMapSource properties") {
    MMapSource mmap(tempFile);
    CHECK(mmap.size() == xmlContent.size());
    CHECK(mmap.stringView() == xmlContent);
    CHECK(mmap.data() != nullptr);
    CHECK(mmap.getSystemId() == tempFile);
  }

  SECTION("Parse via XMLReader using zero-copy stringView()") {
    MMapSource mmap(tempFile);
    XMLReader reader(mmap.stringView());
    reader.setSkipWhitespace(true);

    REQUIRE(reader.read()); // declaration
    CHECK(reader.nodeType() == XMLReader::NodeType::Declaration);

    REQUIRE(reader.read()); // <database>
    CHECK(reader.name() == "database");

    REQUIRE(reader.read()); // <record id="1">
    CHECK(reader.name() == "record");
    CHECK(reader.getAttribute("id") == "1");

    REQUIRE(reader.read()); // Text
    CHECK(reader.value() == "Record One");

    REQUIRE(reader.read()); // </record>
    REQUIRE(reader.read()); // <record id="2">
    CHECK(reader.getAttribute("id") == "2");
  }

  SECTION("Parse via DOM XML::parse with MMapSource") {
    MMapSource mmap(tempFile);
    XML xml;
    xml.parse(mmap);
    CHECK(xml.root().getChildren().size() == 5);
    int elementCount = 0;
    for (const auto &elem : xml.root().elements()) {
      (void)elem;
      ++elementCount;
    }
    CHECK(elementCount == 2);
    CHECK(xml.root()["record"].getContents() == "Record One");
  }

  std::error_code ec;
  std::filesystem::remove(tempFile, ec);
}

TEST_CASE("OASIS_Catalog 1.1 resolution", "[catalog][oasis]")
{
  const std::string catalogXml =
    "<catalog xmlns=\"urn:oasis:names:tc:entity:xmlns:xml:catalog\">\n"
    "  <system systemId=\"http://example.com/dtd/book.dtd\" uri=\"local-book.dtd\"/>\n"
    "  <public publicId=\"-//EXAMPLE//DTD Book 1.0//EN\" uri=\"local-book.dtd\"/>\n"
    "  <rewriteSystem systemIdStartString=\"http://www.w3.org/2001/\" rewritePrefix=\"/schemas/w3c/\"/>\n"
    "  <rewriteURI uriStartString=\"http://example.org/schema/\" rewritePrefix=\"/local/schemas/\"/>\n"
    "</catalog>";

  OASIS_Catalog catalog(catalogXml);
  catalog.addMemoryContent("local-book.dtd", "<!ELEMENT book (title, author)>\n<!ELEMENT title (#PCDATA)>\n<!ELEMENT author (#PCDATA)>");

  SECTION("Exact SYSTEM identifier resolution") {
    auto resolved = catalog.resolveSystem("http://example.com/dtd/book.dtd");
    REQUIRE(resolved.has_value());
    CHECK(resolved.value() == "local-book.dtd");
  }

  SECTION("Exact PUBLIC identifier resolution") {
    auto resolved = catalog.resolvePublic("-//EXAMPLE//DTD Book 1.0//EN");
    REQUIRE(resolved.has_value());
    CHECK(resolved.value() == "local-book.dtd");
  }

  SECTION("rewriteSystem prefix matching") {
    auto resolved = catalog.resolveSystem("http://www.w3.org/2001/XMLSchema.dtd");
    REQUIRE(resolved.has_value());
    CHECK(resolved.value() == "/schemas/w3c/XMLSchema.dtd");
  }

  SECTION("rewriteURI prefix matching") {
    auto resolved = catalog.resolveURI("http://example.org/schema/customer.xsd");
    REQUIRE(resolved.has_value());
    CHECK(resolved.value() == "/local/schemas/customer.xsd");
  }

  SECTION("Integration with IEntityResolver and content retrieval") {
    auto content = catalog.resolve("http://example.com/dtd/book.dtd", "");
    REQUIRE(content.has_value());
    CHECK(content.value().find("<!ELEMENT book (title, author)>") != std::string::npos);
  }

  SECTION("Unknown identifier returns nullopt") {
    auto content = catalog.resolve("http://unknown.org/unknown.dtd", "");
    CHECK_FALSE(content.has_value());
  }
}
