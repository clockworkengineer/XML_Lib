#include "XML_Lib_Tests.hpp"

using namespace XML_Lib;

TEST_CASE("Create and use Root Node.","[Node][Root][API]")
{
  SECTION("Create default Root Node.", "[XML][Node][Root][API]")
  {
    REQUIRE_NOTHROW(Root());
  }
  SECTION("Create Root with a given name.", "[XML][Node][Root][API]")
  {
    auto xRoot = Root("test", {}, {});
    REQUIRE( xRoot.name() == "test");
    REQUIRE_FALSE(!xRoot.getAttributes().empty());
    REQUIRE_FALSE(!xRoot.getNameSpaces().empty());
    REQUIRE(xRoot.getContents()=="");
  }
  SECTION("Create and use Root using make/NRef API.", "[XML][Node][Root][Make][NRef][API]")
  {
    std::vector<XMLAttribute> namespaces, attributes;
    auto xNode = Node::make<Root>("test", namespaces, attributes);
    REQUIRE_FALSE(!isA<Root>(xNode));
    REQUIRE(NRef<Root>(xNode).name()=="test");
  }
}
// Additional tests for Root
TEST_CASE("Root edge cases and integration", "[Node][Root][Edge][Integration]")
{
  SECTION("Root copy/move semantics", "[XML][Node][Root][CopyMove]")
  {
    Root original("root");
    Root moved(std::move(original));
    REQUIRE(moved.name() == "root");
    REQUIRE(moved.getNodeType() == Variant::Type::root);
  }

  SECTION("Root with child nodes", "[XML][Node][Root][Children]")
  {
    Root rootNode("root");
    Node child = Node::make<Element>("child");
    std::vector<XMLAttribute> emptyAttrs;
    std::vector<XMLAttribute> emptyNamespaces;
    Node root = Node::make<Root>("root", emptyAttrs, emptyNamespaces);
    root.addChild(child);
    REQUIRE(root.getChildren().size() == 1);
    REQUIRE(isA<Element>(root.getChildren()[0]));
    REQUIRE(NRef<Element>(root.getChildren()[0]).name() == "child");
  }

  SECTION("Root integration with XML parsing", "[XML][Node][Root][XML]")
  {
    XML xml;
    BufferSource source{
      "<?xml version=\"1.0\"?>\n<root attr1='1'><child>data</child></root>\n"
    };
    xml.parse(source);
    REQUIRE_FALSE(!isA<Root>(xml.root()));
    REQUIRE(NRef<Root>(xml.root()).name() == "root");
    REQUIRE(NRef<Root>(xml.root()).getAttributes().size() == 1);
    REQUIRE(NRef<Root>(xml.root()).getChildren().size() == 1);
    REQUIRE(NRef<Element>(xml.root()[0]).name() == "child");
    REQUIRE(NRef<Element>(xml.root()[0]).getContents() == "data");
  }
}


