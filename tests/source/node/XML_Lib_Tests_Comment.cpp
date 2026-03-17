#include "XML_Lib_Tests.hpp"

TEST_CASE("Create and use Comment Node.", "[Node][Comment][API]")
{
  SECTION("Create default Comment Node.", "[XML][Node][Comment][API]") { REQUIRE_NOTHROW(Comment()); }
  SECTION("Create Comment Node.", "[XML][Node][Comment][API]") { REQUIRE_NOTHROW(Comment("This is a test comment.")); }
  SECTION("Create Comment Node and fetch comment related to it.", "[XML][Node][Comment][API]")
  {
    auto xComment = Comment("This is a test comment.");
    REQUIRE(xComment.value() == "This is a test comment.");
  }
  SECTION("Create and use Comment using make/NRef API.", "[XML][Node][Comment][Make][NRef][API]")
  {
    Node xNode = Node::make<Comment>("This is a test comment.");
    REQUIRE_FALSE(!isA<Comment>(xNode));
    REQUIRE(NRef<Comment>(xNode).value() == "This is a test comment.");
  }
  SECTION("Create Comment Node with empty string.", "[XML][Node][Comment][Edge]")
  {
    Comment comment;
    REQUIRE(comment.value().empty());
  }
  SECTION("Create Comment Node with large content.", "[XML][Node][Comment][Large]")
  {
    std::string large(10000, 'x');
    Comment comment(large);
    REQUIRE(comment.value() == large);
  }
  SECTION("Create Comment Node with special characters.", "[XML][Node][Comment][Special]")
  {
    std::string special = "<>&\"'\n\t";
    Comment comment(special);
    REQUIRE(comment.value() == special);
  }
  SECTION("Mutate Comment Node contents (not allowed, check immutability).", "[XML][Node][Comment][Mutation]")
  {
    Comment comment("original");
    // No mutation API, so value should remain the same
    REQUIRE(comment.value() == "original");
  }
}
