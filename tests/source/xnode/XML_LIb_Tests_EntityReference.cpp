#include "XML_Lib_Tests.hpp"

XMLValue createXMLValue()
{
  return XMLValue("unparsed", "parsed");
}

TEST_CASE("Create and use EntityReference XNode.","[XNode][EntityReference][API]")
{
  SECTION("Create EntityReference XNode.", "[XML][XNode][EntityReference][API]")
  {
    REQUIRE_NOTHROW(EntityReference(createXMLValue()));
  }
  SECTION("Create EntityReference XNode and fetch value related to it.", "[XML][XNode][EntityReference][API]")
  {
    auto xEntityReference = EntityReference(createXMLValue());
    REQUIRE(xEntityReference.value().getParsed() == "parsed");
    REQUIRE(xEntityReference.value().getUnparsed() == "unparsed");
  }
  SECTION("Create and use EntityReference using make/XRef API.", "[XML][XNode][EntityReference][Make][XRef][API]")
  {
    XNode xNode = XNode::make<EntityReference>(createXMLValue());
    REQUIRE_FALSE(!isA<EntityReference>(xNode));
    REQUIRE(XRef<EntityReference>(xNode).value().getParsed() == "parsed");
    REQUIRE(XRef<EntityReference>(xNode).value().getUnparsed() == "unparsed");
  }
}



