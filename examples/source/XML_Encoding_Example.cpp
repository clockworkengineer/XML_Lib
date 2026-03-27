// Program: XML_Encoding_Example
//
// Description: Demonstrates parsing XML with different encodings (UTF-8, UTF-16, UTF-32) and BOM handling. Shows error
// for unsupported encodings.
//
// Dependencies: C++20, PLOG, XML_Lib.
//

#include "../include/XML_Utility.hpp"

namespace xl = XML_Lib;

int main()
{
  init(plog::debug, "XML_Encoding_Example.log");
  PLOG_INFO << "XML_Encoding_Example started ...";
  xl::XML xml;
  // UTF-8 example
  std::string utf8_xml = R"(<?xml version='1.0' encoding='UTF-8'?><root>UTF-8 text: café</root>)";
  try {
    xl::BufferSource source{ utf8_xml };
    xml.parse(source);
    PLOG_INFO << "Parsed UTF-8 XML successfully.";
  } catch (const std::exception &e) {
    PLOG_ERROR << "Error: " << e.what();
  }
  // UTF-16 example (as bytes)
  std::vector<unsigned char> utf16_bytes = { 0xFF,
    0xFE,// BOM
    '<',
    0x00,
    '?',
    0x00,
    'x',
    0x00,
    'm',
    0x00,
    'l',
    0x00,
    ' ',
    0x00,
    'v',
    0x00,
    'e',
    0x00,
    'r',
    0x00,
    's',
    0x00,
    'i',
    0x00,
    'o',
    0x00,
    'n',
    0x00,
    '=',
    0x00,
    '\'',
    0x00,
    '1',
    0x00,
    '.',
    0x00,
    '0',
    0x00,
    '\'',
    0x00,
    ' ',
    0x00,
    'e',
    0x00,
    'n',
    0x00,
    'c',
    0x00,
    'o',
    0x00,
    'd',
    0x00,
    'i',
    0x00,
    'n',
    0x00,
    'g',
    0x00,
    '=',
    0x00,
    '\'',
    0x00,
    'U',
    0x00,
    'T',
    0x00,
    'F',
    0x00,
    '-',
    0x00,
    '1',
    0x00,
    '6',
    0x00,
    '\'',
    0x00,
    '?',
    0x00,
    '>',
    0x00,
    '<',
    0x00,
    'r',
    0x00,
    'o',
    0x00,
    'o',
    0x00,
    't',
    0x00,
    '>',
    0x00,
    'U',
    0x00,
    'T',
    0x00,
    'F',
    0x00,
    '-',
    0x00,
    '1',
    0x00,
    '6',
    0x00,
    ' ',
    0x00,
    't',
    0x00,
    'e',
    0x00,
    'x',
    0x00,
    't',
    0x00,
    '<',
    0x00,
    '/',
    0x00,
    'r',
    0x00,
    'o',
    0x00,
    'o',
    0x00,
    't',
    0x00,
    '>',
    0x00 };
  // Convert bytes to std::u16string (skip BOM for u16string)
  std::u16string utf16_xml;
  for (size_t i = 2; i + 1 < utf16_bytes.size(); i += 2) {
    char16_t ch = static_cast<char16_t>(utf16_bytes[i] | (utf16_bytes[i + 1] << 8));
    utf16_xml.push_back(ch);
  }
  try {
    xl::BufferSource source{ utf16_xml };
    xml.parse(source);
    PLOG_INFO << "Parsed UTF-16 XML successfully.";
  } catch (const std::exception &e) {
    PLOG_ERROR << "Error: " << e.what();
  }
  // Unsupported encoding example
  std::string unsupported_xml = R"(<?xml version='1.0' encoding='ISO-8859-1'?><root>text</root>)";
  try {
    xl::BufferSource source{ unsupported_xml };
    xml.parse(source);
    PLOG_INFO << "Parsed unsupported encoding (unexpected).";
  } catch (const std::exception &e) {
    PLOG_ERROR << "Expected encoding error: " << e.what();
  }
  PLOG_INFO << "XML_Encoding_Example exited.";
  exit(EXIT_SUCCESS);
}
