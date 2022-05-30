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
using namespace XMLLib;
// ==========
// Test cases
// ==========
TEST_CASE("Parse declaration, root element and check parsed information ", "[XML][Parse][Root]")
{
  std::string xmlString;
  SECTION("Empty root element <contact-info> ", "[XML][Parse]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<contact-info></contact-info>\n";
    BufferSource source { xmlString };
    XML xml;
    xml.parse(source);
    REQUIRE(xml.declaration().version == "1.0");
    REQUIRE(xml.declaration().encoding == "UTF-8");
    REQUIRE(xml.declaration().standalone == "no");
    REQUIRE(xml.prolog().children.size() == 4);
    REQUIRE(xml.root().elementName == "contact-info");
  }
  SECTION("Empty root element <AddressBook> ", "[XML][Parse][Root]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<AddressBook> </AddressBook>\n";
    BufferSource source { xmlString };
    XML xml;
    xml.parse(source);
    REQUIRE(xml.declaration().version == "1.0");
    REQUIRE(xml.declaration().encoding == "UTF-8");
    REQUIRE(xml.declaration().standalone == "no");
    REQUIRE(xml.prolog().children.size() == 4);
    REQUIRE(xml.root().elementName == "AddressBook");
  }
  SECTION("Root element <AddressBook> and one child <Address> with contents ", "[XML][Parse][Root]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
                "<AddressBook>\n"
                "<Address>    This is some contents    </Address>\n"
                "</AddressBook>\n";
    BufferSource source { xmlString };
    XML xml;
    xml.parse(source);
    REQUIRE(xml.declaration().version == "1.0");
    REQUIRE(xml.declaration().encoding == "UTF-8");
    REQUIRE(xml.declaration().standalone == "no");
    REQUIRE(xml.root().elementName == "AddressBook");
    REQUIRE(xml.root().children.size() == 3);
    REQUIRE(xml.root()[0].elementName == "Address");
    REQUIRE(xml.root()[0].getContents() == "    This is some contents    ");
  }
  SECTION("Root element <AddressBook> with multiple sibling <Address> elements and contents ", "[XML][Parse][Root]")
  {
    xmlString = "<?xml version=\"1.0\"?>\n"
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
    BufferSource source { xmlString };
    XML xml;
    xml.parse(source);
    REQUIRE(xml.declaration().version == "1.0");
    REQUIRE(xml.declaration().encoding == "UTF-8");
    REQUIRE(xml.declaration().standalone == "no");
    REQUIRE(xml.root().elementName == "AddressBook");
    REQUIRE(xml.root().children.size() == 7);
    REQUIRE(xml.root()[0].elementName == "Address");
    REQUIRE(xml.root()[0].getContents() == "\n    This is some contents 1   ");
    REQUIRE(xml.root()[1].elementName == "Address");
    REQUIRE(xml.root()[1].getContents() == "\n    This is some contents 2   ");
    REQUIRE(xml.root()[2].elementName == "Address");
    REQUIRE(xml.root()[2].getContents() == "\n    This is some contents 3   ");
  }
}