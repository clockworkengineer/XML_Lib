//
// Unit Tests: XML
//
// Description:
//

#include "XML_Tests.hpp"

using namespace XML_Lib;

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
    REQUIRE(XRef<XDeclaration>(xml.declaration()).version() == "1.0");
    REQUIRE(XRef<XDeclaration>(xml.declaration()).encoding() == "UTF-8");
    REQUIRE(XRef<XDeclaration>(xml.declaration()).standalone() == "no");
    REQUIRE(xml.prolog().getChildren().size() == 4);
    REQUIRE(XRef<XElement>(xml.root()).name() == "contact-info");
  }
  SECTION("Empty root element <AddressBook> ", "[XML][Parse][Root]")
  {
    xmlString =
      "<?xml version=\"1.0\"?>\n"
      "<AddressBook> </AddressBook>\n";
    BufferSource source{ xmlString };
    XML xml;
    xml.parse(source);
    REQUIRE(XRef<XDeclaration>(xml.declaration()).version() == "1.0");
    REQUIRE(XRef<XDeclaration>(xml.declaration()).encoding() == "UTF-8");
    REQUIRE(XRef<XDeclaration>(xml.declaration()).standalone() == "no");
    REQUIRE(xml.prolog().getChildren().size() == 4);
    REQUIRE(XRef<XElement>(xml.root()).name() == "AddressBook");
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
    REQUIRE(XRef<XDeclaration>(xml.declaration()).version() == "1.0");
    REQUIRE(XRef<XDeclaration>(xml.declaration()).encoding() == "UTF-8");
    REQUIRE(XRef<XDeclaration>(xml.declaration()).standalone() == "no");
    REQUIRE(XRef<XElement>(xml.root()).name() == "AddressBook");
    REQUIRE(XRef<XElement>(xml.root()).getChildren().size() == 3);
    REQUIRE(XRef<XElement>(xml.root())[0].name() == "Address");
    REQUIRE(XRef<XElement>(xml.root())[0].getContents() == "    This is some contents    ");
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
    REQUIRE(XRef<XDeclaration>(xml.declaration()).version() == "1.0");
    REQUIRE(XRef<XDeclaration>(xml.declaration()).encoding() == "UTF-8");
    REQUIRE(XRef<XDeclaration>(xml.declaration()).standalone() == "no");
    REQUIRE(XRef<XElement>(xml.root()).name() == "AddressBook");
    REQUIRE(XRef<XElement>(xml.root()).getChildren().size() == 7);
    REQUIRE(XRef<XElement>(xml.root())[0].name() == "Address");
    REQUIRE(XRef<XElement>(xml.root())[0].getContents() == "\n    This is some contents 1   ");
    REQUIRE(XRef<XElement>(xml.root())[1].name() == "Address");
    REQUIRE(XRef<XElement>(xml.root())[1].getContents() == "\n    This is some contents 2   ");
    REQUIRE(XRef<XElement>(xml.root())[2].name() == "Address");
    REQUIRE(XRef<XElement>(xml.root())[2].getContents() == "\n    This is some contents 3   ");
  }
}