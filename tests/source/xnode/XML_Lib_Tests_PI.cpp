#include "XML_Lib_Tests.hpp"

TEST_CASE("Create and use PI XNode.","[XNode][PI][API]")
{
  SECTION("Create PI XNode.", "[XML][XNode][PI][API]")
  {
    REQUIRE_NOTHROW(PI("xml-stylesheet", "type='text/xsl' href='style.xsl'"));
  }
  SECTION("Create PI XNode and fetch comment related to it.", "[XML][XNode][PI][API]")
  {
    auto xPI = PI("xml-stylesheet", "type='text/xsl' href='style.xsl'");
    REQUIRE(xPI.name() == "xml-stylesheet");
    REQUIRE(xPI.parameters() == "type='text/xsl' href='style.xsl'");

  }
  SECTION("Create and use PI using make/XRef API.", "[XML][XNode][PI][Make][XRef][API]")
  {
    XNode xNode = XNode::make<PI>("xml-stylesheet", "type='text/xsl' href='style.xsl'");
    REQUIRE_FALSE(!isA<PI>(xNode));
    REQUIRE(XRef<PI>(xNode).name()=="xml-stylesheet");
    REQUIRE(XRef<PI>(xNode).parameters()=="type='text/xsl' href='style.xsl'");
  }
}



