#include "XML_Lib_Tests.hpp"

TEST_CASE("Create and use Declaration XNode.","[XNode][Declaration][API]")
{
  SECTION("Create Declaration XNode.", "[XML][XNode][Declaration][API]")
  {
  REQUIRE_NOTHROW(Declaration("", "", ""));
  }
  SECTION("Create Declaration XNode and fetch content related to it.", "[XML][XNode][Declaration][API]")
  {
    Declaration xDeclaration = Declaration("1.0", "UTF-8", "on");
    REQUIRE(xDeclaration.version() == "1.0");
    REQUIRE(xDeclaration.encoding() == "UTF-8");
    REQUIRE(xDeclaration.standalone() == "on");
  }
  SECTION("Create Declaration XNode and set values.", "[XML][XNode][Declaration][API]")
  {
    Declaration xDeclaration = Declaration("", "", "");
    xDeclaration.setVersion("1.1");
    xDeclaration.setEncoding("UTF-16");
    xDeclaration.setStandalone("off");
    REQUIRE(xDeclaration.version() == "1.1");
    REQUIRE(xDeclaration.encoding() == "UTF-16");
    REQUIRE(xDeclaration.standalone() == "off");
  }
  SECTION("Create and use Declaration using make/XRef API.", "[XML][XNode][Declaration][Make][XRef][API]")
  {
    XNode xNode = XNode::make<Declaration>("1.0", "UTF-8", "on");
    REQUIRE_FALSE(!isA<Declaration>(xNode));
    REQUIRE(XRef<Declaration>(xNode).version() == "1.0");
    REQUIRE(XRef<Declaration>(xNode).encoding() == "UTF-8");
    REQUIRE(XRef<Declaration>(xNode).standalone() == "on");
  }
}


