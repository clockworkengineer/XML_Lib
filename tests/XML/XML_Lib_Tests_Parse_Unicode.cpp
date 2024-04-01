//
// Unit Tests: XML_Lib_Tests_Parse_Unicode
//
// Description:
//

#include "XML_Lib_Tests.hpp"

using namespace XML_Lib;

TEST_CASE("Parse XML with Unicode characters. ", "[XML][Parse][Unicode]")
{
  XML xml;
  SECTION("Japanese characters", "[XML][Parse][[Unicode]")
  {
    BufferSource source{ "<?xml version=\"1.0\" encoding=\"utf-8\"?><config><start_text>転送</start_text></config>\n" };
    REQUIRE_NOTHROW(xml.parse(source));
  }
  SECTION("Well-formed XML document including Chinese, Armenian and Cyrillic characters", "[XML][Parse][[Unicode]")
  {

    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
      "<俄语 լեզու=\"ռուսերեն\">данные</俄语>\n"
    };
    xml.parse(source);
    auto &xRoot = XRef<XElement>(xml.root());
    REQUIRE(xRoot.name() == "俄语");
    REQUIRE(xRoot.getAttributeList().size() == 1);
    REQUIRE(xRoot.getAttribute("լեզու").getValue() == "ռուսերեն");
    REQUIRE(xRoot.getContents() == "данные");
  }
}