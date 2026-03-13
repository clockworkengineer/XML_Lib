#include "XML_Lib_Tests.hpp"

TEST_CASE("XML attribute usage tests cases.", "[XML][Value]")
{
  SECTION("Create an empty attribute does not throw exception.", "[XML][Attribute][Create]")
  {
    REQUIRE_NOTHROW(XMLAttribute("", XMLValue("")));
  }
  SECTION("Create an test attribute and check its value.", "[XML][Attribute][Create]")
  {
    auto attribute = XMLAttribute("test", XMLValue("unparsed", "parsed"));
    REQUIRE(attribute.getName() == "test");
    REQUIRE(attribute.getUnparsed() == "unparsed");
    REQUIRE(attribute.getParsed() == "parsed");
  }
  SECTION("Create an attribute entity reference value and check its type.", "[XML][Attribute][Create]")
  {
    auto attribute = XMLAttribute("test", XMLValue("&test;", "parsed"));
    REQUIRE_FALSE(!attribute.isEntityReference());
    REQUIRE_FALSE(attribute.isCharacterReference());
    REQUIRE_FALSE(!attribute.isReference());
    REQUIRE(attribute.getName() == "test");
    REQUIRE(attribute.getUnparsed() == "&test;");
    REQUIRE(attribute.getParsed() == "parsed");
  }
  SECTION("Create an attribute character reference value and check its type.", "[XML][Attribute][Create]")
  {
    auto attribute = XMLAttribute("test", XMLValue("&#0180;", "parsed"));
    REQUIRE_FALSE(attribute.isEntityReference());
    REQUIRE_FALSE(!attribute.isCharacterReference());
    REQUIRE_FALSE(!attribute.isReference());
    REQUIRE(attribute.getName() == "test");
    REQUIRE(attribute.getUnparsed() == "&#0180;");
    REQUIRE(attribute.getParsed() == "parsed");
  }
  SECTION("Create an empty vector of attributes and try to search.", "[XML][Attribute][API]")
  {
    std::vector<XMLAttribute> attributes;
    REQUIRE_FALSE(XMLAttribute::contains(attributes, "test"));
  }
  SECTION("Create an  vector of three attributes and perform a successful search.", "[XML][Attribute][API]")
  {
    std::vector<XMLAttribute> attributes;
    attributes.emplace_back("test1", XMLValue("&test1;", "parsed"));
    attributes.emplace_back("test2", XMLValue("&test2;", "parsed"));
    attributes.emplace_back("test3", XMLValue("&test3;", "parsed"));
    REQUIRE_FALSE(!XMLAttribute::contains(attributes, "test3"));
  }
  SECTION("Create an  vector of three attributes and perform a failed search.", "[XML][Attribute][API]")
  {
    std::vector<XMLAttribute> attributes;
    attributes.emplace_back("test1", XMLValue("&test1;", "parsed"));
    attributes.emplace_back("test2", XMLValue("&test2;", "parsed"));
    attributes.emplace_back("test3", XMLValue("&test3;", "parsed"));
    REQUIRE_FALSE(XMLAttribute::contains(attributes, "test4"));
  }
  SECTION("Copy and move constructors/assignment.", "[XML][Attribute][CopyMove]")
  {
    XMLAttribute original("copy", XMLValue("unparsed", "parsed"));
    XMLAttribute copyConstructed(original);
    REQUIRE(copyConstructed.getName() == "copy");
    REQUIRE(copyConstructed.getUnparsed() == "unparsed");
    REQUIRE(copyConstructed.getParsed() == "parsed");

    XMLAttribute moveConstructed(std::move(original));
    REQUIRE(moveConstructed.getName() == "copy");
    REQUIRE(moveConstructed.getUnparsed() == "unparsed");
    REQUIRE(moveConstructed.getParsed() == "parsed");

    XMLAttribute assigned("a", XMLValue("b", "c"));
    assigned = copyConstructed;
    REQUIRE(assigned.getName() == "copy");
    REQUIRE(assigned.getUnparsed() == "unparsed");
    REQUIRE(assigned.getParsed() == "parsed");

    XMLAttribute moveAssigned("x", XMLValue("y", "z"));
    moveAssigned = std::move(copyConstructed);
    REQUIRE(moveAssigned.getName() == "copy");
    REQUIRE(moveAssigned.getUnparsed() == "unparsed");
    REQUIRE(moveAssigned.getParsed() == "parsed");
  }

  SECTION("Assignment from XMLValue.", "[XML][Attribute][AssignValue]")
  {
    XMLAttribute attr("assign", XMLValue("a", "b"));
    XMLValue val("newUnparsed", "newParsed");
    attr = val;
    REQUIRE(attr.getUnparsed() == "newUnparsed");
    REQUIRE(attr.getParsed() == "newParsed");
    REQUIRE(attr.getName() == "assign");
  }

  SECTION("Error handling: find non-existent attribute throws.", "[XML][Attribute][Error]")
  {
    std::vector<XMLAttribute> attributes;
    attributes.emplace_back("a", XMLValue("1", "2"));
    bool threw = false;
    try {
      XMLAttribute::find(attributes, "notfound");
    } catch (const XMLAttribute::Error &e) {
      threw = true;
    }
    REQUIRE(threw);
  }

  SECTION("Edge case: attribute with empty name and value.", "[XML][Attribute][Edge]")
  {
    XMLAttribute attr("", XMLValue("", ""));
    REQUIRE(attr.getName().empty());
    REQUIRE(attr.getUnparsed().empty());
    REQUIRE(attr.getParsed().empty());
  }
}