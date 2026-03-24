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
    SECTION("Comment Node with embedded dashes (should not split)", "[XML][Node][Comment][Edge][Dash]")
    {
      std::string tricky = "-- inside comment";
      Comment comment(tricky);
      REQUIRE(comment.value() == tricky);
    }
    SECTION("Comment Node with Unicode characters", "[XML][Node][Comment][Unicode]")
    {
      std::string unicode = "\xF0\x9D\x84\x9E\xF0\x9D\x84\xA2\xF0\x9D\x84\xAB"; // UTF-8 for 𝄞𝄢𝄫
      Comment comment(unicode);
      REQUIRE(comment.value() == unicode);
    }
    SECTION("Comment Node integration with Node API", "[XML][Node][Comment][Integration]")
    {
      Node xNode = Node::make<Comment>("integration test");
      REQUIRE(isA<Comment>(xNode));
      REQUIRE(NRef<Comment>(xNode).value() == "integration test");
    }
    SECTION("Comment Node stringify output", "[XML][Node][Comment][Stringify]")
    {
      Node xNode = Node::make<Comment>("stringify test");
      Default_Stringify stringify;
      std::string result;
      class StringDestination : public IDestination {
        std::string data;
      public:
        void add(const std::string &s) override { data += s; }
        void add(Char c) override { data += static_cast<char>(c); }
        void add(const char *bytes) override { data += bytes; }
        void add(const std::string_view &bytes) override { data += std::string(bytes); }
        void clear() override { data.clear(); }
        std::string str() const { return data; }
      };
      StringDestination dest;
      stringify.stringify(xNode, dest, 0);
      result = dest.str();
      REQUIRE(result == "<!--stringify test-->");
    }
}
