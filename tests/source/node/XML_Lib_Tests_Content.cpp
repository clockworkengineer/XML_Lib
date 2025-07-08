#include "XML_Lib_Tests.hpp"

TEST_CASE("Create and use Content Node.","[Node][Content][API]")
{
  SECTION("Create default Content Node.", "[XML][Node][Content][API]")
  {
    REQUIRE_NOTHROW(Content(""));
  }
  SECTION("Create Content Node.", "[XML][Node][Content][API]")
  {
    REQUIRE_NOTHROW(Content("This is a test content."));
  }
  SECTION("Create Content Node and fetch content related to it.", "[XML][Node][Content][API]")
  {
    auto xContent = Content("This is a test comment.");
    REQUIRE(xContent.value() == "This is a test comment.");
    xContent.addContent("More content.");
    REQUIRE(xContent.value() == "This is a test comment.More content.");
  }
  SECTION("Create and use Content using make/NRef API.", "[XML][Node][Content][Make][NRef][API]")
  {
    Node xNode = Node::make<Content>("This is a test comment.");
    REQUIRE_FALSE(!isA<Content>(xNode));
    REQUIRE(NRef<Content>(xNode).value()=="This is a test comment.");
    NRef<Content>(xNode).addContent("More content.");
    REQUIRE(NRef<Content>(xNode).value()=="This is a test comment.More content.");
  }
}

