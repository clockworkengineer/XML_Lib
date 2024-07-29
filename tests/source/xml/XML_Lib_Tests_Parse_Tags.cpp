#include "XML_Lib_Tests.hpp"

TEST_CASE("Checks for tag names", "[XML][Parse][Tags]")
{
  XML xml;
  SECTION("Allow tag names to with alpha numeric characters and '.','_', '-' ", "[XML][Parse][Tags]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.>\n"
      "</abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.>\n"
    };
    REQUIRE_NOTHROW(xml.parse(source));
  }
  SECTION("Tag names are case sensitive", "[XML][Parse][Tags]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<AddressBook> </addressbook>\n"
    };
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error [Line: 2 Column: 21] Missing closing tag.");
  }
  SECTION("Incorrect closing tag ", "[XML][Parse][Tags]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<AddressBook> </Address>\n"
    };
    REQUIRE_THROWS_WITH(xml.parse(source), "XML Syntax Error [Line: 2 Column: 28] Missing closing tag.");
  }
  SECTION("Self closing tag ", "[XML][Parse][Tags]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<root><AddressBook/></root>\n"
    };
    REQUIRE_NOTHROW(xml.parse(source));
  }
  SECTION("Tag starts with a '.' ", "[XML][Parse][Tags]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?> "
      "<.AddressBook> </.AddressBook>\n"
    };
    REQUIRE_THROWS_WITH(
      xml.parse(source), "XML Syntax Error [Line: 1 Column: 39] Invalid name '.AddressBook' encountered.");
  }
  SECTION("Tag starts with a '-' ", "[XML][Parse][Tags]")
  {
    BufferSource source{ "<?xml version=\"1.0\"?> <-AddressBook> </-AddressBook>\n" };
    REQUIRE_THROWS_WITH(
      xml.parse(source), "XML Syntax Error [Line: 1 Column: 39] Invalid name '-AddressBook' encountered.");
  }
  SECTION("Tag starts with a numeric digit", "[XML][Parse][Tags]")
  {
    BufferSource source{ "<?xml version=\"1.0\"?> <0AddressBook> </0AddressBook>\n" };
    REQUIRE_THROWS_WITH(
      xml.parse(source), "XML Syntax Error [Line: 1 Column: 39] Invalid name '0AddressBook' encountered.");
  }
  SECTION("Tag starts with a xml etc", "[XML][Parse][Tags]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      " <xmlAddressBook> </xmlAddressBook>\n"
    };
    REQUIRE_THROWS_WITH(
      xml.parse(source), "XML Syntax Error [Line: 2 Column: 21] Invalid name 'xmlAddressBook' encountered.");
  }
  SECTION("Tag starts with a XML etc", "[XML][Parse][Tags]")
  {
    BufferSource source{ "<?xml version=\"1.0\"?> <XMLAddressBook> </XMLAddressBook>\n" };
    REQUIRE_THROWS_WITH(
      xml.parse(source), "XML Syntax Error [Line: 1 Column: 41] Invalid name 'XMLAddressBook' encountered.");
  }
  SECTION("Tag starts with a Xml etc", "[XML][Parse][Tags]")
  {
    BufferSource source{ "<?xml version=\"1.0\"?> <XmlAddressBook> </XmlAddressBook>\n" };
    REQUIRE_THROWS_WITH(
      xml.parse(source), "XML Syntax Error [Line: 1 Column: 41] Invalid name 'XmlAddressBook' encountered.");
  }
}
