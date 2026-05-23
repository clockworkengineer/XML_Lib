// Program: XML_Mixed_Content_Example
//
// Description: Demonstrates parsing and validation of XML with mixed content (elements containing both text and child
// elements), including error handling for invalid mixed content.
//
// Dependencies: C++20, XML_Lib.
//

#include "XML_Utility.hpp"

namespace xl = XML_Lib;

/// <summary>Program entry point.</summary>
int main()
{
  std::cout << "XML_Mixed_Content_Example started ..." << std::endl;
  xl::XML xml;
  // Valid mixed content
  std::string valid_mixed = R"(
    <!DOCTYPE para [
      <!ELEMENT para (#PCDATA|emph|ul|ol|li)*>
      <!ELEMENT emph (#PCDATA)>
      <!ELEMENT ul (li*)>
      <!ELEMENT ol (li*)>
      <!ELEMENT li (#PCDATA)>
    ]>
    <para>This is <emph>mixed</emph> content with <ul><li>list</li></ul>.</para>
  )";
  try {
    xl::BufferSource source{ valid_mixed };
    xml.parse(source);
    std::cout << "Parsed valid mixed content successfully." << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
  // Invalid mixed content (illegal order)
  std::string invalid_mixed = R"(
    <!DOCTYPE para [
      <!ELEMENT para (emph|#PCDATA)*>
      <!ELEMENT emph (#PCDATA)>
    ]>
    <para><emph>Should fail</emph> and then text</para>
  )";
  try {
    xl::BufferSource source{ invalid_mixed };
    xml.parse(source);
    std::cout << "Parsed invalid mixed content (unexpected)." << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "Expected mixed content error: " << e.what() << std::endl;
  }
  std::cout << "XML_Mixed_Content_Example exited." << std::endl;
  exit(EXIT_SUCCESS);
}
