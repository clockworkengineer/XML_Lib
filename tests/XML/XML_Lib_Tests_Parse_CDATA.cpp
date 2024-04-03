//
// Unit Tests: XML_Lib_Tests_Parse_CDATA
//
// Description:
//

#include "XML_Lib_Tests.hpp"

using namespace XML_Lib;

TEST_CASE("Parse CDATA section.", "[XML][Parse][CDATA]")
{
  XML xml;
  SECTION("Parse XML root containing CDDATA containing a XML tags", "[XML][Parse][CDATA]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
      " <root>\n"
      "   <![CDATA[<message> Welcome to Tutorials Point </message>   ]]>   "
      "</root>\n"
    };
    REQUIRE_NOTHROW(xml.parse(source));
  }
  SECTION("Parse XML root containing CDDATA containing a XML tags and check contents", "[XML][Parse][CDATA]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
      " <root>\n"
      "   <![CDATA[<message> Welcome to Tutorials Point </message>]]>   "
      "</root>\n"
    };
    xml.parse(source);
    REQUIRE(xml.root().getContents() == "\n   <message> Welcome to Tutorials Point </message>   ");
    REQUIRE(XRef<XCDATA>(xml.root().getChildren()[1]).CDATA() == "<message> Welcome to Tutorials Point </message>");
  }
  SECTION("Parse XML root containing CDDATA containing nested CDATA ", "[XML][Parse][CDATA]")
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
  SECTION("Parse XML root containing CDDATA containing ]]> ", "[XML][Parse][CDATA]")
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
  SECTION("Parse XML root containing CDDATA sequence containing a valid ']]>' ", "[XML][Parse][CDATA]")
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