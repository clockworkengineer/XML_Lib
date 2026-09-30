#include "XML_Lib_Tests.hpp"
#include "XMLReader.hpp"
#include "XMLWriter.hpp"
#include <string>
#include <vector>

using namespace XML_Lib;

TEST_CASE("XMLReader basic document traversal", "[XMLReader]")
{
  const std::string xml =
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
    "<catalog>\n"
    "  <book id=\"bk101\">\n"
    "    <title>XML Developer's Guide</title>\n"
    "    <price>44.95</price>\n"
    "  </book>\n"
    "  <emptyTag attr=\"val\"/>\n"
    "</catalog>";

  XMLReader reader(xml);
  reader.setSkipWhitespace(true);

  // 1. Declaration
  REQUIRE(reader.read());
  CHECK(reader.nodeType() == XMLReader::NodeType::Declaration);
  CHECK(reader.name() == "xml");
  CHECK(reader.getAttribute("version") == "1.0");
  CHECK(reader.getAttribute("encoding") == "UTF-8");

  // 2. <catalog>
  REQUIRE(reader.read());
  CHECK(reader.nodeType() == XMLReader::NodeType::ElementStart);
  CHECK(reader.name() == "catalog");
  CHECK(reader.depth() == 1);
  CHECK_FALSE(reader.isEmptyElement());

  // 3. <book id="bk101">
  REQUIRE(reader.read());
  CHECK(reader.nodeType() == XMLReader::NodeType::ElementStart);
  CHECK(reader.name() == "book");
  CHECK(reader.depth() == 2);
  CHECK(reader.attributeCount() == 1);
  CHECK(reader.getAttribute("id") == "bk101");
  CHECK(reader.getAttribute(0) == "bk101");
  CHECK(reader.getAttributeName(0) == "id");
  CHECK(reader.hasAttribute("id"));
  CHECK_FALSE(reader.hasAttribute("missing"));

  // 4. <title>
  REQUIRE(reader.read());
  CHECK(reader.nodeType() == XMLReader::NodeType::ElementStart);
  CHECK(reader.name() == "title");
  CHECK(reader.depth() == 3);

  // 5. Title text
  REQUIRE(reader.read());
  CHECK(reader.nodeType() == XMLReader::NodeType::Text);
  CHECK(reader.value() == "XML Developer's Guide");

  // 6. </title>
  REQUIRE(reader.read());
  CHECK(reader.nodeType() == XMLReader::NodeType::ElementEnd);
  CHECK(reader.name() == "title");
  CHECK(reader.depth() == 2);

  // 7. <price>
  REQUIRE(reader.read());
  CHECK(reader.nodeType() == XMLReader::NodeType::ElementStart);
  CHECK(reader.name() == "price");

  // 8. Price text
  REQUIRE(reader.read());
  CHECK(reader.nodeType() == XMLReader::NodeType::Text);
  CHECK(reader.value() == "44.95");

  // 9. </price>
  REQUIRE(reader.read());
  CHECK(reader.nodeType() == XMLReader::NodeType::ElementEnd);
  CHECK(reader.name() == "price");

  // 10. </book>
  REQUIRE(reader.read());
  CHECK(reader.nodeType() == XMLReader::NodeType::ElementEnd);
  CHECK(reader.name() == "book");

  // 11. <emptyTag attr="val"/> (ElementStart)
  REQUIRE(reader.read());
  CHECK(reader.nodeType() == XMLReader::NodeType::ElementStart);
  CHECK(reader.name() == "emptyTag");
  CHECK(reader.isEmptyElement());
  CHECK(reader.getAttribute("attr") == "val");

  // 12. Synthesized ElementEnd for emptyTag
  REQUIRE(reader.read());
  CHECK(reader.nodeType() == XMLReader::NodeType::ElementEnd);
  CHECK(reader.name() == "emptyTag");
  CHECK(reader.isEmptyElement());

  // 13. </catalog>
  REQUIRE(reader.read());
  CHECK(reader.nodeType() == XMLReader::NodeType::ElementEnd);
  CHECK(reader.name() == "catalog");

  // End of Document
  REQUIRE_FALSE(reader.read());
  CHECK(reader.nodeType() == XMLReader::NodeType::EndDocument);
}

TEST_CASE("XMLReader CDATA, comments, PIs, and entity references", "[XMLReader]")
{
  const std::string xml =
    "<root>"
    "<!-- Top level comment -->"
    "<?target instruction data?>"
    "<![CDATA[Unescaped <tag> & data]]>"
    "<escaped>&lt;hello &amp; world&gt; &#65; &#x42;</escaped>"
    "</root>";

  XMLReader reader(xml);

  // <root>
  REQUIRE(reader.read());
  CHECK(reader.nodeType() == XMLReader::NodeType::ElementStart);
  CHECK(reader.name() == "root");

  // Comment
  REQUIRE(reader.read());
  CHECK(reader.nodeType() == XMLReader::NodeType::Comment);
  CHECK(reader.value() == " Top level comment ");

  // PI
  REQUIRE(reader.read());
  CHECK(reader.nodeType() == XMLReader::NodeType::ProcessingInstruction);
  CHECK(reader.name() == "target");
  CHECK(reader.value() == "instruction data");

  // CDATA
  REQUIRE(reader.read());
  CHECK(reader.nodeType() == XMLReader::NodeType::CDATA);
  CHECK(reader.value() == "Unescaped <tag> & data");

  // <escaped>
  REQUIRE(reader.read());
  CHECK(reader.nodeType() == XMLReader::NodeType::ElementStart);
  CHECK(reader.name() == "escaped");

  // Escaped text
  REQUIRE(reader.read());
  CHECK(reader.nodeType() == XMLReader::NodeType::Text);
  CHECK(reader.value() == "<hello & world> A B");

  // </escaped>
  REQUIRE(reader.read());
  CHECK(reader.nodeType() == XMLReader::NodeType::ElementEnd);
  CHECK(reader.name() == "escaped");

  // </root>
  REQUIRE(reader.read());
  CHECK(reader.nodeType() == XMLReader::NodeType::ElementEnd);
  CHECK(reader.name() == "root");
}

TEST_CASE("XMLReader helper methods: readToNextElement, readElementText, skip", "[XMLReader]")
{
  const std::string xml =
    "<items>\n"
    "  <item id=\"1\"><desc>Item <b>one</b> description</desc><cost>10</cost></item>\n"
    "  <item id=\"2\"><desc>Item two</desc><cost>20</cost></item>\n"
    "  <item id=\"3\"><desc>Item three</desc><cost>30</cost></item>\n"
    "</items>";

  SECTION("readToNextElement iterates elements only") {
    XMLReader reader(xml);
    std::vector<std::string> elements;
    while (reader.readToNextElement()) {
      elements.emplace_back(reader.name());
    }
    CHECK(elements == std::vector<std::string>{ "items", "item", "desc", "b", "cost", "item", "desc", "cost", "item", "desc", "cost" });
  }

  SECTION("readElementText reads inner element text") {
    XMLReader reader(xml);
    REQUIRE(reader.readToNextElement()); // <items>
    REQUIRE(reader.readToNextElement()); // <item id="1">
    REQUIRE(reader.readToNextElement()); // <desc>
    const std::string text = reader.readElementText();
    CHECK(text == "Item one description");
  }

  SECTION("skip jumps over child elements") {
    XMLReader reader(xml);
    REQUIRE(reader.readToNextElement()); // <items>
    REQUIRE(reader.readToNextElement()); // <item id="1">
    CHECK(reader.getAttribute("id") == "1");
    reader.skip(); // skip subtree of item 1
    CHECK(reader.nodeType() == XMLReader::NodeType::ElementEnd);
    CHECK(reader.name() == "item");

    REQUIRE(reader.readToNextElement()); // <item id="2">
    CHECK(reader.getAttribute("id") == "2");
  }
}

TEST_CASE("XMLWriter generating XML documents", "[XMLWriter]")
{
  SECTION("Compact XML generation") {
    XMLWriter writer;
    writer.setOmitXmlDeclaration(true);
    writer.writeStartElement("catalog");
    writer.writeAttribute("version", "1.0");
    writer.writeStartElement("book");
    writer.writeAttribute("id", "bk01");
    writer.writeElement("title", "Modern C++ Programming");
    writer.writeElement("price", "49.99");
    writer.writeEndElement(); // </book>
    writer.writeEmptyElement("marker");
    writer.writeEndElement(); // </catalog>

    const std::string expected =
      "<catalog version=\"1.0\">"
      "<book id=\"bk01\">"
      "<title>Modern C++ Programming</title>"
      "<price>49.99</price>"
      "</book>"
      "<marker/>"
      "</catalog>";

    CHECK(writer.result() == expected);
  }

  SECTION("Indented XML generation") {
    XMLWriter writer;
    writer.setIndent(true, 2);
    writer.writeStartDocument("1.0", "UTF-8");
    writer.writeStartElement("root");
    writer.writeElement("child", "Hello World");
    writer.writeEndElement();
    writer.writeEndDocument();

    const std::string result = writer.result();
    CHECK(result.find("<?xml version=\"1.0\" encoding=\"UTF-8\"?>") != std::string::npos);
    CHECK(result.find("  <child>Hello World</child>") != std::string::npos);
    CHECK(result.find("</root>") != std::string::npos);
  }

  SECTION("Special characters escaping") {
    XMLWriter writer;
    writer.setOmitXmlDeclaration(true);
    writer.writeStartElement("data");
    writer.writeAttribute("quote", "He said \"hello\" & 'goodbye'");
    writer.writeCharacters("Math: 5 < 10 && 10 > 5");
    writer.writeEndElement();

    const std::string expected =
      "<data quote=\"He said &quot;hello&quot; &amp; &apos;goodbye&apos;\">"
      "Math: 5 &lt; 10 &amp;&amp; 10 &gt; 5"
      "</data>";

    CHECK(writer.result() == expected);
  }

  SECTION("Comments, CDATA, and Processing Instructions") {
    XMLWriter writer;
    writer.setOmitXmlDeclaration(true);
    writer.writeStartElement("doc");
    writer.writeComment("Config section");
    writer.writeProcessingInstruction("app", "mode=fast");
    writer.writeCDATA("<xml>inside cdata & raw</xml>");
    writer.writeEndElement();

    const std::string expected =
      "<doc>"
      "<!--Config section-->"
      "<?app mode=fast?>"
      "<![CDATA[<xml>inside cdata & raw</xml>]]>"
      "</doc>";

    CHECK(writer.result() == expected);
  }
}

TEST_CASE("XMLWriter and XMLReader round-trip streaming", "[XMLStreamRoundTrip]")
{
  // Generate XML with XMLWriter
  XMLWriter writer;
  writer.setOmitXmlDeclaration(true);
  writer.writeStartElement("inventory");

  constexpr int kNumItems = 1000;
  for (int i = 0; i < kNumItems; ++i) {
    writer.writeStartElement("item");
    writer.writeAttribute("index", std::to_string(i));
    writer.writeElement("name", "Product " + std::to_string(i));
    writer.writeElement("qty", std::to_string(i * 10));
    writer.writeEndElement();
  }
  writer.writeEndElement(); // </inventory>

  const std::string xmlOutput = writer.result();
  REQUIRE_FALSE(xmlOutput.empty());

  // Parse back with XMLReader
  XMLReader reader(xmlOutput);
  int itemCount = 0;
  int textNodeCount = 0;

  while (reader.read()) {
    if (reader.nodeType() == XMLReader::NodeType::ElementStart && reader.name() == "item") {
      CHECK(reader.getAttribute("index") == std::to_string(itemCount));
      ++itemCount;
    } else if (reader.nodeType() == XMLReader::NodeType::Text) {
      ++textNodeCount;
    }
  }

  CHECK(itemCount == kNumItems);
  CHECK(textNodeCount == kNumItems * 2); // 2 text nodes per item (name, qty)
}
