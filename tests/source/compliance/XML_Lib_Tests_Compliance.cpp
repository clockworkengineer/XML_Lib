#include "XML_Lib_Tests.hpp"

TEST_CASE("W3C-derived XML parsing compliance", "[Compliance][XML]")
{
  SECTION("Valid simple XML fixture parses successfully.", "[Compliance][XML]")
  {
    XML xml;
    REQUIRE_NOTHROW(xml.parse(FileSource(prefixTestDataPath("w3c/xml/valid_simple.xml"))));
  }
}

#if defined(XML_LIB_ENABLE_DTD)
TEST_CASE("W3C-derived DTD validation compliance", "[Compliance][DTD]")
{
  SECTION("XML with external DTD subset validates successfully.", "[Compliance][DTD]")
  {
    const std::string xmlText = XML::fromFile(prefixTestDataPath("w3c/dtd/valid_root.xml"));
    XML xml(xmlText);
    REQUIRE_NOTHROW(xml.validate());
  }
}
#endif

#if defined(XML_LIB_ENABLE_XPATH)
TEST_CASE("W3C-derived XPath evaluation compliance", "[Compliance][XPath]")
{
  SECTION("XPath count and attribute filtering work as expected.", "[Compliance][XPath]")
  {
    XML xml(XML::fromFile(prefixTestDataPath("w3c/xpath/axis_example.xml")));
    XPath xp(xml.root());
    REQUIRE(xp.evaluateString("count(//item)") == "4");
    REQUIRE(xp.evaluateString("string(//item[@type='book'][1])") == "The Art of XML");
  }
}
#endif

#if defined(XML_LIB_ENABLE_XSD)
TEST_CASE("W3C-derived XSD validation compliance", "[Compliance][XSD]")
{
  SECTION("XSD schema validates a simple string element.", "[Compliance][XSD]")
  {
    XML xml(XML::fromFile(prefixTestDataPath("w3c/xsd/simple_string.xml")));
    REQUIRE_NOTHROW(xml.validate(XML::fromFile(prefixTestDataPath("w3c/xsd/simple_string.xsd"))));
  }
}
#endif
