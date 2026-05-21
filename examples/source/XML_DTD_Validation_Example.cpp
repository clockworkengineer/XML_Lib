// Program: XML_DTD_Validation_Example
//
// Description: Demonstrates parsing and validation of XML with internal and external DTDs, including error reporting
// for DTD violations.
//
// Dependencies: C++20, XML_Lib.
//

#include "XML_Utility.hpp"

namespace xl = XML_Lib;

int main()
{
  std::cout << "XML_DTD_Validation_Example started ..." << std::endl;
  xl::XML xml;
  // Example with internal DTD
  std::string xml_with_internal_dtd = R"(
    <?xml version='1.0'?>
    <!DOCTYPE note [
      <!ELEMENT note (to,from,heading,body)>
      <!ELEMENT to (#PCDATA)>
      <!ELEMENT from (#PCDATA)>
      <!ELEMENT heading (#PCDATA)>
      <!ELEMENT body (#PCDATA)>
    ]>
    <note>
      <to>Tove</to>
      <from>Jani</from>
      <heading>Reminder</heading>
      <body>Don't forget me this weekend!</body>
    </note>
  )";
  try {
    xl::BufferSource source{ xml_with_internal_dtd };
    xml.parse(source);
    std::cout << "Parsed XML with internal DTD successfully." << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
  // Example with DTD violation
  std::string xml_with_dtd_error = R"(
    <?xml version='1.0'?>
    <!DOCTYPE note [
      <!ELEMENT note (to,from,heading,body)>
      <!ELEMENT to (#PCDATA)>
      <!ELEMENT from (#PCDATA)>
      <!ELEMENT heading (#PCDATA)>
      <!ELEMENT body (#PCDATA)>
    ]>
    <note>
      <to>Tove</to>
      <from>Jani</from>
      <heading>Reminder</heading>
      <!-- Missing <body> element -->
    </note>
  )";
  try {
    xl::BufferSource source{ xml_with_dtd_error };
    xml.parse(source);
    std::cout << "Parsed XML with DTD error (unexpected)." << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "Expected DTD error: " << e.what() << std::endl;
  }
  std::cout << "XML_DTD_Validation_Example exited." << std::endl;
  exit(EXIT_SUCCESS);
}
