#include "XML_Lib_Tests.hpp"

TEST_CASE("Create and use Prolog Node.","[Node][Prolog][API]")
{
  SECTION("Create default Prolog Node.", "[XML][Node][Prolog][API]")
  {
    REQUIRE_NOTHROW(Prolog());
  }
  SECTION("Create and use Prolog using make/NRef API.", "[XML][Node][Prolog][Make][NRef][API]")
  {
    const Node xNode = Node::make<Prolog>();
    REQUIRE_FALSE(!isA<Prolog>(xNode));
  }
}



