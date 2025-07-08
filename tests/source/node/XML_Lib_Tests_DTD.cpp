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
  SECTION("Create and use DTD using make/NRef API.", "[XML][Node][DTD][Make][NRef][API]")
  {
    XML_EntityMapper mapper;
    Node xNode = Node::make<DTD>(mapper);
    REQUIRE_FALSE(!isA<DTD>(xNode));
    REQUIRE(NRef<DTD>(xNode).getRootName()=="");
    REQUIRE(NRef<DTD>(xNode).getElementCount()==0);
  }
}



