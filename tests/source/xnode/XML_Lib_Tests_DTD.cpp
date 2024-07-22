//
// Unit Tests: XML_Lib_Tests_DTD.cpp
//
// Description: Create and use DTD XNode
//

#include "XML_Lib_Tests.hpp"

using namespace XML_Lib;

TEST_CASE("Create and use DTD XNode.","[XNode][DTD][API]")
{
  SECTION("Create DTD XNode.", "[XML][XNode][DTD][API]")
  {
    XML_EntityMapper mapper;
    REQUIRE_NOTHROW(DTD(mapper));
  }
  SECTION("Create DTD XNode and check root name hasn't been ser.", "[XML][XNode][DTD][API]")
  {
    XML_EntityMapper mapper;
    DTD xDTD (mapper);
    REQUIRE(xDTD.getRootName()=="");
    REQUIRE(xDTD.getElementCount()==0);
  }
  SECTION("Create and use DTD using make/XRef API.", "[XML][XNode][DTD][Make][XRef][API]")
  {
    XML_EntityMapper mapper;
    XNode xNode = XNode::make<DTD>(mapper);
    REQUIRE_FALSE(!isA<DTD>(xNode));
    REQUIRE(XRef<DTD>(xNode).getRootName()=="");
    REQUIRE(XRef<DTD>(xNode).getElementCount()==0);
  }
}



