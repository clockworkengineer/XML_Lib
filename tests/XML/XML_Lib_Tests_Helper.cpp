//
// Unit Tests: XML_Lib_Tests_Helper
//
// Description: Helper functions for XML unit tests.
//

#include "XML_Lib_Tests.hpp"
// =======================
// XML class namespace
// =======================
using namespace XML_Lib;
// ==========================
// Unit test helper functions
// ==========================
// ==========================
// Unit test helper functions
// ==========================
/// <summary>
/// Prefix path to test data file name.
/// </summary>
/// <param name="name">Test data file name.</param>
/// <returns>Full path to test data file</returns>
std::string prefixPath(const std::string &file)
{
  std::filesystem::path currentPath = std::filesystem::current_path() / "files" / file;
  return (currentPath.string());
}

/// <summary>
/// Create an XML file and write XML to it in UTF8 encoding.
/// </summary>
/// <param name="xmlFileName">XML file name.</param>
/// <param name="xmlFileName">XML.</param>
/// <returns></returns>
void writeXMLToFileUTF8(const std::string &xmlFileName, const std::string &xmlString)
{
  std::remove(xmlFileName.c_str());
  std::ofstream xmlFile;
  xmlFile.open(xmlFileName, std::ios::binary);
  xmlFile << xmlString;
  xmlFile.close();
}

/// <summary>
/// Create an XML file and write XML to it in UTF16 encoding.
/// </summary>
/// <param name="xmlFileName">XML file name.</param>
/// <param name="xmlFileName">XML.</param>
/// <returns></returns>
void writeXMLToFileUTF16(const std::string &xmlFileName, const std::u16string &xmlString, bool be = true)
{
  std::remove(xmlFileName.c_str());
  std::ofstream xmlFile;
  char ch[2];
  xmlFile.open(xmlFileName);
  for (unsigned short u16 : xmlString) {
    if (be) {
      ch[0] = u16 >> 8;
      ch[1] = u16 & 0xFF;
    } else {
      ch[0] = u16 & 0xFF;
      ch[1] = u16 >> 8;
    }
    xmlFile.write(ch, 2);
  }
  xmlFile.close();
}

/// <summary>
/// Convert CRLF to LF for source and check number of CR/LF left after with
/// REQUIRE.
/// </summary>
/// <param name="xmlFileName">XML source.</param>
/// <param name="crFinal">Final number of CR.</param>
/// <param name="lfFinal">FInal number of LF.</param>
/// <returns></returns>
void verifyCRLFCount(ISource &source, long lfFinal, long crFinal)
{
  long crCount = 0;
  long lfCount = 0;
  while (source.more()) {
    switch (source.current()) {
    case kLineFeed:
      lfCount++;
      break;
    case kCarriageReturn:
      crCount++;
      break;
    }
    source.next();
  }
  REQUIRE(lfCount == lfFinal);
  REQUIRE(crCount == crFinal);
}

/// <summary>
/// Convert XML to an XML XNode tree, stringify the XML XNode tree and
/// compare the result with the passed in XML with REQUIRE.
/// </summary>
/// <param name="xmlString">XML to parse and stringify.</param>
/// <returns></returns>
void checkStringify(const std::string &xmlString)
{
  BufferSource source{ xmlString };
  XML xml;
  xml.parse(source);
  BufferDestination destination;
  xml.stringify(destination);
  REQUIRE(destination.getBuffer() == xmlString);
}