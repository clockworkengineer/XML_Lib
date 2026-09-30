#include "XML_Lib_Tests.hpp"
#include "XML.hpp"
#include "XMLReader.hpp"
#include "XML_Ranges.hpp"
#include "XML_NodeRef.hpp"
#include "XML_Sources.hpp"
#include "XSD_Schema.hpp"

#include <expected>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

using namespace XML_Lib;

TEST_CASE("C++23: std::expected non-throwing parsing (XML::parseExpected)", "[cpp23][expected]")
{
  SECTION("Valid XML string returns expected document")
  {
    const std::string validXml = "<root><item id=\"42\">Hello C++23</item></root>";
    auto result = XML::parseExpected(validXml);
    REQUIRE(result.has_value());
    REQUIRE(*result != nullptr);
    CHECK(NRef<Element>((*result)->root()).name() == "root");
    CHECK((*result)->root()["item"].getContents() == "Hello C++23");
  }

  SECTION("Malformed XML string returns unexpected with error details")
  {
    const std::string malformedXml = "<root><item>Unclosed root";
    auto result = XML::parseExpected(malformedXml);
    REQUIRE(!result.has_value());
    CHECK(!result.error().message.empty());
    CHECK(result.error().line > 0);
  }

  SECTION("Valid ISource stream parsing")
  {
    const std::string validXml = "<config enabled=\"true\"/>";
    BufferSource source{ validXml };
    auto result = XML::parseExpected(source);
    REQUIRE(result.has_value());
    CHECK(NRef<Element>((*result)->root()).name() == "config");
  }

  SECTION("Non-existent file returns error without throwing")
  {
    auto result = XML::parseExpected(std::filesystem::path("non_existent_file_12345.xml"));
    REQUIRE(!result.has_value());
    CHECK(!result.error().message.empty());
  }
}

TEST_CASE("C++23: std::expected validation (validateExpected)", "[cpp23][expected][validation]")
{
  const std::string schemaXml =
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
    "<xs:schema xmlns:xs=\"http://www.w3.org/2001/XMLSchema\">\n"
    "  <xs:element name=\"note\">\n"
    "    <xs:complexType>\n"
    "      <xs:sequence>\n"
    "        <xs:element name=\"to\" type=\"xs:string\"/>\n"
    "        <xs:element name=\"from\" type=\"xs:string\"/>\n"
    "        <xs:element name=\"heading\" type=\"xs:string\"/>\n"
    "        <xs:element name=\"body\" type=\"xs:string\"/>\n"
    "      </xs:sequence>\n"
    "    </xs:complexType>\n"
    "  </xs:element>\n"
    "</xs:schema>";

  const std::string validXml =
    "<note>\n"
    "  <to>Alice</to>\n"
    "  <from>Bob</from>\n"
    "  <heading>Reminder</heading>\n"
    "  <body>Don't forget the meeting</body>\n"
    "</note>";

  const std::string invalidXml =
    "<note>\n"
    "  <to>Alice</to>\n"
    "  <!-- missing from, heading, body -->\n"
    "</note>";

  XSD_Schema compiledSchema(schemaXml);

  SECTION("Valid document against pre-compiled schema")
  {
    XML doc(validXml);
    auto result = doc.validateExpected(compiledSchema);
    CHECK(result.has_value());
  }

  SECTION("Invalid document against pre-compiled schema returns unexpected error")
  {
    XML doc(invalidXml);
    auto result = doc.validateExpected(compiledSchema);
    REQUIRE(!result.has_value());
    CHECK(!result.error().empty());
  }

  SECTION("Valid document against inline schema string")
  {
    XML doc(validXml);
    auto result = doc.validateExpected(schemaXml);
    CHECK(result.has_value());
  }
}

TEST_CASE("C++23: XMLReader readExpected and monadic findAttribute", "[cpp23][reader]")
{
  const std::string xmlString =
    "<server host=\"localhost\" port=\"8080\">\n"
    "  <service name=\"http\" enabled=\"true\"/>\n"
    "</server>";

  XMLReader reader(xmlString);

  SECTION("readExpected traverses tokens without throwing")
  {
    int elementCount = 0;
    while (true) {
      auto res = reader.readExpected();
      REQUIRE(res.has_value());
      if (!*res) { break; }
      if (reader.nodeType() == XMLReader::NodeType::ElementStart) {
        elementCount++;
      }
    }
    CHECK(elementCount == 2);
  }

  SECTION("Monadic findAttribute on XMLReader")
  {
    REQUIRE(reader.readToNextElement());
    CHECK(reader.name() == "server");

    // Existing attribute
    auto host = reader.findAttribute("host");
    REQUIRE(host.has_value());
    CHECK(*host == "localhost");

    // Monadic transform on port
    auto port = reader.findAttribute("port")
      .transform([](std::string_view sv) { return std::stoi(std::string(sv)); })
      .value_or(0);
    CHECK(port == 8080);

    // Non-existent attribute fallback
    auto timeout = reader.findAttribute("timeout")
      .transform([](std::string_view sv) { return std::stoi(std::string(sv)); })
      .value_or(30);
    CHECK(timeout == 30);
  }

  SECTION("nodeTypeToString static conversion")
  {
    CHECK(XMLReader::nodeTypeToString(XMLReader::NodeType::ElementStart) == "ElementStart");
    CHECK(XMLReader::nodeTypeToString(XMLReader::NodeType::ElementEnd) == "ElementEnd");
    CHECK(XMLReader::nodeTypeToString(XMLReader::NodeType::Text) == "Text");
    CHECK(XMLReader::nodeTypeToString(XMLReader::NodeType::CDATA) == "CDATA");
    CHECK(XMLReader::nodeTypeToString(XMLReader::NodeType::Comment) == "Comment");
    CHECK(XMLReader::nodeTypeToString(XMLReader::NodeType::ProcessingInstruction) == "ProcessingInstruction");
    CHECK(XMLReader::nodeTypeToString(XMLReader::NodeType::EndDocument) == "EndDocument");
  }
}

TEST_CASE("C++23: Monadic lookups and multidimensional indexing on DOM", "[cpp23][dom]")
{
  const std::string xmlString =
    "<root>\n"
    "  <database type=\"postgres\" max_connections=\"100\">\n"
    "    <credentials username=\"admin\"/>\n"
    "  </database>\n"
    "  <cache enabled=\"true\"/>\n"
    "</root>";

  XML xml(xmlString);
  const auto &root = xml.root();

  SECTION("Monadic findChild and findAttribute chaining")
  {
    // Existing chain
    auto maxConn = root.findChild("database")
      .and_then([](const Node &db) {
        return NRef<Element>(db).findAttribute("max_connections");
      })
      .transform([](const XMLAttribute &attr) {
        return std::stoi(attr.getParsed());
      })
      .value_or(10);
    CHECK(maxConn == 100);

    // Missing child fallback
    auto replicaPort = root.findChild("replica")
      .and_then([](const Node &rep) {
        return NRef<Element>(rep).findAttribute("port");
      })
      .transform([](const XMLAttribute &attr) {
        return std::stoi(attr.getParsed());
      })
      .value_or(5432);
    CHECK(replicaPort == 5432);
  }

  SECTION("C++23 Multidimensional subscript operator[]")
  {
    // Indexing parent and child directly: root["database", "credentials"]
    const auto &creds = root["database", "credentials"];
    CHECK(NRef<Element>(creds).name() == "credentials");
    CHECK(NRef<Element>(creds)["username"].getParsed() == "admin");
  }
}
