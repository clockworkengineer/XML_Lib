// Program: XML_Malformed_Example
//
// Description: Demonstrates handling and reporting of malformed XML input.
//
// Dependencies: C++20, PLOG, XML_Lib.
//

#include "XML_Utility.hpp"

namespace xl = XML_Lib;

int main() {
  init(plog::debug, "XML_Malformed_Example.log");
  PLOG_INFO << "XML_Malformed_Example started ...";
  xl::XML xml;
  try {
    xl::BufferSource source{"<root><child></root>"};
    xml.parse(source);
    PLOG_INFO << "Parsed malformed XML without error (unexpected).";
  } catch (const std::exception &e) {
    PLOG_ERROR << "Error parsing malformed XML: " << e.what();
  }
  return 0;
}
