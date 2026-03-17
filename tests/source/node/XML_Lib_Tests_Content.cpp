#include "XML_Lib_Tests.hpp"

TEST_CASE("Create and use Content Node.", "[Node][Content][API]")
{
  SECTION("Create default Content Node.", "[XML][Node][Content][API]") { REQUIRE_NOTHROW(Content("")); }
  SECTION("Create Content Node.", "[XML][Node][Content][API]") { REQUIRE_NOTHROW(Content("This is a test content.")); }
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
    REQUIRE(NRef<Content>(xNode).value() == "This is a test comment.");
    NRef<Content>(xNode).addContent("More content.");
    REQUIRE(NRef<Content>(xNode).value() == "This is a test comment.More content.");
  }
  SECTION("Create Content Node with empty string.", "[XML][Node][Content][Edge]")
  {
    Content content("");
    REQUIRE(content.value().empty());
    REQUIRE(content.getContents().empty());
  }
  SECTION("Create Content Node with large content.", "[XML][Node][Content][Large]")
  {
    std::string large(10000, 'x');
    Content content(large);
    REQUIRE(content.value() == large);
    REQUIRE(content.getContents() == large);
  }
  SECTION("Create Content Node with special characters.", "[XML][Node][Content][Special]")
  {
    std::string special = "<>&\"'\n\t";
    Content content(special);
    REQUIRE(content.value() == special);
    REQUIRE(content.getContents() == special);
  }
  SECTION("Content Node whitespace flag.", "[XML][Node][Content][Whitespace]")
  {
    Content content("   ", true);
    REQUIRE(content.isWhiteSpace());
    content.setIsWhiteSpace(false);
    REQUIRE_FALSE(content.isWhiteSpace());
  }
  SECTION("Content Node addContent edge case.", "[XML][Node][Content][Mutation]")
  {
    Content content("abc");
    content.addContent("");
    REQUIRE(content.value() == "abc");
    content.addContent("123");
    REQUIRE(content.value() == "abc123");
  }
}
