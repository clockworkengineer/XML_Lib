#include "XML_Lib_Tests.hpp"

TEST_CASE("Check the parsing of character entities/reference.", "[XML][Parse][Entities]")
{
  XML xml;
  SECTION("Parse entity &amp; in contents area", "[XML][Parse][Entities]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<root> &amp; </root>\n"
    };
    xml.parse(source);
    REQUIRE(NRef<Element>(xml.root()).getContents() == " & ");
  }
  SECTION("Parse entity &quot; in contents area", "[XML][Parse][Entities]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      " <root> &quot; </root>\n"
    };
    xml.parse(source);
    REQUIRE(NRef<Element>(xml.root()).getContents() == " \" ");
  }
  SECTION("Parse entities &apos; &lt; &gt; in contents area", "[XML][Parse][Entities]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      " <root> &apos; &lt; &gt; </root>\n"
    };
    xml.parse(source);
    REQUIRE(NRef<Element>(xml.root()).getContents() == " ' < > ");
  }
  SECTION("Parse reference &#x00A5; in contents area", "[XML][Parse][Entities]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      " <root> &#x00A5; </root>\n"
    };
    xml.parse(source);
    REQUIRE(NRef<Element>(xml.root()).getContents() == " ¥ ");
  }
  SECTION("Parse reference &#163; in contents area", "[XML][Parse][Entities]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<root> &#163; </root>\n"
    };
    xml.parse(source);
    REQUIRE(NRef<Element>(xml.root()).getContents() == " £ ");
  }
  SECTION("Parse reference &#x00As; (invalid hex value) in contents area", "[XML][Parse][Entities]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      " <root> &#x00As; </root>\n"
    };
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error [Line: 2 Column: 21] Cannot convert character reference.");
  }
  SECTION("Parse reference &#16A; (invalid decimal value) in contents area", "[XML][Parse][Entities]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<root> &#16A; </root>\n"
    };
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error [Line: 2 Column: 18] Cannot convert character reference.");
  }
  SECTION("Parse entity &amp;&quot;&apos;&gt;&lt; in attribute value", "[XML][Parse][Entities]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      " <root attr1=\" &amp;&quot;&apos;&gt;&lt; \">\n"
      "</root>\n"
    };
    xml.parse(source);
    REQUIRE(NRef<Element>(xml.root()).getAttributes().size() == 1);
    REQUIRE(NRef<Element>(xml.root())["attr1"].getParsed() == " &#x26;&#x22;&#x27;&#x3E;&#x3C; ");
  }
  SECTION("Parse references &#x00A5;&#163 in attribute value", "[XML][Parse][Entities]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<root attr1=\" &#x00A5;&#163; \"></root>\n"
    };
    xml.parse(source);
    REQUIRE(NRef<Element>(xml.root()).getAttributes().size() == 1);
    REQUIRE(NRef<Element>(xml.root())["attr1"].getParsed() == " ¥£ ");
  }
  SECTION("Parse entity &lamp; (non-existent named) in contents area", "[XML][Parse][Entities]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<root> &lamp; </root>\n"
    };
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error: Entity '&lamp;' does not exist.");
  }
  SECTION("Parse entity &lamp; (non-existent named) in attribute.", "[XML][Parse][Entities]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<root attr1=\" &lamp; \"> </root>\n"
    };
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error: Entity '&lamp;' does not exist.");
  }
  SECTION("Parse entity &; (null entity) in contents area", "[XML][Parse][Entities]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<root> &; </root>\n"
    };
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error [Line: 2 Column: 14] Invalid name '' encountered.");
  }
  SECTION("Parse entity &; (null entity in attribute.", "[XML][Parse][Entities]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<root attr1=\" &; \"> </root>\n"
    };
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error [Line: 2 Column: 21] Invalid name '' encountered.");
  }
  SECTION("Parse character entity &#; (null entity) in contents area", "[XML][Parse][Entities]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<root> &#; </root>\n"
    };
    REQUIRE_THROWS_WITH(
      xml.parse(source), "XML Syntax Error [Line: 2 Column: 15] Character reference invalid character.");
  }
  SECTION("Parse character entity &#; (null entity in attribute", "[XML][Parse][Entities]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<root attr1=\" &#; \"> </root>\n"
    };
    REQUIRE_THROWS_WITH(
      xml.parse(source), "XML Syntax Error [Line: 2 Column: 22] Character reference invalid character.");
  }
}