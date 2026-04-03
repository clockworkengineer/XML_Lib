// Program: XML_XPath_Predicates
//
// Description: Demonstrates XPath 1.0 predicate filtering using XML_Lib.
// Covers positional predicates ([1], [last()]), attribute value tests
// ([@category='web']), child element value tests ([year=2003]), and
// compound/nested predicates.
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
    <title lang="en">XQuery Kick Start</title>
    <author>James McGovern</author>
    <year>2003</year>
    <price>49.99</price>
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
  xl::XPath xp(gXml.root());
  try {
    const auto nodes = xp.evaluate(expr);
    PLOG_INFO << "    expression  : " << expr;
    PLOG_INFO << "    result count: " << nodes.size();
    for (const auto *n : nodes) {
      const std::string title = xp.evaluateString(
        "string(title)");// evaluated from context — use evaluateString for display
      // Just show the element name; use a separate XPath for title text
      if (xl::isA<xl::Element>(*n) || xl::isA<xl::Root>(*n)) {
        const std::string nm = xl::NRef<xl::Element>(*n).name();
        PLOG_INFO << "    element     : <" << nm << ">";
      }
    }
  } catch (const xl::XPath::Error &e) {
    PLOG_ERROR << "    XPath error : " << e.what();
  }
}

// ---- Main ------------------------------------------------------------------

int main()
{
  init(plog::debug, "XML_XPath_Predicates.log");
  PLOG_INFO << "XML_XPath_Predicates started ...";

  xl::BufferSource source{ kBookstore };
  gXml.parse(source);
  xl::XPath xp(gXml.root());

  PLOG_INFO << "--- Positional predicates ---";
  query("First book  [//book[1]]",               "//book[1]");
  query("Last book   [//book[last()]]",           "//book[last()]");
  query("Second book [//book[2]]",               "//book[2]");
  query("All but first [//book[position()>1]]",  "//book[position()>1]");

  PLOG_INFO << "--- Attribute value predicates ---";
  query("Books with category='web'   [//book[@category='web']]",      "//book[@category='web']");
  query("Books with category='cooking' [//book[@category='cooking']]","//book[@category='cooking']");
  query("Books that have a category attr [//book[@category]]",        "//book[@category]");
  query("Titles with lang='en' [//title[@lang='en']]",                "//title[@lang='en']");

  PLOG_INFO << "--- Child element value predicates ---";
  query("Books where year=2003 [//book[year=2003]]",                  "//book[year=2003]");
  query("Books where year=2005 [//book[year=2005]]",                  "//book[year=2005]");
  query("Books with price>35   [//book[price>35]]",                   "//book[price>35]");
  query("Books with price<35   [//book[price<35]]",                   "//book[price<35]");

  PLOG_INFO << "--- Compound predicates ---";
  query("Web books published in 2003 [//book[@category='web'][year=2003]]",
        "//book[@category='web'][year=2003]");
  query("First web book [//book[@category='web'][1]]",
        "//book[@category='web'][1]");

  PLOG_INFO << "--- Using evaluateString on filtered results ---";
  PLOG_INFO << "  Title of first book       : "
            << xp.evaluateString("string(//book[1]/title)");
  PLOG_INFO << "  Title of last book        : "
            << xp.evaluateString("string(//book[last()]/title)");
  PLOG_INFO << "  Title of first web book   : "
            << xp.evaluateString("string(//book[@category='web'][1]/title)");
  PLOG_INFO << "  Price of cheapest (first) : "
            << xp.evaluateString("string(//book[price<35][1]/price)");

  PLOG_INFO << "XML_XPath_Predicates exited.";
  return 0;
}
