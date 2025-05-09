//
// Class: XML_Impl_File
//
// Description: XML class implementation layer to read and write
// XML files in a number of different formats. Note that these
// methods are all static and do not need an XML object to invoke.
// For more information on byte marks and their meaning check out link
// https://en.wikipedia.org/wiki/Byte_order_mark.
//
// Dependencies: C++20 - Language standard features used.
//

#include "XML_Impl.hpp"

namespace XML_Lib {

/// <summary>
/// Write XML string to a file stream.
/// </summary>
/// <param name="xmlFile">XML file stream</param>
/// <param name="xmlString">XML string</param>
void writeXMLString(std::ofstream &xmlFile, const std::string_view &xmlString) { xmlFile << xmlString; }
/// <summary>
/// Write XML string to a file stream.
/// </summary>
/// <param name="xmlFile">XML file stream</param>
/// <param name="xmlString">XML string</param>
/// <param name="format">XML file format</param>
void writeXMLString(std::ofstream &xmlFile, const std::u16string &xmlString, const XML::Format format)
{
  if (format == XML::Format::utf16BE) {
    xmlFile << static_cast<unsigned char>(0xFE) << static_cast<unsigned char>(0xFF);
    for (const auto ch : xmlString) {
      xmlFile.put(static_cast<char>(ch >> 8));
      xmlFile.put(static_cast<char>(ch));
    }
  } else if (format == XML::Format::utf16LE) {
    xmlFile << static_cast<unsigned char>(0xFF) << static_cast<unsigned char>(0xFE);
    for (const auto ch : xmlString) {
      xmlFile.put(static_cast<char>(ch));
      xmlFile.put(static_cast<char>(ch >> 8));
    }
  } else {
    throw Error("Unsupported XML file format (Byte Order Mark) specified in call to writeXMLString().");
  }
}

/// <summary>
/// Read XML string from a file stream.
/// </summary>
/// <param name="xmlFile">XML file stream</param>
/// <returns>XML string.</returns>
std::string readXMLString(const std::ifstream &xmlFile)
{
  std::ostringstream xmlFileBuffer;
  xmlFileBuffer << xmlFile.rdbuf();
  return xmlFileBuffer.str();
}
std::u16string readXMLString(std::ifstream &xmlFile, const XML::Format format)
{
  std::u16string utf16String;
  // Move past byte order mark
  xmlFile.seekg(2);
  if (format == XML::Format::utf16BE)
    while (true) {
      char16_t ch16 = static_cast<unsigned char>(xmlFile.get()) << 8;
      ch16 |= static_cast<unsigned char>(xmlFile.get());
      if (xmlFile.eof()) break;
      utf16String.push_back(ch16);
    }
  else if (format == XML::Format::utf16LE) {
    while (true) {
      char16_t ch16 = static_cast<unsigned char>(xmlFile.get());
      ch16 |= static_cast<unsigned char>(xmlFile.get()) << 8;
      if (xmlFile.eof()) break;
      utf16String.push_back(ch16);
    }
  } else {
    throw Error("Unsupported XML file format (Byte Order Mark) specified in call to readXMLString().");
  }
  return utf16String;
}

/// <summary>
/// Return format of an XML file after checking for any byte order marks at
/// the beginning of the XML file.
/// </summary>
/// <param name="fileName">XML file name</param>
/// <returns>XML file format.</returns>
XML::Format XML_Impl::getFileFormat(const std::string_view &fileName)
{
  std::ifstream xmlFile{ fileName.data(), std::ios_base::binary };
  uint32_t byteOrderMark = static_cast<unsigned char>(xmlFile.get()) << 24;
  byteOrderMark |= static_cast<unsigned char>(xmlFile.get()) << 16;
  byteOrderMark |= static_cast<unsigned char>(xmlFile.get()) << 8;
  byteOrderMark |= static_cast<unsigned char>(xmlFile.get());
  if (byteOrderMark == 0x0000FEFF) { return XML::Format::utf32BE; }
  if (byteOrderMark == 0xFFFE0000) { return XML::Format::utf32LE; }
  if ((byteOrderMark & 0xFFFFFF00) == 0xEFBBBF00) { return XML::Format::utf8BOM; }
  if ((byteOrderMark & 0xFFFF0000) == 0xFEFF0000) { return XML::Format::utf16BE; }
  if ((byteOrderMark & 0xFFFF0000) == 0xFFFE0000) { return XML::Format::utf16LE; }
  xmlFile.close();
  return XML::Format::utf8;
}

/// <summary>
/// Open a XML file, read its contents into a string buffer and return
/// the buffer. Note any CRLF in the source file are translated to just a
/// LF internally.
/// </summary>
/// <param name="fileName">XML file name</param>
/// <returns>XML string.</returns>
std::string XML_Impl::fromFile(const std::string_view &fileName)
{
  const auto kCRLF = "\x0D\x0A";
  // Get a file format
  const XML::Format format = getFileFormat(fileName);
  // Read in XML
  std::ifstream xmlFile{ fileName.data(), std::ios_base::binary };
  std::string translated;
  switch (format) {
  case XML::Format::utf8BOM:
    xmlFile.seekg(3);// Move past the byte order mark
  case XML::Format::utf8:
    translated = readXMLString(xmlFile);
    break;
  case XML::Format::utf16BE:
  case XML::Format::utf16LE:
    translated = toUtf8(readXMLString(xmlFile, format));
    break;
  default:
    throw Error("Unsupported XML file format (Byte Order Mark) encountered.");
  }
  xmlFile.close();
  // Translate CRLF -> LF
  size_t pos = translated.find(kCRLF);
  while (pos != std::string::npos) {
    const auto kLF = "\x0A";
    translated.replace(pos, 2, kLF);
    pos = translated.find(kCRLF, pos + 1);
  }
  return translated;
}

/// <summary>
/// Create an XML file and write XML string to it.
/// </summary>
/// <param name="fileName">XML file name</param>
/// <param name="xmlString">XML string</param>
/// <param name="format">XML file format</param>
void XML_Impl::toFile(const std::string_view &fileName, const std::string_view &xmlString, const XML::Format format)
{
  std::ofstream xmlFile{ fileName.data(), std::ios::binary };
  switch (format) {
  case XML::Format::utf8BOM:
    xmlFile << static_cast<unsigned char>(0xEF) << static_cast<unsigned char>(0xBB) << static_cast<unsigned char>(0xBF);
  case XML::Format::utf8:
    writeXMLString(xmlFile, xmlString.data());
    break;
  case XML::Format::utf16BE:
  case XML::Format::utf16LE:
    writeXMLString(xmlFile, toUtf16(xmlString.data()), format);
    break;
  default:
    throw Error("Unsupported XML file format (Byte Order Mark) specified.");
  }
  xmlFile.close();
}
}// namespace XML_Lib