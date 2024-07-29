#include "XML_Lib_Tests.hpp"

TEST_CASE("Parse CDATA section.", "[XML][Parse][CDATA]")
{
  XML xml;
  SECTION("Parse XML root containing CDATA containing a XML tags", "[XML][Parse][CDATA]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
      " <root>\n"
      "   <![CDATA[<message> Welcome to Tutorials Point </message>   ]]>   "
      "</root>\n"
    };
    REQUIRE_NOTHROW(xml.parse(source));
  }
  SECTION("Parse XML root containing CDATA containing a XML tags and check contents", "[XML][Parse][CDATA]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
      " <root>\n"
      "   <![CDATA[<message> Welcome to Tutorials Point </message>]]>   "
      "</root>\n"
    };
    xml.parse(source);
    REQUIRE(xml.root().getContents() == "\n   <message> Welcome to Tutorials Point </message>   ");
    REQUIRE(XRef<CDATA>(xml.root().getChildren()[1]).value() == "<message> Welcome to Tutorials Point </message>");
  }
  SECTION("Parse XML root containing CDATA containing nested CDATA ", "[XML][Parse][CDATA]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
      " <root>\n"
      "   <![CDATA[< Test test <![CDATA[ Test text ]]> ]]>\n"
      "   </root>\n "
    };
    REQUIRE_THROWS_WITH(
      xml.parse(source), "XML Syntax Error [Line: 3 Column: 40] Nesting of CDATA sections is not allowed.");
  }
  SECTION("Parse XML root containing CDATA containing ]]> ", "[XML][Parse][CDATA]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
      " <root>\n"
      "   <![CDATA[< Test Test text ]]>  ]]>\n"
      "   </root>\n"
    };
    REQUIRE_THROWS_WITH(
      xml.parse(source), "XML Syntax Error [Line: 4 Column: 1] ']]>' invalid in element content area.");
  }
  SECTION("Parse XML root containing CDATA sequence containing a valid ']]>' ", "[XML][Parse][CDATA]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
      "<root>"
      "<![CDATA[]]]]><![CDATA[>]]></root>\n"
    };
    REQUIRE_NOTHROW(xml.parse(source));
    REQUIRE(xml.root().getContents() == "]]>");
  }
}