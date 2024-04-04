//
// Unit Tests: XML_Lib_Tests_Parse_Entities
//
// Description:
//

#include "XML_Lib_Tests.hpp"

using namespace XML_Lib;

TEST_CASE("Check the pasring of character entities/reference.", "[XML][Parse][Entities]")
{
  XML xml;
  SECTION("Parse entity &amp; in contents area", "[XML][Parse][Entities]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
      "<root> &amp; </root>\n"
    };
    xml.parse(source);
    REQUIRE(XRef<XElement>(xml.root()).getContents() == " & ");
  }
  SECTION("Parse entity &quot; in contents area", "[XML][Parse][Entities]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
      " <root> &quot; </root>\n"
    };
    xml.parse(source);
    REQUIRE(XRef<XElement>(xml.root()).getContents() == " \" ");
  }
  SECTION("Parse entities &apos; &lt; &gt; in contents area", "[XML][Parse][Entities]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
      " <root> &apos; &lt; &gt; </root>\n"
    };
    xml.parse(source);
    REQUIRE(XRef<XElement>(xml.root()).getContents() == " ' < > ");
  }
  SECTION("Parse reference &#x00A5; in contents area", "[XML][Parse][Entities]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
      " <root> &#x00A5; </root>\n"
    };
    xml.parse(source);
    REQUIRE(XRef<XElement>(xml.root()).getContents() == " ¥ ");
  }
  SECTION("Parse reference &#163; in contents area", "[XML][Parse][Entities]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
      "<root> &#163; </root>\n"
    };
    xml.parse(source);
    REQUIRE(XRef<XElement>(xml.root()).getContents() == " £ ");
  }
  SECTION("Parse entity &amp;&quot;&apos;&gt;&lt; in attribute value", "[XML][Parse][Entities]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
      " <root attr1=\" &amp;&quot;&apos;&gt;&lt; \">\n"
      "</root>\n"
    };
    xml.parse(source);
    REQUIRE(XRef<XElement>(xml.root()).getAttributeList().size() == 1);
    REQUIRE(XRef<XElement>(xml.root()).getAttribute("attr1").getParsed() == " &#x26;&#x22;&#x27;&#x3E;&#x3C; ");
  }
  SECTION("Parse references &#x00A5;&#163 in attribute value", "[XML][Parse][Entities]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
      "<root attr1=\" &#x00A5;&#163; \"></root>\n"
    };
    xml.parse(source);
    REQUIRE(XRef<XElement>(xml.root()).getAttributeList().size() == 1);
    REQUIRE(XRef<XElement>(xml.root()).getAttribute("attr1").getParsed() == " ¥£ ");
  }
  SECTION("Parse entity &lamp; (non-existant named) in contents area", "[XML][Parse][Entities]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
      "<root> &lamp; </root>\n"
    };
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error: Entity '&lamp;' does not exist.");
  }
  SECTION("Parse entity &lamp; (non-existant named) in attribute.", "[XML][Parse][Entities]")
  {
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
      "<root attr1=\" &lamp; \"> </root>\n"
    };
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error: Entity '&lamp;' does not exist.");
  }
}