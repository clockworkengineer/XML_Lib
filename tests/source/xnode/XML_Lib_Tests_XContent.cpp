//
// Unit Tests: XML_Lib_Tests_XContent.cpp
//
// Description: Create and use XContent XNode
//

#include "XML_Lib_Tests.hpp"

using namespace XML_Lib;

TEST_CASE("Create and use XContent XNode.","[XNode][XContent][API]")
{
  SECTION("Create default XContent XNode.", "[XML][XNode][XContent][API]")
  {
    REQUIRE_NOTHROW(Content(""));
  }
  SECTION("Create XContent XNode.", "[XML][XNode][XContent][API]")
  {
    REQUIRE_NOTHROW(Content("This is a test content."));
  }
  SECTION("Create XContent XNode and fetch content related to it.", "[XML][XNode][XContent][API]")
  {
    Content xContent = Content("This is a test comment.");
    REQUIRE(xContent.value() == "This is a test comment.");
    xContent.addContent("More content.");
    REQUIRE(xContent.value() == "This is a test comment.More content.");
  }
  SECTION("Create and use XContent using make/XRef API.", "[XML][XNode][XContent][Make][XRef][API]")
  {
    XNode xNode = XNode::make<Content>("This is a test comment.");
    REQUIRE_FALSE(!isA<Content>(xNode));
    REQUIRE(XRef<Content>(xNode).value()=="This is a test comment.");
    XRef<Content>(xNode).addContent("More content.");
    REQUIRE(XRef<Content>(xNode).value()=="This is a test comment.More content.");
  }
}

