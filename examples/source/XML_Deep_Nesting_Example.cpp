// Program: XML_Deep_Nesting_Example
//
// Description: Demonstrates parsing and handling of deeply nested XML.
//
// Dependencies: C++20, PLOG, XML_Lib.
//

#include "XML_Utility.hpp"

namespace xl = XML_Lib;

int main() {
  init(plog::debug, "XML_Deep_Nesting_Example.log");
  PLOG_INFO << "XML_Deep_Nesting_Example started ...";
  xl::XML xml;
  std::string deep_xml = "<a>";
  for (int i = 0; i < 50; ++i) deep_xml += "<b>";
  for (int i = 0; i < 50; ++i) deep_xml += "</b>";
  deep_xml += "</a>";
  try {
    xl::BufferSource source{deep_xml};
    xml.parse(source);
    PLOG_INFO << "Successfully parsed deeply nested XML.";
  } catch (const std::exception &e) {
    PLOG_ERROR << "Error parsing deeply nested XML: " << e.what();
  }
  return 0;
}
