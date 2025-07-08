#include "XML_Lib_Tests.hpp"

XMLValue createXMLValue()
{
  return XMLValue("unparsed", "parsed");
}

TEST_CASE("Create and use EntityReference Node.","[Node][EntityReference][API]")
{
  SECTION("Create EntityReference Node.", "[XML][Node][EntityReference][API]")
  {
    REQUIRE_NOTHROW(EntityReference(createXMLValue()));
  }
  SECTION("Create EntityReference Node and fetch value related to it.", "[XML][Node][EntityReference][API]")
  {
    auto xEntityReference = EntityReference(createXMLValue());
    REQUIRE(xEntityReference.value().getParsed() == "parsed");
    REQUIRE(xEntityReference.value().getUnparsed() == "unparsed");
  }
  SECTION("Create and use EntityReference using make/NRef API.", "[XML][Node][EntityReference][Make][NRef][API]")
  {
    Node xNode = Node::make<EntityReference>(createXMLValue());
    REQUIRE_FALSE(!isA<EntityReference>(xNode));
    REQUIRE(NRef<EntityReference>(xNode).value().getParsed() == "parsed");
    REQUIRE(NRef<EntityReference>(xNode).value().getUnparsed() == "unparsed");
  }
}



