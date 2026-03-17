#include "XML_Lib_Tests.hpp"

TEST_CASE("XML external reference usage tests cases.", "[XML][ExternalReference]")
{
  SECTION("Create an empty external reference does not throw exception.", "[XML][ExternalReference][Create]")
  {
    REQUIRE_NOTHROW(XMLExternalReference(""));
  }
  SECTION("Try to access an empty reference (base).", "[XML][ExternalReference][Create]")
  {
    auto externalReference = XMLExternalReference("");
    REQUIRE_THROWS_WITH(externalReference.getType(), "ExternalReference Error: External reference type not set.");
    REQUIRE_FALSE(externalReference.isSystem());
    REQUIRE_FALSE(externalReference.isPublic());
    REQUIRE_THROWS_WITH(externalReference.getPublicID(), "ExternalReference Error: External reference is not public.");
    REQUIRE_THROWS_WITH(
      externalReference.getSystemID(), "ExternalReference Error: External reference has no system value set.");
  }
  SECTION("Create a system external reference", "[XML][ExternalReference][Create]")
  {
    auto externalReference =
      XMLExternalReference(XMLExternalReference::kSystemID, "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd");
    REQUIRE_FALSE(!externalReference.isSystem());
    REQUIRE_FALSE(externalReference.isPublic());
    REQUIRE(externalReference.getSystemID() == "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd");
  }
  SECTION("Create a public external reference", "[XML][ExternalReference][Create]")
  {
    auto externalReference = XMLExternalReference(XMLExternalReference::kPublicID,
      "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd",
      "-//W3C//DTD XHTML 1.0 Transitional//EN");
    REQUIRE_FALSE(externalReference.isSystem());
    REQUIRE_FALSE(!externalReference.isPublic());
    REQUIRE(externalReference.getPublicID() == "-//W3C//DTD XHTML 1.0 Transitional//EN");
    REQUIRE(externalReference.getSystemID() == "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd");
  }
  SECTION("Create a system external reference and try to access public value.", "[XML][ExternalReference][Create]")
  {
    auto externalReference =
      XMLExternalReference(XMLExternalReference::kSystemID, "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd");
    REQUIRE_FALSE(!externalReference.isSystem());
    REQUIRE_FALSE(externalReference.isPublic());
    REQUIRE_THROWS_WITH(externalReference.getPublicID(), "ExternalReference Error: External reference is not public.");
  }
  SECTION("Copy and move constructors/assignment.", "[XML][ExternalReference][CopyMove]")
  {
    XMLExternalReference original(XMLExternalReference::kPublicID, "sys", "pub");
    XMLExternalReference copyConstructed(original);
    REQUIRE(copyConstructed.isPublic());
    REQUIRE(copyConstructed.getSystemID() == "sys");
    REQUIRE(copyConstructed.getPublicID() == "pub");

    XMLExternalReference moveConstructed(std::move(original));
    REQUIRE(moveConstructed.isPublic());
    REQUIRE(moveConstructed.getSystemID() == "sys");
    REQUIRE(moveConstructed.getPublicID() == "pub");

    XMLExternalReference assigned(XMLExternalReference::kSystemID, "a");
    assigned = copyConstructed;
    REQUIRE(assigned.isPublic());
    REQUIRE(assigned.getSystemID() == "sys");
    REQUIRE(assigned.getPublicID() == "pub");

    XMLExternalReference moveAssigned(XMLExternalReference::kSystemID, "b");
    moveAssigned = std::move(copyConstructed);
    REQUIRE(moveAssigned.isPublic());
    REQUIRE(moveAssigned.getSystemID() == "sys");
    REQUIRE(moveAssigned.getPublicID() == "pub");
  }

  SECTION("Error handling: getType() on default constructed throws.", "[XML][ExternalReference][Error]")
  {
    bool threw = false;
    try {
      XMLExternalReference ref(XMLExternalReference::kSystemID, "");
      ref = XMLExternalReference("", "");
      auto type = ref.getType();
    } catch (const XMLExternalReference::Error &e) {
      threw = true;
    }
    REQUIRE(threw);
  }

  SECTION("Edge case: publicID with empty system/public.", "[XML][ExternalReference][Edge]")
  {
    XMLExternalReference ref(XMLExternalReference::kPublicID, "", "");
    REQUIRE(ref.isPublic());
    REQUIRE(ref.getSystemID().empty());
    REQUIRE(ref.getPublicID().empty());
  }
}