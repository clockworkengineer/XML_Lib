// Program: XML_Deep_Nesting_Example
//
// Description: Demonstrates parsing and handling of deeply nested XML.
//
// Dependencies: C++20, XML_Lib.
//

#include "XML_Utility.hpp"

namespace xl = XML_Lib;

/// @briefProgram entry point.</summary>
int main() {
  std::cout << "XML_Deep_Nesting_Example started ..." << std::endl;
  xl::XML xml;
  std::string deep_xml = "<a>";
  for (int i = 0; i < 50; ++i) deep_xml += "<b>";
  for (int i = 0; i < 50; ++i) deep_xml += "</b>";
  deep_xml += "</a>";
  try {
    xl::BufferSource source{deep_xml};
    xml.parse(source);
    std::cout << "Successfully parsed deeply nested XML." << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "Error parsing deeply nested XML: " << e.what() << std::endl;
  }
  return 0;
}
