#include "XML_Lib_Tests.hpp"

TEST_CASE("Create and use Declaration Node.", "[Node][Declaration][API]")
{
  SECTION("Create Declaration Node.", "[XML][Node][Declaration][API]") { REQUIRE_NOTHROW(Declaration("", "", "")); }
  SECTION("Create Declaration Node and fetch content related to it.", "[XML][Node][Declaration][API]")
  {
    auto xDeclaration = Declaration("1.0", "UTF-8", "on");
    REQUIRE(xDeclaration.version() == "1.0");
    REQUIRE(xDeclaration.encoding() == "UTF-8");
    REQUIRE(xDeclaration.standalone() == "on");
  }
  SECTION("Create Declaration Node and set values.", "[XML][Node][Declaration][API]")
  {
    auto xDeclaration = Declaration("", "", "");
    xDeclaration.setVersion("1.1");
    xDeclaration.setEncoding("UTF-16");
    xDeclaration.setStandalone("off");
    REQUIRE(xDeclaration.version() == "1.1");
    REQUIRE(xDeclaration.encoding() == "UTF-16");
    REQUIRE(xDeclaration.standalone() == "off");
  }
  SECTION("Create and use Declaration using make/NRef API.", "[XML][Node][Declaration][Make][NRef][API]")
  {
    Node xNode = Node::make<Declaration>("1.0", "UTF-8", "on");
    REQUIRE_FALSE(!isA<Declaration>(xNode));
    REQUIRE(NRef<Declaration>(xNode).version() == "1.0");
    REQUIRE(NRef<Declaration>(xNode).encoding() == "UTF-8");
    REQUIRE(NRef<Declaration>(xNode).standalone() == "on");
  }
  SECTION("Create Declaration Node with empty values.", "[XML][Node][Declaration][Edge]")
  {
    Declaration decl{ "", "", "" };
    REQUIRE(decl.version().empty());
    REQUIRE(decl.encoding().empty());
    REQUIRE(decl.standalone().empty());
  }
  SECTION("Create Declaration Node with large values.", "[XML][Node][Declaration][Large]")
  {
    std::string large(10000, 'x');
    Declaration decl{ large, large, large };
    REQUIRE(decl.version() == large);
    REQUIRE(decl.encoding() == large);
    REQUIRE(decl.standalone() == large);
  }
  SECTION("Create Declaration Node with special characters.", "[XML][Node][Declaration][Special]")
  {
    std::string special = "<>&\"'\n\t";
    Declaration decl{ special, special, special };
    REQUIRE(decl.version() == special);
    REQUIRE(decl.encoding() == special);
    REQUIRE(decl.standalone() == special);
  }
  SECTION("Declaration Node set values edge case.", "[XML][Node][Declaration][Mutation]")
  {
    Declaration decl{ "a", "b", "c" };
    decl.setVersion("");
    decl.setEncoding("");
    decl.setStandalone("");
    REQUIRE(decl.version().empty());
    REQUIRE(decl.encoding().empty());
    REQUIRE(decl.standalone().empty());
    decl.setVersion("1.2");
    decl.setEncoding("UTF-32");
    decl.setStandalone("maybe");
    REQUIRE(decl.version() == "1.2");
    REQUIRE(decl.encoding() == "UTF-32");
    REQUIRE(decl.standalone() == "maybe");
  }
    SECTION("Declaration Node with Unicode characters", "[XML][Node][Declaration][Unicode]")
    {
      std::string unicode = "\xF0\x9D\x84\x9E\xF0\x9D\x84\xA2\xF0\x9D\x84\xAB"; // UTF-8 for 𝄞𝄢𝄫
      Declaration decl{ unicode, unicode, unicode };
      REQUIRE(decl.version() == unicode);
      REQUIRE(decl.encoding() == unicode);
      REQUIRE(decl.standalone() == unicode);
      decl.setVersion("new");
      decl.setEncoding("new");
      decl.setStandalone("new");
      REQUIRE(decl.version() == "new");
      REQUIRE(decl.encoding() == "new");
      REQUIRE(decl.standalone() == "new");
    }
    SECTION("Declaration Node integration with Node API", "[XML][Node][Declaration][Integration]")
    {
      Node xNode = Node::make<Declaration>("integration test", "UTF-8", "yes");
      REQUIRE(isA<Declaration>(xNode));
      REQUIRE(NRef<Declaration>(xNode).version() == "integration test");
      REQUIRE(NRef<Declaration>(xNode).encoding() == "UTF-8");
      REQUIRE(NRef<Declaration>(xNode).standalone() == "yes");
      NRef<Declaration>(xNode).setVersion("1.3");
      NRef<Declaration>(xNode).setEncoding("UTF-16");
      NRef<Declaration>(xNode).setStandalone("no");
      REQUIRE(NRef<Declaration>(xNode).version() == "1.3");
      REQUIRE(NRef<Declaration>(xNode).encoding() == "UTF-16");
      REQUIRE(NRef<Declaration>(xNode).standalone() == "no");
    }
    SECTION("Declaration Node stringify output", "[XML][Node][Declaration][Stringify]")
    {
      Node xNode = Node::make<Declaration>("1.0", "UTF-8", "yes");
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
      REQUIRE(result == "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>");
    }
}
