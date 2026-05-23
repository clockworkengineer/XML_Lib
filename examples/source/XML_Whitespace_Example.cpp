// Program: XML_Whitespace_Example
//
// Description: Demonstrates handling of whitespace and formatting edge cases in XML.
//
// Dependencies: C++20, XML_Lib.
//

#include "XML_Utility.hpp"

namespace xl = XML_Lib;

/// <summary>Program entry point.</summary>
int main() {
  std::cout << "XML_Whitespace_Example started ..." << std::endl;
  xl::XML xml;
  try {
    xl::BufferSource source{"<root>   <child>   </child>   </root>"};
    xml.parse(source);
    std::cout << "Parsed XML with whitespace between tags." << std::endl;
    auto &xRoot = xl::NRef<xl::Element>(xml.root());
    std::cout << "Root element: " << xRoot.name() << std::endl;
    std::cout << "First child: " << xRoot[0].name() << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "Error parsing whitespace XML: " << e.what() << std::endl;
  }
  return 0;
}
