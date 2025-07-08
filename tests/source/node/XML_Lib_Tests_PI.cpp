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



