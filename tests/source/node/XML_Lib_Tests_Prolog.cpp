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
// Additional tests for Prolog
TEST_CASE("Prolog edge cases and integration", "[Node][Prolog][Edge][Integration]")
{
  SECTION("Prolog copy/move semantics", "[XML][Node][Prolog][CopyMove]")
  {
    Prolog original;
    Prolog moved(std::move(original));
    REQUIRE(moved.getNodeType() == Variant::Type::prolog);
  }

  SECTION("Prolog with child nodes", "[XML][Node][Prolog][Children]")
  {
    Node prologNode = Node::make<Prolog>();
    Node child = Node::make<Declaration>("1.0", "UTF-8", "yes");
    prologNode.addChild(child);
    REQUIRE(prologNode.getChildren().size() == 1);
    REQUIRE(isA<Declaration>(prologNode.getChildren()[0]));
  }

  SECTION("Prolog integration with XML", "[XML][Node][Prolog][XML]")
  {
    XML xml;
    BufferSource source{
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n<root></root>\n"
    };
    xml.parse(source);
    REQUIRE_FALSE(!isA<Prolog>(xml.prolog()));
    REQUIRE(xml.prolog().getChildren().size() > 0);
  }
}



