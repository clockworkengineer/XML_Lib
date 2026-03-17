#include "XML_Lib_Tests.hpp"

XMLValue createXMLValue()
{
  return XMLValue("unparsed", "parsed");
}
// Additional tests for EntityReference
TEST_CASE("EntityReference edge cases and integration", "[Node][EntityReference][Edge][Integration]")
{
  SECTION("EntityReference with empty XMLValue", "[XML][Node][EntityReference][Empty]")
  {
    REQUIRE_NOTHROW(EntityReference(XMLValue("", "")));
    auto xEntityReference = EntityReference(XMLValue("", ""));
    REQUIRE(xEntityReference.value().getUnparsed() == "");
    REQUIRE(xEntityReference.value().getParsed() == "");
  }

  SECTION("EntityReference copy/move semantics", "[XML][Node][EntityReference][CopyMove]")
  {
    EntityReference original(createXMLValue());
    EntityReference moved(std::move(original));
    REQUIRE(moved.value().getParsed() == "parsed");
    REQUIRE(moved.value().getUnparsed() == "unparsed");
  }

  SECTION("EntityReference with invalid value throws", "[XML][Node][EntityReference][Error]")
  {
    REQUIRE_NOTHROW(EntityReference(XMLValue("invalid", "")));
    auto xEntityReference = EntityReference(XMLValue("invalid", ""));
    REQUIRE(xEntityReference.value().getUnparsed() == "invalid");
  }

  SECTION("EntityReference integration with XML_EntityMapper", "[XML][Node][EntityReference][EntityMapper]")
  {
    XML_EntityMapper entityMapper;
    entityMapper.setInternal("&foo;", "bar");
    XMLValue val("&foo;");
    auto mapped = entityMapper.map(val);
    REQUIRE(mapped.getParsed() == "bar");
    EntityReference ref(val);
    REQUIRE(ref.value().getUnparsed() == "&foo;");
    REQUIRE(ref.value().getParsed() == "");
  }
}

TEST_CASE("Create and use EntityReference Node.","[Node][EntityReference][API]")
{
  SECTION("Create EntityReference Node.", "[XML][Node][EntityReference][API]")
  {
    REQUIRE_NOTHROW(EntityReference(createXMLValue()));
  }
  SECTION("Create EntityReference Node and fetch value related to it.", "[XML][Node][EntityReference][API]")
  {
    auto xEntityReference = EntityReference(createXMLValue());
    REQUIRE(xEntityReference.value().getParsed() == "parsed");
    REQUIRE(xEntityReference.value().getUnparsed() == "unparsed");
  }
  SECTION("Create and use EntityReference using make/NRef API.", "[XML][Node][EntityReference][Make][NRef][API]")
  {
    Node xNode = Node::make<EntityReference>(createXMLValue());
    REQUIRE_FALSE(!isA<EntityReference>(xNode));
    REQUIRE(NRef<EntityReference>(xNode).value().getParsed() == "parsed");
    REQUIRE(NRef<EntityReference>(xNode).value().getUnparsed() == "unparsed");
  }
}



