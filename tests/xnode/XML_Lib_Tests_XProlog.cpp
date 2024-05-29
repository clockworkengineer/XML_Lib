//
// Unit Tests: XML_Lib_Tests_XProlog.cpp
//
// Description: Create and use XProlog XNode
//

#include "XML_Lib_Tests.hpp"

using namespace XML_Lib;

TEST_CASE("Create and use XProlog XNode.","[XNode][XProlog][API]")
{
  SECTION("Create default XProlog XNode.", "[XML][XNode][XProlog][API]")
  {
    REQUIRE_NOTHROW(XProlog());
  }
  SECTION("Create and use XProlog using make/XRef API.", "[XML][XNode][XProlog][Make][XRef][API]")
  {
    XNode xNode = XNode::make<XProlog>();
    REQUIRE_FALSE(!xNode.isProlog());
  }
}



