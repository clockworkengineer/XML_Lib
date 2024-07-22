//
// Created by robt on 29/05/24.
//
// Unit Tests: XML_Lib_Tests_EntityReference.cpp
//
// Description: Create and use EntityReference XNode
//

#include "XML_Lib_Tests.hpp"

using namespace XML_Lib;

XMLValue createXMLValue()
{
  return(XMLValue("unparsed", "parsed"));
}

TEST_CASE("Create and use EntityReference XNode.","[XNode][EntityReference][API]")
{
  SECTION("Create EntityReference XNode.", "[XML][XNode][EntityReference][API]")
  {
    REQUIRE_NOTHROW(EntityReference(createXMLValue()));
  }
  SECTION("Create EntityReference XNode and fetch value related to it.", "[XML][XNode][EntityReference][API]")
  {
    EntityReference xEntityReference = EntityReference(createXMLValue());
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



