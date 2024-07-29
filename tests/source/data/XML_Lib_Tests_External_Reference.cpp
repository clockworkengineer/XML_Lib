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
    REQUIRE_THROWS_WITH(externalReference.getSystemID(), "ExternalReference Error: External reference has no system value set." );
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
}