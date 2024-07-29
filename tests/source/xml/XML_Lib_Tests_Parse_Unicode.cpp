#include "XML_Lib_Tests.hpp"

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
    auto &xRoot = XRef<Element>(xml.root());
    REQUIRE(xRoot.name() == "俄语");
    REQUIRE(xRoot.getAttributes().size() == 1);
    REQUIRE(xRoot["լեզու"].getParsed() == "ռուսերեն");
    REQUIRE(xRoot.getContents() == "данные");
  }
}