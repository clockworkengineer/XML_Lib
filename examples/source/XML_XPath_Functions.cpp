// Program: XML_XPath_Functions
//
// Description: Demonstrates XPath 1.0 built-in functions using XML_Lib.
// Covers node-set functions (count, last, position, name),
// string functions (string, concat, contains, starts-with, substring,
// string-length, normalize-space, translate),
// number functions (number, sum, floor, ceiling, round),
// and boolean functions (boolean, not, true, false).
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

// ---- Helpers ---------------------------------------------------------------

static xl::XML gXml;

static void evalStr(const std::string &label, const std::string &expr, xl::XPath &xp)
{
  std::cout << "  " << label << std::endl;
  std::cout << "    " << expr << " => " << xp.evaluateString(expr) << std::endl;
}

static void evalNum(const std::string &label, const std::string &expr, xl::XPath &xp)
{
  std::cout << "  " << label << std::endl;
  std::cout << "    " << expr << " => " << xp.evaluateNumber(expr) << std::endl;
}

static void evalBool(const std::string &label, const std::string &expr, xl::XPath &xp)
{
  std::cout << "  " << label << std::endl;
  std::cout << "    " << expr << " => " << (xp.evaluateBool(expr) ? "true" : "false") << std::endl;
}

// ---- Main ------------------------------------------------------------------

int main()
{
  std::cout << "XML_XPath_Functions started ..."; << std::endl;
  xl::BufferSource source{ kBookstore };
  gXml.parse(source);
  xl::XPath xp(gXml.root());

  // ── Node-set functions ──────────────────────────────────────────────────
  std::cout << "--- Node-set functions ---" << std::endl;
  evalNum("count(//book)                  — total books", "count(//book)", xp);
  evalNum("count(//book[@category='web']) — web books", "count(//book[@category='web'])", xp);
  evalNum("count(//title)                — total titles", "count(//title)", xp);
  evalStr("name(//book[1])               — name of 1st book", "name(//book[1])", xp);
  evalStr("name(/*)                      — name of root", "name(/*)", xp);

  // ── String functions ────────────────────────────────────────────────────
  std::cout << "--- String functions ---" << std::endl;
  evalStr("string(//title[1])                    — first title as string", "string(//title[1])", xp);
  evalStr("concat(//title[1], ' / ', //title[2]) — join two titles",
    "concat(string(//title[1]), ' / ', string(//title[2]))",
    xp);
  evalStr("contains(//title[1], 'Italian')       — does first title contain 'Italian'?",
    "string(contains(//title[1], 'Italian'))",
    xp);
  evalStr("starts-with(//title[1], 'Everyday')   — starts with 'Everyday'?",
    "string(starts-with(//title[1], 'Everyday'))",
    xp);
  evalStr(
    "substring(//title[1], 1, 8)           — first 8 chars of first title", "substring(string(//title[1]), 1, 8)", xp);
  evalStr(
    "substring-before(//title[1], ' ')     — word before first space", "substring-before(string(//title[1]), ' ')", xp);
  evalStr(
    "substring-after(//title[1], ' ')      — text after first space", "substring-after(string(//title[1]), ' ')", xp);
  evalNum(
    "string-length(//title[1])             — character count of first title", "string-length(string(//title[1]))", xp);
  evalStr("normalize-space('  hello   world  ')  — collapse whitespace", "normalize-space('  hello   world  ')", xp);
  evalStr(
    "translate(//title[1], 'aeiou', '*****') — vowels → *", "translate(string(//title[1]), 'aeiou', '*****')", xp);

  // ── Number functions ────────────────────────────────────────────────────
  std::cout << "--- Number functions ---" << std::endl;
  evalNum("number(//book[1]/price)             — price of first book as number", "number(string(//book[1]/price))", xp);
  evalNum("sum(//price)                        — total price of all books", "sum(//price)", xp);
  evalNum("floor(49.7)                         — floor of 49.7", "floor(49.7)", xp);
  evalNum("ceiling(49.2)                       — ceiling of 49.2", "ceiling(49.2)", xp);
  evalNum("round(49.5)                         — round 49.5", "round(49.5)", xp);
  evalNum("round(49.4)                         — round 49.4", "round(49.4)", xp);

  // ── Boolean functions ───────────────────────────────────────────────────
  std::cout << "--- Boolean functions ---" << std::endl;
  evalBool("boolean(//book)                    — node-set is non-empty → true", "boolean(//book)", xp);
  evalBool("boolean(//nonexistent)             — empty node-set → false", "boolean(//nonexistent)", xp);
  evalBool("not(//nonexistent)                 — not of false → true", "not(//nonexistent)", xp);
  evalBool("true()                             — literal true", "true()", xp);
  evalBool("false()                            — literal false", "false()", xp);
  evalBool("count(//book) > 2                  — arithmetic comparison", "count(//book) > 2", xp);
  evalBool("count(//book[@category='web']) = 2 — equality comparison", "count(//book[@category='web']) = 2", xp);

  // ── Contains in a predicate ─────────────────────────────────────────────
  std::cout << "--- Functions inside predicates ---" << std::endl;
  const auto xmlTitles = gXml.xpath("//title[contains(., 'XML')]");
  std::cout << "  //title[contains(., 'XML')] — titles containing 'XML': " << xmlTitles.size() << std::endl;
  for (const auto *n : xmlTitles) {
    if (xl::isA<xl::Element>(*n)) {
      std::cout << "    text: " << xp.evaluateString("string(//title[contains(., 'XML')])") << std::endl;
    }
  }

  const auto longTitles = gXml.xpath("//title[string-length(.) > 12]");
  std::cout << "  //title[string-length(.) > 12] — titles longer than 12 chars: " << longTitles.size(); << std::endl;
  std::cout << "XML_XPath_Functions exited." << std::endl;
  return 0;
}
