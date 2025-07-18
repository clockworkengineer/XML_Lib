#include "XML_Lib_Tests.hpp"

/// <summary>
/// Prefix path to test data file name.
/// </summary>
/// <param name="xmlFileName">Test data file name.</param>
std::string prefixTestDataPath(const std::string &xmlFileName)
{
  if (std::filesystem::is_directory("./files")) {
    return (std::filesystem::current_path() / "./files" / xmlFileName).string();
  } else {
    return (std::filesystem::current_path() / "../files" / xmlFileName).string();
  }
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
/// Convert XML to an XML Node tree, stringify the XML Node tree and
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

/// <summary>
/// Generate unique file name.
/// </summary>
/// <returns>Unique torrent file name</returns>
std::string generateRandomFileName(void)
{
  std::filesystem::path namepath = std::tmpnam(nullptr);
  std::string result { std::filesystem::temp_directory_path().string() };
  result.push_back(std::filesystem::path::preferred_separator);
  return result+namepath.filename().string();
}