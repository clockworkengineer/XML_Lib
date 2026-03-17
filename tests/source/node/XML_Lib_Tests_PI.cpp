#include "XML_Lib_Tests.hpp"

TEST_CASE("Create and use PI Node.","[Node][PI][API]")
{
  SECTION("Create PI Node.", "[XML][Node][PI][API]")
  {
    REQUIRE_NOTHROW(PI("xml-stylesheet", "type='text/xsl' href='style.xsl'"));
  }
  SECTION("Create PI Node and fetch comment related to it.", "[XML][Node][PI][API]")
  {
    auto xPI = PI("xml-stylesheet", "type='text/xsl' href='style.xsl'");
    REQUIRE(xPI.name() == "xml-stylesheet");
    REQUIRE(xPI.parameters() == "type='text/xsl' href='style.xsl'");

  }
  SECTION("Create and use PI using make/NRef API.", "[XML][Node][PI][Make][NRef][API]")
  {
    Node xNode = Node::make<PI>("xml-stylesheet", "type='text/xsl' href='style.xsl'");
    REQUIRE_FALSE(!isA<PI>(xNode));
    REQUIRE(NRef<PI>(xNode).name()=="xml-stylesheet");
    REQUIRE(NRef<PI>(xNode).parameters()=="type='text/xsl' href='style.xsl'");
  }
}
// Additional tests for PI
TEST_CASE("PI edge cases and integration", "[Node][PI][Edge][Integration]")
{
  SECTION("PI with empty name and parameters", "[XML][Node][PI][Empty]")
  {
    REQUIRE_NOTHROW(PI("", ""));
    auto xPI = PI("", "");
    REQUIRE(xPI.name() == "");
    REQUIRE(xPI.parameters() == "");
  }

  SECTION("PI copy/move semantics", "[XML][Node][PI][CopyMove]")
  {
    PI original("xml-stylesheet", "type='text/xsl' href='style.xsl'");
    PI moved(std::move(original));
    REQUIRE(moved.name() == "xml-stylesheet");
    REQUIRE(moved.parameters() == "type='text/xsl' href='style.xsl'");
  }

  SECTION("PI with unusual characters", "[XML][Node][PI][SpecialChars]")
  {
    std::string specialName = "xml-stylesheet-π";
    std::string specialParams = "type='text/xsl' href='style.xsl' & π";
    REQUIRE_NOTHROW(PI(specialName, specialParams));
    auto xPI = PI(specialName, specialParams);
    REQUIRE(xPI.name() == specialName);
    REQUIRE(xPI.parameters() == specialParams);
  }

  SECTION("PI with long parameters", "[XML][Node][PI][LongParams]")
  {
    std::string longParams(1000, 'a');
    REQUIRE_NOTHROW(PI("xml-stylesheet", longParams));
    auto xPI = PI("xml-stylesheet", longParams);
    REQUIRE(xPI.parameters() == longParams);
  }
}



