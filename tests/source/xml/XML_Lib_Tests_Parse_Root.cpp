#include "XML_Lib_Tests.hpp"

TEST_CASE("Parse declaration, root element and check parsed information ", "[XML][Parse][Root]")
{
  XML xml;
  SECTION("Empty root element <contact-info> ", "[XML][Parse]")
  {

    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<contact-info></contact-info>\n"
    };
    xml.parse(source);
    auto &xDeclaration = XRef<Declaration>(xml.declaration());
    auto &xRoot = XRef<Element>(xml.root());
    REQUIRE(xDeclaration.version() == "1.0");
    REQUIRE(xDeclaration.encoding() == "UTF-8");
    REQUIRE(xDeclaration.standalone() == "no");
    REQUIRE(xml.prolog().getChildren().size() == 4);
    REQUIRE(xRoot.name() == "contact-info");
  }
  SECTION("Empty root element <AddressBook> ", "[XML][Parse][Root]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<AddressBook></AddressBook>\n"
    };
    xml.parse(source);
    auto &xDeclaration = XRef<Declaration>(xml.declaration());
    auto &xRoot = XRef<Element>(xml.root());
    REQUIRE(xDeclaration.version() == "1.0");
    REQUIRE(xDeclaration.encoding() == "UTF-8");
    REQUIRE(xDeclaration.standalone() == "no");
    REQUIRE(xml.prolog().getChildren().size() == 4);
    REQUIRE(xRoot.name() == "AddressBook");
  }
  SECTION("Root element <AddressBook> and one child <Address> with contents ", "[XML][Parse][Root]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<AddressBook>\n"
      "<Address>    This is some contents    </Address>\n"
      "</AddressBook>\n"
    };
    xml.parse(source);
    auto &xDeclaration = XRef<Declaration>(xml.declaration());
    auto &xRoot = XRef<Element>(xml.root());
    REQUIRE(xDeclaration.version() == "1.0");
    REQUIRE(xDeclaration.encoding() == "UTF-8");
    REQUIRE(xDeclaration.standalone() == "no");
    REQUIRE(xRoot.name() == "AddressBook");
    REQUIRE(xRoot.getChildren().size() == 3);
    REQUIRE(xRoot[0].name() == "Address");
    REQUIRE(xRoot[0].getContents() == "    This is some contents    ");
  }
  SECTION("Root element <AddressBook> with multiple sibling <Address> elements and contents ", "[XML][Parse][Root]")
  {
    BufferSource source{
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
      "</AddressBook>\n"
    };
    xml.parse(source);
    auto &xDeclaration = XRef<Declaration>(xml.declaration());
    auto &xRoot = XRef<Element>(xml.root());
    REQUIRE(xDeclaration.version() == "1.0");
    REQUIRE(xDeclaration.encoding() == "UTF-8");
    REQUIRE(xDeclaration.standalone() == "no");
    REQUIRE(xRoot.name() == "AddressBook");
    REQUIRE(xRoot.getChildren().size() == 7);
    REQUIRE(xRoot[0].name() == "Address");
    REQUIRE(xRoot[0].getContents() == "\n    This is some contents 1   ");
    REQUIRE(xRoot[1].name() == "Address");
    REQUIRE(xRoot[1].getContents() == "\n    This is some contents 2   ");
    REQUIRE(xRoot[2].name() == "Address");
    REQUIRE(xRoot[2].getContents() == "\n    This is some contents 3   ");
  }
  SECTION("Self closing root tag ", "[XML][Parse][Root]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<root/>\n"
    };
    REQUIRE_NOTHROW(xml.parse(source));
  }
}