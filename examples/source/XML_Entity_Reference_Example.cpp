// Program: XML_Entity_Reference_Example
//
// Description: Demonstrates parsing XML with predefined and custom entity references, including error handling for
// undefined entities.
//
// Dependencies: C++20, PLOG, XML_Lib.
//

#include "XML_Utility.hpp"

namespace xl = XML_Lib;

int main()
{
  init(plog::debug, "XML_Entity_Reference_Example.log");
  PLOG_INFO << "XML_Entity_Reference_Example started ...";
  xl::XML xml;
  // Example with predefined entities
  std::string xml_with_entities = R"(
    <root>
      <text>5 &lt; 10 &amp;&amp; 10 &gt; 5 &quot;quoted&quot; &apos;single&apos;</text>
    </root>
  )";
  try {
    xl::BufferSource source{ xml_with_entities };
    xml.parse(source);
    PLOG_INFO << "Parsed XML with predefined entities successfully.";
  } catch (const std::exception &e) {
    PLOG_ERROR << "Error: " << e.what();
  }
  // Example with undefined entity
  std::string xml_with_undefined_entity = R"(
    <root>
      <text>This will fail: &undefined;</text>
    </root>
  )";
  try {
    xl::BufferSource source{ xml_with_undefined_entity };
    xml.parse(source);
    PLOG_INFO << "Parsed XML with undefined entity (unexpected).";
  } catch (const std::exception &e) {
    PLOG_ERROR << "Expected entity error: " << e.what();
  }
  PLOG_INFO << "XML_Entity_Reference_Example exited.";
  exit(EXIT_SUCCESS);
}
