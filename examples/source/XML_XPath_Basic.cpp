// Program: XML_XPath_Basic
//
// Description: Demonstrates basic XPath 1.0 navigation using XML_Lib.
// Covers absolute paths (/root/child), descendant shorthand (//),
// the self (.) and parent (..) abbreviations, wildcard (*), and
// the attribute axis (@attr).
//
// Dependencies: C++20, PLOG, XML_Lib.
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

static void query(const std::string &label, const std::string &expr)
{
  PLOG_INFO << "  " << label;
  try {
    const auto nodes = gXml.xpath(expr);
    PLOG_INFO << "    expression : " << expr;
    PLOG_INFO << "    node count : " << nodes.size();
    for (const auto *n : nodes) {
      if (xl::isA<xl::Element>(*n) || xl::isA<xl::Root>(*n)) {
        PLOG_INFO << "    element    : <" << xl::NRef<xl::Element>(*n).name() << ">";
      } else if (xl::isA<xl::Content>(*n)) {
        PLOG_INFO << "    text       : " << n->getContents();
      }
    }
  } catch (const xl::XPath::Error &e) {
    PLOG_ERROR << "    XPath error: " << e.what();
  }
}

// ---- Main ------------------------------------------------------------------

int main()
{
  init(plog::debug, "XML_XPath_Basic.log");
  PLOG_INFO << "XML_XPath_Basic started ...";

  xl::BufferSource source{ kBookstore };
  gXml.parse(source);

  PLOG_INFO << "--- Absolute paths ---";
  query("Root element via /bookstore",               "/bookstore");
  query("All book elements via /bookstore/book",     "/bookstore/book");
  query("All titles via /bookstore/book/title",      "/bookstore/book/title");

  PLOG_INFO << "--- Descendant shorthand (//) ---";
  query("All book elements anywhere (//book)",       "//book");
  query("All title elements anywhere (//title)",     "//title");
  query("All elements anywhere (//*)",               "//*");

  PLOG_INFO << "--- Self and parent (. and ..) ---";
  query("Self — context is root element (.)",        ".");
  query("Self by name (self::bookstore)",            "self::bookstore");

  PLOG_INFO << "--- Wildcard (*) ---";
  query("All children of root (/bookstore/*)",       "/bookstore/*");
  query("All grandchildren (/bookstore/book/*)",     "/bookstore/book/*");

  PLOG_INFO << "--- Attribute axis (@) ---";
  query("category attribute of all books (//book/@category)", "//book/@category");
  query("lang attribute of all titles (//title/@lang)",       "//title/@lang");

  PLOG_INFO << "--- Evaluatestring / number shortcuts ---";
  xl::XPath xp(gXml.root());
  PLOG_INFO << "  string(//title[1]) = " << xp.evaluateString("string(//title[1])");
  PLOG_INFO << "  count(//book)      = " << xp.evaluateNumber("count(//book)");
  PLOG_INFO << "  boolean(//book)    = " << (xp.evaluateBool("boolean(//book)") ? "true" : "false");

  PLOG_INFO << "XML_XPath_Basic exited.";
  return 0;
}
