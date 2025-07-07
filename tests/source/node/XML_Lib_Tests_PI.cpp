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
  SECTION("Create and use PI using make/XRef API.", "[XML][Node][PI][Make][XRef][API]")
  {
    Node xNode = Node::make<PI>("xml-stylesheet", "type='text/xsl' href='style.xsl'");
    REQUIRE_FALSE(!isA<PI>(xNode));
    REQUIRE(XRef<PI>(xNode).name()=="xml-stylesheet");
    REQUIRE(XRef<PI>(xNode).parameters()=="type='text/xsl' href='style.xsl'");
  }
}



