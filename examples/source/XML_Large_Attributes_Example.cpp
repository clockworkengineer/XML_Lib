// Program: XML_Large_Attributes_Example
//
// Description: Demonstrates parsing XML with a large number of attributes.
//
// Dependencies: C++20, PLOG, XML_Lib.
//

#include "XML_Utility.hpp"

namespace xl = XML_Lib;

int main() {
  init(plog::debug, "XML_Large_Attributes_Example.log");
  PLOG_INFO << "XML_Large_Attributes_Example started ...";
  xl::XML xml;
  std::string xml_str = "<root";
  for (int i = 0; i < 100; ++i) xml_str += " attr" + std::to_string(i) + "='val'";
  xml_str += "></root>";
  try {
    xl::BufferSource source{xml_str};
    xml.parse(source);
    PLOG_INFO << "Parsed XML with 100 attributes.";
  } catch (const std::exception &e) {
    PLOG_ERROR << "Error parsing XML with many attributes: " << e.what();
  }
  return 0;
}
