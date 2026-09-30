#include "XML_Lib_Tests.hpp"
#include "XML.hpp"
#include "XML_Concepts.hpp"
#include "XML_Ranges.hpp"
#include "XML_NodeRef.hpp"
#include "XML_Destinations.hpp"
#include "XML_Sources.hpp"
#include "implementation/stringify/Default_Stringify.hpp"

#include <ranges>
#include <vector>
#include <string>

using namespace XML_Lib;

TEST_CASE("C++20 Concepts verification", "[concepts]")
{
  static_assert(XMLNodeLike<Node>, "Node should satisfy XMLNodeLike concept");
  static_assert(XMLSourceLike<BufferSource>, "BufferSource should satisfy XMLSourceLike concept");
  static_assert(XMLSourceLike<FileSource>, "FileSource should satisfy XMLSourceLike concept");
  static_assert(XMLDestinationLike<BufferDestination>, "BufferDestination should satisfy XMLDestinationLike concept");
  static_assert(XMLDestinationLike<FileDestination>, "FileDestination should satisfy XMLDestinationLike concept");

  REQUIRE(true);
}

TEST_CASE("C++20 Ranges: childElements and Node::elements()", "[ranges]")
{
  const std::string xmlString =
    "<catalog>\n"
    "  <!-- Comment should be skipped by elements() -->\n"
    "  <book category=\"fiction\" inStock=\"true\">\n"
    "    <title>Dune</title>\n"
    "    <price>19.99</price>\n"
    "  </book>\n"
    "  <?pi target instruction?>\n"
    "  <book category=\"non-fiction\" inStock=\"false\">\n"
    "    <title>Clean Code</title>\n"
    "    <price>39.99</price>\n"
    "  </book>\n"
    "  <magazine title=\"Wired\"/>\n"
    "</catalog>";

  XML xml(xmlString);
  const auto &root = xml.root();

  SECTION("Iterating all child elements") {
    std::vector<std::string> names;
    for (const auto &elem : root.elements()) {
      names.emplace_back(NRef<Element>(elem).name());
    }
    CHECK(names == std::vector<std::string>{ "book", "book", "magazine" });
  }

  SECTION("Filtering child elements by tag name") {
    std::vector<std::string> bookCategories;
    for (const auto &elem : root.elements("book")) {
      const auto &el = NRef<Element>(elem);
      if (el.hasAttribute("category")) {
        bookCategories.emplace_back(el.getAttribute("category").getParsed());
      }
    }
    CHECK(bookCategories == std::vector<std::string>{ "fiction", "non-fiction" });
  }

  SECTION("Composing with std::views::filter and std::views::transform") {
    auto inStockBookCategories = root.elements("book")
      | std::views::filter([](const Node &node) {
          const auto &el = NRef<Element>(node);
          return el.hasAttribute("inStock") && el.getAttribute("inStock").getParsed() == "true";
        })
      | std::views::transform([](const Node &node) {
          return NRef<Element>(node).getAttribute("category").getParsed();
        });

    std::vector<std::string> results;
    for (const auto &cat : inStockBookCategories) {
      results.emplace_back(cat);
    }
    CHECK(results == std::vector<std::string>{ "fiction" });
  }

  SECTION("Range views over attributes") {
    for (const auto &elem : root.elements("book")) {
      auto attrs = elementAttributes(elem);
      CHECK(attrs.size() == 2);
      CHECK(attrs[0].getName() == "category");
      CHECK(attrs[1].getName() == "inStock");
      break;
    }
  }
}

TEST_CASE("IStringify: StringifyOptions formatting controls", "[stringify][options]")
{
  const std::string xmlString =
    "<root><item id=\"1\" status=\"active\"/><item id=\"2\"><child>text</child></item></root>";

  XML xml(xmlString);

  SECTION("Default compact serialization") {
    BufferDestination dest;
    Default_Stringify stringifier;
    stringifier.stringify(xml.root(), dest, 0);

    const std::string out = dest.toString();
    CHECK(out == "<root><item id=\"1\" status=\"active\"/><item id=\"2\"><child>text</child></item></root>");
  }

  SECTION("Self-closing spacing (<tag /> vs <tag/>)") {
    BufferDestination dest;
    Default_Stringify stringifier;
    StringifyOptions opts;
    opts.selfClosingSpacing = true;
    stringifier.setOptions(opts);

    stringifier.stringify(xml.root(), dest, 0);

    const std::string out = dest.toString();
    CHECK(out.find("<item id=\"1\" status=\"active\" />") != std::string::npos);
  }

  SECTION("Attribute newline wrapping") {
    BufferDestination dest;
    Default_Stringify stringifier;
    StringifyOptions opts;
    opts.prettyPrint = true;
    opts.attributeNewlineWrapping = true;
    opts.indentSpaces = 2;
    stringifier.setOptions(opts);

    stringifier.stringify(xml.root(), dest, 0);

    const std::string out = dest.toString();
    CHECK(out.find("\n    id=\"1\"\n    status=\"active\"") != std::string::npos);
  }
}
