//
// Unit Tests: XML_Lib_Tests_DTD_Validate_Elements
//
// Description: Unit tests for XML DTD element validation.
//

#include "XML_Lib_Tests.hpp"

using namespace XML_Lib;

TEST_CASE("Parse XML with various DTD issues.", "[XML][DTD][Validate]")
{
  XML xml;
  SECTION("XML with no DTD and try to validate.", "[XML][DTD][Validate]")
  {
    BufferSource source{
      "<?xml version=\"1.0\"?>\n"
      "<root>\n"
      "<child1>contents</child1>\n"
      "<child1></child1>\n"
      "</root>\n"
    };
    xml.parse(source);
    REQUIRE_THROWS_WITH(xml.validate(), "XML Error: No DTD specified for validation.");
  }
  SECTION("XML parse 2 xml strings with DTD.", "[XML][DTD][Validate][Elements]")
  {
    BufferSource source1{
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE root [\n"
      "<!ELEMENT root (child1)+ >\n"
      "<!ELEMENT child1 (#PCDATA)>\n"
      "]>\n"
      "<root>\n"
      "<child1>contents</child1>\n"
      "<child1></child1>\n"
      "</root>\n"
    };
    BufferSource source2{
      "<?xml version=\"1.0\"?>\n"
      "<!DOCTYPE root [\n"
      "<!ELEMENT root (child1)+ >\n"
      "<!ELEMENT child1 (#PCDATA)>\n"
      "]>\n"
      "<root>\n"
      "<child1>contents</child1>\n"
      "<child1></child1>\n"
      "</root>\n"
    };
    xml.parse(source1);
    REQUIRE_NOTHROW(xml.parse(source2));
  }
}