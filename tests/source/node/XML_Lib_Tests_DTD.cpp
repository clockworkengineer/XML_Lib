#include "XML_Lib_Tests.hpp"

TEST_CASE("Create and use DTD Node.","[Node][DTD][API]")
{
  SECTION("Create DTD Node.", "[XML][Node][DTD][API]")
  {
    XML_EntityMapper mapper;
    REQUIRE_NOTHROW(DTD(mapper));
  }
  SECTION("Create DTD Node and check root name hasn't been ser.", "[XML][Node][DTD][API]")
  {
    XML_EntityMapper mapper;
    DTD xDTD (mapper);
    REQUIRE(xDTD.getRootName()=="");
    REQUIRE(xDTD.getElementCount()==0);
  }
  SECTION("Create and use DTD using make/NRef API.", "[XML][Node][DTD][Make][NRef][API]")
  {
    XML_EntityMapper mapper;
    Node xNode = Node::make<DTD>(mapper);
    REQUIRE_FALSE(!isA<DTD>(xNode));
    REQUIRE(NRef<DTD>(xNode).getRootName()=="");
    REQUIRE(NRef<DTD>(xNode).getElementCount()==0);
  }
  SECTION("Add and retrieve DTD element", "[XML][Node][DTD][Element][API]")
  {
    XML_EntityMapper mapper;
    DTD xDTD(mapper);
    DTD::Element elem{"testElement", XMLValue("ANY", "ANY")};
    xDTD.addElement("testElement", elem);
    REQUIRE(xDTD.isElementPresent("testElement"));
    REQUIRE(xDTD.getElementCount() == 1);
    REQUIRE(xDTD.getElement("testElement").name == "testElement");
  }
  SECTION("Add and retrieve DTD notation", "[XML][Node][DTD][Notation][API]")
  {
    XML_EntityMapper mapper;
    DTD xDTD(mapper);
    XMLExternalReference notation("SYSTEM", "testNotation");
    xDTD.addNotation("testNotation", notation);
    REQUIRE(xDTD.getNotationCount("testNotation") == 1);
    REQUIRE(xDTD.getNotation("testNotation").getSystemID() == "testNotation");
  }
  SECTION("Add and retrieve DTD attribute", "[XML][Node][DTD][Attribute][API]")
  {
    XML_EntityMapper mapper;
    DTD xDTD(mapper);
    DTD::Attribute attr{"testAttr", DTD::AttributeType::cdata | DTD::AttributeType::required, "", XMLValue("value", "value")};
    DTD::Element elem{"testElement", XMLValue("ANY", "ANY")};
    elem.idAttributePresent = true;
    xDTD.addElement("testElement", elem);
    // Simulate attribute addition and retrieval
    REQUIRE_NOTHROW(xDTD.getElement("testElement"));
    REQUIRE(xDTD.getElement("testElement").name == "testElement");
  }
  SECTION("Throw error for missing element/notation", "[XML][Node][DTD][Error][API]")
  {
    XML_EntityMapper mapper;
    DTD xDTD(mapper);
    REQUIRE_THROWS_AS(xDTD.getElement("missingElement"), DTD::Error);
    REQUIRE_THROWS_AS(xDTD.getNotation("missingNotation"), DTD::Error);
  }
}



