//
// Created by robt on 29/05/24.
//
// Unit Tests: XML_Lib_Tests_XEntityReference.cpp
//
// Description: Create and use XEntityReference XNode
//

#include "XML_Lib_Tests.hpp"

using namespace XML_Lib;

XMLValue createXMLValue()
{
  return(XMLValue("unparsed", "parsed"));
}

TEST_CASE("Create and use XEntityReference XNode.","[XNode][XEntityReference][API]")
{
  SECTION("Create XEntityReference XNode.", "[XML][XNode][XEntityReference][API]")
  {
    REQUIRE_NOTHROW(XEntityReference(createXMLValue()));
  }
  SECTION("Create XEntityReference XNode and fetch value related to it.", "[XML][XNode][XEntityReference][API]")
  {
    XEntityReference xEntityReference = XEntityReference(createXMLValue());
    REQUIRE(xEntityReference.value().getParsed() == "parsed");
    REQUIRE(xEntityReference.value().getUnparsed() == "unparsed");
  }
  SECTION("Create and use XEntityReference using make/XRef API.", "[XML][XNode][XEntityReference][Make][XRef][API]")
  {
    XNode xNode = XNode::make<XEntityReference>(createXMLValue());
    REQUIRE_FALSE(!isA<XEntityReference>(xNode));
    REQUIRE(XRef<XEntityReference>(xNode).value().getParsed() == "parsed");
    REQUIRE(XRef<XEntityReference>(xNode).value().getUnparsed() == "unparsed");
  }
}



