// Program: XML_XPath_Basic
//
// Description: Demonstrates basic XPath 1.0 navigation using XML_Lib.
// Covers absolute paths (/root/child), descendant shorthand (//),
// the self (.) and parent (..) abbreviations, wildcard (*), and
// the attribute axis (@attr).
//
// Dependencies: C++20, XML_Lib.
//

#include "XML_Utility.hpp"
#include "XPath.hpp"

namespace xl = XML_Lib;

// ---- Document under test ---------------------------------------------------

static const std::string kBookstore = R"(<?xml version="1.0" encoding="UTF-8"?>
<bookstore>
  <book category="cooking">
    <title lang="en">Everyday Italian</title>
    <author>Giada De Laurentiis</author>
    <year>2005</year>
    <price>30.00</price>
  </book>
  <book category="children">
    <title lang="en">Harry Potter</title>
    <author>J K. Rowling</author>
    <year>2005</year>
    <price>29.99</price>
  </book>
  <book category="web">
    <title lang="en">Learning XML</title>
    <author>Erik T. Ray</author>
    <year>2003</year>
    <price>39.95</price>
  </book>
</bookstore>)";

// ---- Helper ----------------------------------------------------------------

static xl::XML gXml;

/// <summary>
/// Implementation of query.
/// </summary>
static void query(const std::string &label, const std::string &expr)
{
  std::cout << "  " << label << std::endl;
  try {
    const auto nodes = gXml.xpath(expr);
    std::cout << "    expression : " << expr << std::endl;
    std::cout << "    node count : " << nodes.size() << std::endl;
    for (const auto *n : nodes) {
      if (xl::isA<xl::Element>(*n) || xl::isA<xl::Root>(*n)) {
        std::cout << "    element    : <" << xl::NRef<xl::Element>(*n).name() << ">" << std::endl;
      } else if (xl::isA<xl::Content>(*n)) {
        std::cout << "    text       : " << n->getContents() << std::endl;
      }
    }
  } catch (const xl::XPath::Error &e) {
    std::cerr << "    XPath error: " << e.what() << std::endl;
  }
}

// ---- Main ------------------------------------------------------------------

/// <summary>Program entry point.</summary>
int main()
{
  std::cout << "XML_XPath_Basic started ..." << std::endl;
  xl::BufferSource source{ kBookstore };
  gXml.parse(source);

  std::cout << "--- Absolute paths ---" << std::endl;
  query("Root element via /bookstore", "/bookstore");
  query("All book elements via /bookstore/book", "/bookstore/book");
  query("All titles via /bookstore/book/title", "/bookstore/book/title");

  std::cout << "--- Descendant shorthand (//) ---" << std::endl;
  query("All book elements anywhere (//book)", "//book");
  query("All title elements anywhere (//title)", "//title");
  query("All elements anywhere (//*)", "//*");

  std::cout << "--- Self and parent (. and ..) ---" << std::endl;
  query("Self — context is root element (.)", ".");
  query("Self by name (self::bookstore)", "self::bookstore");

  std::cout << "--- Wildcard (*) ---" << std::endl;
  query("All children of root (/bookstore/*)", "/bookstore/*");
  query("All grandchildren (/bookstore/book/*)", "/bookstore/book/*");

  std::cout << "--- Attribute axis (@) ---" << std::endl;
  query("category attribute of all books (//book/@category)", "//book/@category");
  query("lang attribute of all titles (//title/@lang)", "//title/@lang");

  std::cout << "--- Evaluatestring / number shortcuts ---" << std::endl;
  xl::XPath xp(gXml.root());
  std::cout << "  string(//title[1]) = " << xp.evaluateString("string(//title[1])") << std::endl;
  std::cout << "  count(//book)      = " << xp.evaluateNumber("count(//book)") << std::endl;
  std::cout << "  boolean(//book)    = " << (xp.evaluateBool("boolean(//book)") ? "true" : "false") << std::endl;
  std::cout << "XML_XPath_Basic exited." << std::endl;
  return 0;
}