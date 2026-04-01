#include "XML_Lib_Tests.hpp"

// ============================================================
// Helper: parse xml, run xpath, return node count.
// Only safe for checking .size(); do NOT dereference the returned
// pointers (the XML object is destroyed when this returns).
// ============================================================
static std::vector<const XML_Lib::Node *> xpathQuery(const std::string &xmlStr, const std::string &expr)
{
  XML xml{ xmlStr };
  return xml.xpath(expr);
}

// ============================================================
// Inline bookstore XML used throughout the tests
// ============================================================
static const std::string kBookstore{
  "<?xml version=\"1.0\"?>"
  "<bookstore>"
  "<book category=\"cooking\">"
  "<title lang=\"en\">Everyday Italian</title>"
  "<author>Giada De Laurentiis</author>"
  "<year>2005</year>"
  "<price>30.00</price>"
  "</book>"
  "<book category=\"children\">"
  "<title lang=\"en\">Harry Potter</title>"
  "<author>J K. Rowling</author>"
  "<year>2005</year>"
  "<price>29.99</price>"
  "</book>"
  "<book category=\"web\">"
  "<title lang=\"en\">XQuery Kick Start</title>"
  "<author>James McGovern</author>"
  "<author>Per Bothner</author>"
  "<year>2003</year>"
  "<price>49.99</price>"
  "</book>"
  "<book category=\"web\">"
  "<title lang=\"en\">Learning XML</title>"
  "<author>Erik T. Ray</author>"
  "<year>2003</year>"
  "<price>39.95</price>"
  "</book>"
  "</bookstore>"
};

// ============================================================
// TEST CASES
// ============================================================

TEST_CASE("XPath basic navigation", "[XML][XPath][Navigation]")
{
  SECTION("Select root element with /bookstore")
  {
    XML xml{ kBookstore };
    auto nodes = xml.xpath("/bookstore");
    REQUIRE(nodes.size() == 1);
    REQUIRE(isA<XML_Lib::Root>(*nodes[0]));
    REQUIRE(NRef<XML_Lib::Root>(*nodes[0]).name() == "bookstore");
  }

  SECTION("Select all books with //book")
  {
    auto nodes = xpathQuery(kBookstore, "//book");
    REQUIRE(nodes.size() == 4);
  }

  SECTION("Select book children of bookstore with /bookstore/book")
  {
    auto nodes = xpathQuery(kBookstore, "/bookstore/book");
    REQUIRE(nodes.size() == 4);
  }

  SECTION("Select titles of books with //book/title")
  {
    auto nodes = xpathQuery(kBookstore, "//book/title");
    REQUIRE(nodes.size() == 4);
  }

  SECTION("Wildcard child::* returns direct children of root")
  {
    auto nodes = xpathQuery(kBookstore, "/bookstore/*");
    REQUIRE(nodes.size() == 4);
  }

  SECTION("Wildcard //title/* returns nothing (titles have text, not element children)")
  {
    auto nodes = xpathQuery(kBookstore, "//title/*");
    REQUIRE(nodes.empty());
  }

  SECTION("Select all elements with //*")
  {
    auto nodes = xpathQuery(kBookstore, "//*");
    // bookstore + 4 books * (title+author+year+price) = 1+4+4*4 = 1+4+16=21 but 3rd book has 2 authors so 22
    REQUIRE(nodes.size() == 22);
  }
}

TEST_CASE("XPath self and parent axes", "[XML][XPath][Axes]")
{
  SECTION("Dot (.) returns context node (root element)")
  {
    XML xml{ kBookstore };
    auto nodes = xml.xpath(".");
    REQUIRE(nodes.size() == 1);
    REQUIRE(isA<XML_Lib::Root>(*nodes[0]));
  }

  SECTION("DotDot (..) from /bookstore/book would need relative context; self::* on root")
  {
    XML xml{ kBookstore };
    auto nodes = xml.xpath("self::bookstore");
    REQUIRE(nodes.size() == 1);
    REQUIRE(isA<XML_Lib::Root>(*nodes[0]));
  }

  SECTION("descendant::title finds all titles")
  {
    auto nodes = xpathQuery(kBookstore, "descendant::title");
    REQUIRE(nodes.size() == 4);
  }

  SECTION("descendant-or-self::book finds all book elements")
  {
    auto nodes = xpathQuery(kBookstore, "descendant-or-self::book");
    REQUIRE(nodes.size() == 4);
  }
}

TEST_CASE("XPath predicates", "[XML][XPath][Predicates]")
{
  SECTION("First book: //book[1]")
  {
    auto nodes = xpathQuery(kBookstore, "//book[1]");
    REQUIRE(nodes.size() == 1);
    // First book is the cooking one
    XML xml{ kBookstore };
    XPath xp(xml.root());
    REQUIRE(xp.evaluateString("string(//book[1]/title)") == "Everyday Italian");
  }

  SECTION("Last book: //book[last()]")
  {
    XML xml{ kBookstore };
    XPath xp(xml.root());
    REQUIRE(xp.evaluateString("string(//book[last()]/title)") == "Learning XML");
  }

  SECTION("Filter by attribute value: //book[@category='cooking']")
  {
    auto nodes = xpathQuery(kBookstore, "//book[@category='cooking']");
    REQUIRE(nodes.size() == 1);
  }

  SECTION("Filter by attribute value: //book[@category='web']")
  {
    auto nodes = xpathQuery(kBookstore, "//book[@category='web']");
    REQUIRE(nodes.size() == 2);
  }

  SECTION("Filter by child text: //book[year=2003]")
  {
    // year element text = "2003"
    auto nodes = xpathQuery(kBookstore, "//book[year=2003]");
    REQUIRE(nodes.size() == 2);
  }

  SECTION("Numeric position predicate: //book[position()=2]")
  {
    XML xml{ kBookstore };
    XPath xp(xml.root());
    REQUIRE(xp.evaluateString("string(//book[position()=2]/title)") == "Harry Potter");
  }
}

TEST_CASE("XPath node-set functions", "[XML][XPath][Functions][NodeSet]")
{
  SECTION("count(//book) returns 4")
  {
    XML xml{ kBookstore };
    XPath xp(xml.root());
    REQUIRE(xp.evaluateNumber("count(//book)") == 4.0);
  }

  SECTION("count(//author) returns 5 (one book has 2 authors)")
  {
    XML xml{ kBookstore };
    XPath xp(xml.root());
    REQUIRE(xp.evaluateNumber("count(//author)") == 5.0);
  }

  SECTION("name(//book[1]) returns 'book'")
  {
    XML xml{ kBookstore };
    XPath xp(xml.root());
    REQUIRE(xp.evaluateString("name(//book[1])") == "book");
  }

  SECTION("local-name(//title[1]) returns 'title'")
  {
    XML xml{ kBookstore };
    XPath xp(xml.root());
    REQUIRE(xp.evaluateString("local-name(//title[1])") == "title");
  }
}

TEST_CASE("XPath string functions", "[XML][XPath][Functions][String]")
{
  SECTION("string(//book[1]/title) returns text content")
  {
    XML xml{ kBookstore };
    XPath xp(xml.root());
    REQUIRE(xp.evaluateString("string(//book[1]/title)") == "Everyday Italian");
  }

  SECTION("contains(string(//book[1]/title),'Italian')")
  {
    XML xml{ kBookstore };
    XPath xp(xml.root());
    REQUIRE(xp.evaluateBool("contains(string(//book[1]/title),'Italian')"));
  }

  SECTION("starts-with(string(//book[2]/title),'Harry')")
  {
    XML xml{ kBookstore };
    XPath xp(xml.root());
    REQUIRE(xp.evaluateBool("starts-with(string(//book[2]/title),'Harry')"));
  }

  SECTION("string-length('Hello') = 5")
  {
    XML xml{ kBookstore };
    XPath xp(xml.root());
    REQUIRE(xp.evaluateNumber("string-length('Hello')") == 5.0);
  }

  SECTION("normalize-space('  hello   world  ') = 'hello world'")
  {
    XML xml{ kBookstore };
    XPath xp(xml.root());
    REQUIRE(xp.evaluateString("normalize-space('  hello   world  ')") == "hello world");
  }

  SECTION("concat('Hello',' ','World') = 'Hello World'")
  {
    XML xml{ kBookstore };
    XPath xp(xml.root());
    REQUIRE(xp.evaluateString("concat('Hello',' ','World')") == "Hello World");
  }

  SECTION("substring('12345',2,3) = '234'")
  {
    XML xml{ kBookstore };
    XPath xp(xml.root());
    REQUIRE(xp.evaluateString("substring('12345',2,3)") == "234");
  }

  SECTION("translate('abc','abc','ABC') = 'ABC'")
  {
    XML xml{ kBookstore };
    XPath xp(xml.root());
    REQUIRE(xp.evaluateString("translate('abc','abc','ABC')") == "ABC");
  }

  SECTION("substring-before('2003/06','/')= '2003'")
  {
    XML xml{ kBookstore };
    XPath xp(xml.root());
    REQUIRE(xp.evaluateString("substring-before('2003/06','/')") == "2003");
  }

  SECTION("substring-after('2003/06','/')= '06'")
  {
    XML xml{ kBookstore };
    XPath xp(xml.root());
    REQUIRE(xp.evaluateString("substring-after('2003/06','/')") == "06");
  }
}

TEST_CASE("XPath number and boolean functions", "[XML][XPath][Functions][NumBool]")
{
  SECTION("number('42') = 42")
  {
    XML xml{ kBookstore };
    XPath xp(xml.root());
    REQUIRE(xp.evaluateNumber("number('42')") == 42.0);
  }

  SECTION("floor(1.7) = 1")
  {
    XML xml{ kBookstore };
    XPath xp(xml.root());
    REQUIRE(xp.evaluateNumber("floor(1.7)") == 1.0);
  }

  SECTION("ceiling(1.2) = 2")
  {
    XML xml{ kBookstore };
    XPath xp(xml.root());
    REQUIRE(xp.evaluateNumber("ceiling(1.2)") == 2.0);
  }

  SECTION("round(1.5) = 2")
  {
    XML xml{ kBookstore };
    XPath xp(xml.root());
    REQUIRE(xp.evaluateNumber("round(1.5)") == 2.0);
  }

  SECTION("sum(//price) sums all book prices")
  {
    XML xml{ kBookstore };
    XPath xp(xml.root());
    const double total = xp.evaluateNumber("sum(//price)");
    REQUIRE(total == Catch::Approx(149.93).epsilon(0.001));
  }

  SECTION("boolean(//book) is true (non-empty node-set)")
  {
    XML xml{ kBookstore };
    XPath xp(xml.root());
    REQUIRE(xp.evaluateBool("boolean(//book)"));
  }

  SECTION("boolean(//magazine) is false (empty node-set)")
  {
    XML xml{ kBookstore };
    XPath xp(xml.root());
    REQUIRE_FALSE(xp.evaluateBool("boolean(//magazine)"));
  }

  SECTION("not(//magazine) is true")
  {
    XML xml{ kBookstore };
    XPath xp(xml.root());
    REQUIRE(xp.evaluateBool("not(//magazine)"));
  }

  SECTION("true() is true, false() is false")
  {
    XML xml{ kBookstore };
    XPath xp(xml.root());
    REQUIRE(xp.evaluateBool("true()"));
    REQUIRE_FALSE(xp.evaluateBool("false()"));
  }
}

TEST_CASE("XPath abbreviated syntax", "[XML][XPath][Abbreviated]")
{
  SECTION("@category selects attribute nodes from root (no direct attributes)")
  {
    // root element <bookstore> has no attributes; expect empty
    auto nodes = xpathQuery(kBookstore, "@category");
    REQUIRE(nodes.empty());
  }

  SECTION("//book/@category returns parent element nodes for attribute axis")
  {
    // Attribute nodes are returned as their parent element in this implementation
    auto nodes = xpathQuery(kBookstore, "//book/@category");
    REQUIRE(nodes.size() == 4);
  }

  SECTION("//title/@lang returns title parent elements (4 titles)")
  {
    auto nodes = xpathQuery(kBookstore, "//title/@lang");
    REQUIRE(nodes.size() == 4);
  }

  SECTION("// is equivalent to descendant-or-self::node()/")
  {
    auto a = xpathQuery(kBookstore, "//book");
    auto b = xpathQuery(kBookstore, "descendant-or-self::node()/book");
    REQUIRE(a.size() == b.size());
  }
}

TEST_CASE("XPath union expressions", "[XML][XPath][Union]")
{
  SECTION("//title | //author returns all title and author elements")
  {
    // 4 titles + 5 authors = 9
    auto nodes = xpathQuery(kBookstore, "//title | //author");
    REQUIRE(nodes.size() == 9);
  }

  SECTION("//book[1] | //book[last()] returns 2 distinct books")
  {
    auto nodes = xpathQuery(kBookstore, "//book[1] | //book[last()]");
    REQUIRE(nodes.size() == 2);
  }
}

TEST_CASE("XPath relational operators", "[XML][XPath][Operators]")
{
  SECTION("//book[number(price) > 35] returns books over 35")
  {
    // prices: 30.00, 29.99, 49.99, 39.95 → > 35: 49.99 and 39.95
    auto nodes = xpathQuery(kBookstore, "//book[number(price) > 35]");
    REQUIRE(nodes.size() == 2);
  }

  SECTION("//book[number(price) < 35] returns books under 35")
  {
    // prices: 30.00, 29.99 → < 35: 2 books
    auto nodes = xpathQuery(kBookstore, "//book[number(price) < 35]");
    REQUIRE(nodes.size() == 2);
  }
}

TEST_CASE("XPath via XML::xpath() convenience method", "[XML][XPath][Integration]")
{
  SECTION("XML::xpath() returns same results as XPath::evaluate()")
  {
    XML xml{ kBookstore };
    auto via_xml = xml.xpath("//book");
    XPath xp(xml.root());
    auto via_xp = xp.evaluate("//book");
    REQUIRE(via_xml.size() == via_xp.size());
    for (size_t i = 0; i < via_xml.size(); ++i) { REQUIRE(via_xml[i] == via_xp[i]); }
  }
}

TEST_CASE("XPath error handling", "[XML][XPath][Errors]")
{
  SECTION("Invalid XPath expression throws XPath::Error")
  {
    XML xml{ kBookstore };
    XPath xp(xml.root());
    REQUIRE_THROWS_AS(xp.evaluate("//book["), XPath::Error);
  }

  SECTION("Unknown function throws XPath::Error")
  {
    XML xml{ kBookstore };
    XPath xp(xml.root());
    REQUIRE_THROWS_AS(xp.evaluate("unknownFunction()"), XPath::Error);
  }

  SECTION("Non-existent elements return empty node-set (no error)")
  {
    auto nodes = xpathQuery(kBookstore, "//nonexistent");
    REQUIRE(nodes.empty());
  }

  SECTION("Empty expression throws XPath::Error")
  {
    XML xml{ kBookstore };
    XPath xp(xml.root());
    REQUIRE_THROWS_AS(xp.evaluate(""), XPath::Error);
  }
}
