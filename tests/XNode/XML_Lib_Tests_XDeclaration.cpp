//
// Unit Tests: XML_Lib_Tests_XDeclaration.cpp
//
// Description: Create and use XDeclaration XNode
//

#include "XML_Lib_Tests.hpp"

using namespace XML_Lib;

TEST_CASE("Create and use XDeclaration XNode.","[XNode][XDeclaration][API]")
{
  SECTION("Create XDeclaration XNode.", "[XML][XNode][XDeclaration][API]")
  {
  REQUIRE_NOTHROW(XDeclaration("", "", ""));
  }
  SECTION("Create XDeclaration XNode and fetch content related to it.", "[XML][XNode][XDeclaration][API]")
  {
    XDeclaration xDeclaration = XDeclaration("1.0", "UTF-8", "on");
    REQUIRE(xDeclaration.version() == "1.0");
    REQUIRE(xDeclaration.encoding() == "UTF-8");
    REQUIRE(xDeclaration.standalone() == "on");
  }
  SECTION("Create XDeclaration XNode and set values.", "[XML][XNode][XDeclaration][API]")
  {
    XDeclaration xDeclaration = XDeclaration("", "", "");
    xDeclaration.setVersion("1.1");
    xDeclaration.setEncoding("UTF-16");
    xDeclaration.setStandalone("off");
    REQUIRE(xDeclaration.version() == "1.1");
    REQUIRE(xDeclaration.encoding() == "UTF-16");
    REQUIRE(xDeclaration.standalone() == "off");
  }
  SECTION("Create and use XDeclaration using make/XRef API.", "[XML][XNode][XDeclaration][Make][XRef][API]")
  {
    XNode xNode = XNode::make<XDeclaration>("1.0", "UTF-8", "on");
    REQUIRE_FALSE(!xNode.isDeclaration());
    REQUIRE(XRef<XDeclaration>(xNode).version() == "1.0");
    REQUIRE(XRef<XDeclaration>(xNode).encoding() == "UTF-8");
    REQUIRE(XRef<XDeclaration>(xNode).standalone() == "on");
  }
}


