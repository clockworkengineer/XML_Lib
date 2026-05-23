// Program: XML_Malformed_Example
//
// Description: Demonstrates handling and reporting of malformed XML input.
//
// Dependencies: C++20, XML_Lib.
//

#include "XML_Utility.hpp"

namespace xl = XML_Lib;

/// <summary>Program entry point.</summary>
int main() {
  std::cout << "XML_Malformed_Example started ..." << std::endl;
  xl::XML xml;
  try {
    xl::BufferSource source{"<root><child></root>"};
    xml.parse(source);
    std::cout << "Parsed malformed XML without error (unexpected)." << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "Error parsing malformed XML: " << e.what() << std::endl;
  }
  return 0;
}
