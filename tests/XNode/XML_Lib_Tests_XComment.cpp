//
// Unit Tests: XML_Lib_Tests_Comment.cpp
//
// Description: Create and use XComment XNode
//

#include "XML_Lib_Tests.hpp"

using namespace XML_Lib;

TEST_CASE("Create and use Variant", "[XML][XNode][XComment][API]")
{
  XML xml;
  SECTION("Create default variant.", "XML][XNode][XComment][API]")
  {
    REQUIRE_NOTHROW(XComment());
  }
}

