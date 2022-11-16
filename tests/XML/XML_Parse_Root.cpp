//
// Unit Tests: XML
//
// Description:
//
// ================
// Test definitions
// =================
#include "XML_Tests.hpp"
// =======================
// JSON class namespace
// =======================
using namespace XML_Lib;
// ==========
// Test cases
// ==========
TEST_CASE("Parse declaration, root element and check parsed information ", "[XML][Parse][Root]")
{
  std::string xmlString;
  SECTION("Empty root element <contact-info> ", "[XML][Parse]")
  {
    xmlString =
      "<?xml version=\"1.0\"?>\n"
      "<contact-info></contact-info>\n";
    BufferSource source{ xmlString };
    XML xml;
    xml.parse(source);
    REQUIRE(XNodeRef<XNodeDeclaration>(xml.declaration()).version() == "1.0");
    REQUIRE(XNodeRef<XNodeDeclaration>(xml.declaration()).encoding() == "UTF-8");
    REQUIRE(XNodeRef<XNodeDeclaration>(xml.declaration()).standalone() == "no");
    REQUIRE(xml.prolog().children.size() == 4);
    REQUIRE(XNodeRef<XNodeElement>(xml.root()).name() == "contact-info");
  }
  SECTION("Empty root element <AddressBook> ", "[XML][Parse][Root]")
  {
    xmlString =
      "<?xml version=\"1.0\"?>\n"
      "<AddressBook> </AddressBook>\n";
    BufferSource source{ xmlString };
    XML xml;
    xml.parse(source);
    REQUIRE(XNodeRef<XNodeDeclaration>(xml.declaration()).version() == "1.0");
    REQUIRE(XNodeRef<XNodeDeclaration>(xml.declaration()).encoding() == "UTF-8");
    REQUIRE(XNodeRef<XNodeDeclaration>(xml.declaration()).standalone() == "no");
    REQUIRE(xml.prolog().children.size() == 4);
    REQUIRE(XNodeRef<XNodeElement>(xml.root()).name() == "AddressBook");
  }
  SECTION("Root element <AddressBook> and one child <Address> with contents ", "[XML][Parse][Root]")
  {
    xmlString =
      "<?xml version=\"1.0\"?>\n"
      "<AddressBook>\n"
      "<Address>    This is some contents    </Address>\n"
      "</AddressBook>\n";
    BufferSource source{ xmlString };
    XML xml;
    xml.parse(source);
    REQUIRE(XNodeRef<XNodeDeclaration>(xml.declaration()).version() == "1.0");
    REQUIRE(XNodeRef<XNodeDeclaration>(xml.declaration()).encoding() == "UTF-8");
    REQUIRE(XNodeRef<XNodeDeclaration>(xml.declaration()).standalone() == "no");
    REQUIRE(XNodeRef<XNodeElement>(xml.root()).name() == "AddressBook");
    REQUIRE(XNodeRef<XNodeElement>(xml.root()).children.size() == 3);
    REQUIRE(XNodeRef<XNodeElement>(xml.root())[0].name() == "Address");
    REQUIRE(XNodeRef<XNodeElement>(xml.root())[0].getContents() == "    This is some contents    ");
  }
  SECTION("Root element <AddressBook> with multiple sibling <Address> elements and contents ", "[XML][Parse][Root]")
  {
    xmlString =
      "<?xml version=\"1.0\"?>\n"
      "<AddressBook>\n"
      "<Address>\n"
      "    This is some contents 1   "
      "</Address>\n"
      "<Address>\n"
      "    This is some contents 2   "
      "</Address>\n"
      "<Address>\n"
      "    This is some contents 3   "
      "</Address>\n"
      "</AddressBook>\n";
    BufferSource source{ xmlString };
    XML xml;
    xml.parse(source);
    REQUIRE(XNodeRef<XNodeDeclaration>(xml.declaration()).version() == "1.0");
    REQUIRE(XNodeRef<XNodeDeclaration>(xml.declaration()).encoding() == "UTF-8");
    REQUIRE(XNodeRef<XNodeDeclaration>(xml.declaration()).standalone() == "no");
    REQUIRE(XNodeRef<XNodeElement>(xml.root()).name() == "AddressBook");
    REQUIRE(XNodeRef<XNodeElement>(xml.root()).children.size() == 7);
    REQUIRE(XNodeRef<XNodeElement>(xml.root())[0].name() == "Address");
    REQUIRE(XNodeRef<XNodeElement>(xml.root())[0].getContents() == "\n    This is some contents 1   ");
    REQUIRE(XNodeRef<XNodeElement>(xml.root())[1].name() == "Address");
    REQUIRE(XNodeRef<XNodeElement>(xml.root())[1].getContents() == "\n    This is some contents 2   ");
    REQUIRE(XNodeRef<XNodeElement>(xml.root())[2].name() == "Address");
    REQUIRE(XNodeRef<XNodeElement>(xml.root())[2].getContents() == "\n    This is some contents 3   ");
  }
}