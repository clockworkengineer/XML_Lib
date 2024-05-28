//
// Unit Tests: XML_Lib_Tests_Comment.cpp
//
// Description: Create and use XComment XNode
//

#include "XML_Lib_Tests.hpp"

using namespace XML_Lib;

TEST_CASE("Create and use XComment XNode.","[XNode][XComment][API]")
{
  SECTION("Create default XComment XNode.", "[XML][XNode][XComment][API]")
  {
    REQUIRE_NOTHROW(XComment());
  }
  SECTION("Create XComment XNode.", "[XML][XNode][XComment][API]")
  {
    REQUIRE_NOTHROW(XComment("This is a test comment."));
  }
  SECTION("Create XComment XNode and fetch comment related to it.", "[XML][XNode][XComment][API]")
  {
    XComment xComment = XComment("This is a test comment.");
    REQUIRE(xComment.comment() == "This is a test comment.");
  }
  SECTION("Create and use XComment using make/JRef API.", "[XML][XNode][XComment][Make][XRef][API]")
  {
    XNode xNode = XNode::make<XComment>("This is a test comment.");
    REQUIRE_FALSE(!xNode.isComment());
    REQUIRE(XRef<XComment>(xNode).comment()=="This is a test comment.");
  }
}

