#include "XML_Lib_Tests.hpp"

/// <summary>
/// Prefix path to test data file name.
/// </summary>
/// <param name="xmlFileName">Test data file name.</param>
std::string prefixTestDataPath(const std::string &xmlFileName)
{
#ifdef _WIN32
  return (std::filesystem::current_path() / "./files" / xmlFileName).string();
#else
  return (std::filesystem::current_path() / "../files" / file).string();
#endif
}

/// <summary>
/// Convert CRLF to LF for source and check number of CR/LF left after with
/// REQUIRE.
/// </summary>
/// <param name="source">XML source.</param>
/// <param name="crFinal">Final number of CR.</param>
/// <param name="lfFinal">FInal number of LF.</param>
/// <returns></returns>
void verifyCRLFCount(ISource &source, const long lfFinal, const long crFinal)
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
    default:;
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
  const XML xml;
  xml.parse(source);
  BufferDestination destination;
  xml.stringify(destination);
  REQUIRE(destination.toString() == xmlString);
}