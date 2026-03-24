// Program: XML_Whitespace_Example
//
// Description: Demonstrates handling of whitespace and formatting edge cases in XML.
//
// Dependencies: C++20, PLOG, XML_Lib.
//

#include "XML_Utility.hpp"

namespace xl = XML_Lib;

int main() {
  init(plog::debug, "XML_Whitespace_Example.log");
  PLOG_INFO << "XML_Whitespace_Example started ...";
  xl::XML xml;
  try {
    xl::BufferSource source{"<root>   <child>   </child>   </root>"};
    xml.parse(source);
    PLOG_INFO << "Parsed XML with whitespace between tags.";
    auto &xRoot = xl::NRef<xl::Element>(xml.root());
    PLOG_INFO << "Root element: " << xRoot.name();
    PLOG_INFO << "First child: " << xRoot[0].name();
  } catch (const std::exception &e) {
    PLOG_ERROR << "Error parsing whitespace XML: " << e.what();
  }
  return 0;
}
