//
// Unit Tests: XML_Lib_Tests_XPI.cpp
//
// Description: Create and use XPI XNode
//

#include "XML_Lib_Tests.hpp"

using namespace XML_Lib;

TEST_CASE("Create and use XPI XNode.","[XNode][XPI][API]")
{
  SECTION("Create XPI XNode.", "[XML][XNode][XPI][API]")
  {
    REQUIRE_NOTHROW(PI("xml-stylesheet", "type='text/xsl' href='style.xsl'"));
  }
  SECTION("Create XPI XNode and fetch comment related to it.", "[XML][XNode][XPI][API]")
  {
    PI xPI = PI("xml-stylesheet", "type='text/xsl' href='style.xsl'");
    REQUIRE(xPI.name() == "xml-stylesheet");
    REQUIRE(xPI.parameters() == "type='text/xsl' href='style.xsl'");

  }
  SECTION("Create and use XPI using make/XRef API.", "[XML][XNode][XPI][Make][XRef][API]")
  {
    XNode xNode = XNode::make<PI>("xml-stylesheet", "type='text/xsl' href='style.xsl'");
    REQUIRE_FALSE(!isA<PI>(xNode));
    REQUIRE(XRef<PI>(xNode).name()=="xml-stylesheet");
    REQUIRE(XRef<PI>(xNode).parameters()=="type='text/xsl' href='style.xsl'");
  }
}



