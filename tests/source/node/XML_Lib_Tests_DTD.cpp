#include "XML_Lib_Tests.hpp"

TEST_CASE("Create and use DTD Node.","[Node][DTD][API]")
{
  SECTION("Create DTD Node.", "[XML][Node][DTD][API]")
  {
    XML_EntityMapper mapper;
    REQUIRE_NOTHROW(DTD(mapper));
  }
  SECTION("Create DTD Node and check root name hasn't been ser.", "[XML][Node][DTD][API]")
  {
    XML_EntityMapper mapper;
    DTD xDTD (mapper);
    REQUIRE(xDTD.getRootName()=="");
    REQUIRE(xDTD.getElementCount()==0);
  }
  SECTION("Create and use DTD using make/XRef API.", "[XML][Node][DTD][Make][XRef][API]")
  {
    XML_EntityMapper mapper;
    Node xNode = Node::make<DTD>(mapper);
    REQUIRE_FALSE(!isA<DTD>(xNode));
    REQUIRE(XRef<DTD>(xNode).getRootName()=="");
    REQUIRE(XRef<DTD>(xNode).getElementCount()==0);
  }
}



