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
}
