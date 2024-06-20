//
// Unit Tests: XML_Lib_Tests_XDTD.cpp
//
// Description: Create and use XDTD XNode
//

#include "XML_Lib_Tests.hpp"

using namespace XML_Lib;

TEST_CASE("Create and use XDTD XNode.","[XNode][XDTD][API]")
{
  SECTION("Create XDTD XNode.", "[XML][XNode][XDTD][API]")
  {
    XML_EntityMapper mapper;
    REQUIRE_NOTHROW(XDTD(mapper));
  }
  SECTION("Create XDTD XNode and check root name hasn't been ser.", "[XML][XNode][XDTD][API]")
  {
    XML_EntityMapper mapper;
    XDTD xDTD (mapper);
    REQUIRE(xDTD.getRootName()=="");
    REQUIRE(xDTD.getElementCount()==0);
  }
  SECTION("Create and use XDTD using make/XRef API.", "[XML][XNode][XDTD][Make][XRef][API]")
  {
    XML_EntityMapper mapper;
    XNode xNode = XNode::make<XDTD>(mapper);
    REQUIRE_FALSE(!xNode.isDTD());
    REQUIRE(XRef<XDTD>(xNode).getRootName()=="");
    REQUIRE(XRef<XDTD>(xNode).getElementCount()==0);
  }
}



