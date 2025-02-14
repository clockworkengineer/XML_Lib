#include "XML_Lib_Tests.hpp"

TEST_CASE("Create and use Content XNode.","[XNode][Content][API]")
{
  SECTION("Create default Content XNode.", "[XML][XNode][Content][API]")
  {
    REQUIRE_NOTHROW(Content(""));
  }
  SECTION("Create Content XNode.", "[XML][XNode][Content][API]")
  {
    REQUIRE_NOTHROW(Content("This is a test content."));
  }
  SECTION("Create Content XNode and fetch content related to it.", "[XML][XNode][Content][API]")
  {
    auto xContent = Content("This is a test comment.");
    REQUIRE(xContent.value() == "This is a test comment.");
    xContent.addContent("More content.");
    REQUIRE(xContent.value() == "This is a test comment.More content.");
  }
  SECTION("Create and use Content using make/XRef API.", "[XML][XNode][Content][Make][XRef][API]")
  {
    XNode xNode = XNode::make<Content>("This is a test comment.");
    REQUIRE_FALSE(!isA<Content>(xNode));
    REQUIRE(XRef<Content>(xNode).value()=="This is a test comment.");
    XRef<Content>(xNode).addContent("More content.");
    REQUIRE(XRef<Content>(xNode).value()=="This is a test comment.More content.");
  }
}

