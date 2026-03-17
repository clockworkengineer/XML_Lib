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
    SECTION("Content Node with Unicode characters", "[XML][Node][Content][Unicode]")
    {
      std::string unicode = "\xF0\x9D\x84\x9E\xF0\x9D\x84\xA2\xF0\x9D\x84\xAB"; // UTF-8 for 𝄞𝄢𝄫
      Content content(unicode);
      REQUIRE(content.value() == unicode);
      content.addContent(unicode);
      REQUIRE(content.value() == unicode + unicode);
    }
    SECTION("Content Node integration with Node API", "[XML][Node][Content][Integration]")
    {
      Node xNode = Node::make<Content>("integration test");
      REQUIRE(isA<Content>(xNode));
      REQUIRE(NRef<Content>(xNode).value() == "integration test");
      NRef<Content>(xNode).addContent("123");
      REQUIRE(NRef<Content>(xNode).value() == "integration test123");
    }
    SECTION("Content Node stringify output", "[XML][Node][Content][Stringify]")
    {
      Node xNode = Node::make<Content>("stringify test");
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
      REQUIRE(result == "stringify test");
    }
}
