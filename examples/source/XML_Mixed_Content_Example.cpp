// Program: XML_Mixed_Content_Example
//
// Description: Demonstrates parsing and validation of XML with mixed content (elements containing both text and child
// elements), including error handling for invalid mixed content.
//
// Dependencies: C++20, PLOG, XML_Lib.
//

#include "XML_Utility.hpp"

namespace xl = XML_Lib;

int main()
{
  init(plog::debug, "XML_Mixed_Content_Example.log");
  PLOG_INFO << "XML_Mixed_Content_Example started ...";
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
    PLOG_INFO << "Parsed valid mixed content successfully.";
  } catch (const std::exception &e) {
    PLOG_ERROR << "Error: " << e.what();
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
    PLOG_INFO << "Parsed invalid mixed content (unexpected).";
  } catch (const std::exception &e) {
    PLOG_ERROR << "Expected mixed content error: " << e.what();
  }
  PLOG_INFO << "XML_Mixed_Content_Example exited.";
  exit(EXIT_SUCCESS);
}
