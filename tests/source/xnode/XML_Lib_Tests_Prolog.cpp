#include "XML_Lib_Tests.hpp"

TEST_CASE("Create and use Prolog XNode.","[XNode][Prolog][API]")
{
  SECTION("Create default Prolog XNode.", "[XML][XNode][Prolog][API]")
  {
    REQUIRE_NOTHROW(Prolog());
  }
  SECTION("Create and use Prolog using make/XRef API.", "[XML][XNode][Prolog][Make][XRef][API]")
  {
    XNode xNode = XNode::make<Prolog>();
    REQUIRE_FALSE(!isA<Prolog>(xNode));
  }
}



