#include "XML_Lib_Tests.hpp"

TEST_CASE("Create and use Comment XNode.","[XNode][Comment][API]")
{
  SECTION("Create default Comment XNode.", "[XML][XNode][Comment][API]")
  {
    REQUIRE_NOTHROW(Comment());
  }
  SECTION("Create Comment XNode.", "[XML][XNode][Comment][API]")
  {
    REQUIRE_NOTHROW(Comment("This is a test comment."));
  }
  SECTION("Create Comment XNode and fetch comment related to it.", "[XML][XNode][Comment][API]")
  {
    auto xComment = Comment("This is a test comment.");
    REQUIRE(xComment.value() == "This is a test comment.");
  }
  SECTION("Create and use Comment using make/XRef API.", "[XML][XNode][Comment][Make][XRef][API]")
  {
    XNode xNode = XNode::make<Comment>("This is a test comment.");
    REQUIRE_FALSE(!isA<Comment>(xNode));
    REQUIRE(XRef<Comment>(xNode).value()=="This is a test comment.");
  }
}

