// Program: XML_Entity_Reference_Example
//
// Description: Demonstrates parsing XML with predefined and custom entity references, including error handling for
// undefined entities.
//
// Dependencies: C++20, XML_Lib.
//

#include "XML_Utility.hpp"

namespace xl = XML_Lib;

/// @briefProgram entry point.</summary>
int main()
{
  std::cout << "XML_Entity_Reference_Example started ..." << std::endl;
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
    std::cout << "Parsed XML with predefined entities successfully." << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
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
    std::cout << "Parsed XML with undefined entity (unexpected)." << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "Expected entity error: " << e.what() << std::endl;
  }
  std::cout << "XML_Entity_Reference_Example exited." << std::endl;
  exit(EXIT_SUCCESS);
}
