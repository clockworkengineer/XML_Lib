
//
// Unit Tests: XML_Lib_Tests_XElement.cpp
//
// Description: Create and use XElement XNode
//

#include "XML_Lib_Tests.hpp"

using namespace XML_Lib;

TEST_CASE("Create and use XElement XNode.","[XNode][XElement][API]")
{
  SECTION("Create default XElement XNode.", "[XML][XNode][XElement][API]")
  {
    REQUIRE_NOTHROW(XElement());
  }
  // SECTION("Create XElement XNode.", "[XML][XNode][XElement][API]")
  // {
  //   REQUIRE_NOTHROW(XElement("This is a test comment."));
  // }
  // SECTION("Create XElement XNode and fetch comment related to it.", "[XML][XNode][XElement][API]")
  // {
  //   XElement XElement = XElement("This is a test comment.");
  //   REQUIRE(XElement.comment() == "This is a test comment.");
  // }
  // SECTION("Create and use XElement using make/JRef API.", "[XML][XNode][XElement][Make][XRef][API]")
  // {
  //   XNode xNode = XNode::make<XElement>("This is a test comment.");
  //   REQUIRE_FALSE(!xNode.isComment());
  //   REQUIRE(XRef<XElement>(xNode).comment()=="This is a test comment.");
  // }
}

