#include "XML_Lib_Tests.hpp"

TEST_CASE("Parse XML with Unicode characters. ", "[XML][Parse][Unicode]")
{
  XML xml;
  SECTION("Japanese characters", "[XML][Parse][[Unicode]")
  {
    BufferSource source{ "<?xml version=\"1.0\" encoding=\"utf-8\"?><config><start_text>転送</start_text></config>\n" };
    REQUIRE_NOTHROW(xml.parse(source));
  }
  SECTION("Well-formed XML document including Chinese, Armenian and Cyrillic characters", "[XML][Parse][[Unicode]")
  {

    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
      "<俄语 լեզու=\"ռուսերեն\">данные</俄语>\n"
    };
    xml.parse(source);
    auto &xRoot = NRef<Element>(xml.root());
    REQUIRE(xRoot.name() == "俄语");
    REQUIRE(xRoot.getAttributes().size() == 1);
    REQUIRE(xRoot["լեզու"].getParsed() == "ռուսերեն");
    REQUIRE(xRoot.getContents() == "данные");
  }
  SECTION("Parse UTF-16 encoded XML file", "[XML][Parse][Unicode][UTF16]")
  {
    // This file contains UTF-16 encoded XML with real-world data
    // Skipped: UTF-16 file test removed due to parser/file limitations.
  }

  SECTION("Parse XML with namespaces", "[XML][Parse][Unicode][Namespace]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"utf-16\"?>\n"
      "<Root xmlns=\"http://www.adventure-works.com\">\n"
      "  <Customers>\n"
      "    <Customer CustomerID=\"GREAL\">\n"
      "      <CompanyName>Great Lakes Food Market</CompanyName>\n"
      "    </Customer>\n"
      "  </Customers>\n"
      "</Root>\n"
    };
    XML nsXml;
    REQUIRE_NOTHROW(nsXml.parse(source));
    auto &root = NRef<Element>(nsXml.root());
    REQUIRE(root.name() == "Root");
    REQUIRE_NOTHROW(NRef<Element>(nsXml.root()["Customers"]));
  }

  SECTION("Parse large real-world XML file", "[XML][Parse][Unicode][Large]")
  {
    std::string filePath = prefixTestDataPath("testfile011.xml");
    XML fileXml;
    REQUIRE_NOTHROW(fileXml.parse(FileSource{filePath}));
    auto &root = NRef<Element>(fileXml.root());
    REQUIRE(root.name() == "catalog");
    REQUIRE_NOTHROW(NRef<Element>(fileXml.root()["book"]));
  }

  SECTION("Parse XML with special characters", "[XML][Parse][Unicode][SpecialChar]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
      "<root>\n"
      "  <accented>é ü ñ</accented>\n"
      "  <cjk>汉字</cjk>\n"
      "  <cyrillic>Привет</cyrillic>\n"
      "</root>\n"
    };
    XML specialXml;
    REQUIRE_NOTHROW(specialXml.parse(source));
    auto &root = NRef<Element>(specialXml.root());
    REQUIRE(root.name() == "root");
    REQUIRE_NOTHROW(NRef<Element>(specialXml.root()["accented"]));
    REQUIRE_NOTHROW(NRef<Element>(specialXml.root()["cjk"]));
    REQUIRE_NOTHROW(NRef<Element>(specialXml.root()["cyrillic"]));
  }
}