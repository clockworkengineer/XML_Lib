#include "XML_Lib_Tests.hpp"

TEST_CASE("XML value usage tests cases.", "[XML][Value]")
{
  SECTION("Create an empty value does not throw exception.", "[XML][Value][Create]") { REQUIRE_NOTHROW(XMLValue("")); }
  SECTION("Create an empty value and check its type.", "[XML][Value][Create]")
  {
    XMLValue value("");
    REQUIRE_FALSE(value.isEntityReference());
    REQUIRE_FALSE(value.isCharacterReference());
    REQUIRE_FALSE(value.isReference());
  }
  SECTION("Create an entity reference value and check its type.", "[XML][Value][Create]")
  {
    XMLValue value("&test;");
    REQUIRE_FALSE(!value.isEntityReference());
    REQUIRE_FALSE(value.isCharacterReference());
    REQUIRE_FALSE(!value.isReference());
  }
  SECTION("Create an character reference value and check its type.", "[XML][Value][Create]")
  {
    XMLValue value("&#0180;");
    REQUIRE_FALSE(value.isEntityReference());
    REQUIRE_FALSE(!value.isCharacterReference());
    REQUIRE_FALSE(!value.isReference());
  }
  SECTION("Create a value with parsed and unparsed data.", "[XML][Value][Create]")
  {
    XMLValue value("unparsed", "parsed");
    REQUIRE_FALSE(value.isEntityReference());
    REQUIRE_FALSE(value.isCharacterReference());
    REQUIRE_FALSE(value.isReference());
    REQUIRE(value.getUnparsed() == "unparsed");
    REQUIRE(value.getParsed() == "parsed");
  }
  SECTION("Copy and move constructors/assignment.", "[XML][Value][CopyMove]")
  {
    XMLValue original("unparsed", "parsed", '\'');
    XMLValue copyConstructed(original);
    REQUIRE(copyConstructed.getUnparsed() == "unparsed");
    REQUIRE(copyConstructed.getParsed() == "parsed");
    REQUIRE(copyConstructed.getQuote() == '\'');

    XMLValue moveConstructed(std::move(original));
    REQUIRE(moveConstructed.getUnparsed() == "unparsed");
    REQUIRE(moveConstructed.getParsed() == "parsed");
    REQUIRE(moveConstructed.getQuote() == '\'');

    XMLValue assigned("a", "b", '"');
    assigned = copyConstructed;
    REQUIRE(assigned.getUnparsed() == "unparsed");
    REQUIRE(assigned.getParsed() == "parsed");
    REQUIRE(assigned.getQuote() == '\'');

    XMLValue moveAssigned("x", "y", '"');
    moveAssigned = std::move(copyConstructed);
    REQUIRE(moveAssigned.getUnparsed() == "unparsed");
    REQUIRE(moveAssigned.getParsed() == "parsed");
    REQUIRE(moveAssigned.getQuote() == '\'');
  }

  SECTION("Quote handling.", "[XML][Value][Quote]")
  {
    XMLValue value("a", "b", '\'');
    REQUIRE(value.getQuote() == '\'');
    XMLValue value2("a", "b", '"');
    REQUIRE(value2.getQuote() == '"');
  }

  SECTION("Edge case: empty unparsed and parsed.", "[XML][Value][Edge]")
  {
    XMLValue value("", "");
    REQUIRE(value.getUnparsed().empty());
    REQUIRE(value.getParsed().empty());
    REQUIRE_FALSE(value.isReference());
  }
}