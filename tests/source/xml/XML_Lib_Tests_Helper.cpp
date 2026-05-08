#include "XML_Lib_Tests.hpp"
#include <cstdio>

/// <summary>
/// Prefix path to test data file name.
/// </summary>
/// <param name="xmlFileName">Test data file name.</param>
std::string prefixTestDataPath(const std::string &xmlFileName)
{
  std::filesystem::path base;
  const std::filesystem::path sourceFile = __FILE__;
  if (sourceFile.is_absolute()) {
    auto repoRoot = sourceFile.parent_path(); // tests/source/xml
    if (repoRoot.filename() == "xml") { repoRoot = repoRoot.parent_path(); }
    if (repoRoot.filename() == "source") { repoRoot = repoRoot.parent_path(); }
    if (repoRoot.filename() == "tests") { repoRoot = repoRoot.parent_path(); }
    base = repoRoot;
  } else {
    try {
      base = std::filesystem::current_path();
    } catch (const std::filesystem::filesystem_error &) {
      base.clear();
    }
  }

  const std::vector<std::filesystem::path> candidates{
    base / "tests" / "files",
    base / "files",
    base / "build" / "tests" / "files",
    base / "build-tests" / "tests" / "files",
  };

  for (const auto &candidate : candidates) {
    const auto filePath = candidate / xmlFileName;
    if (!filePath.empty() && std::filesystem::exists(filePath)) {
      return filePath.string();
    }
  }

  if (!base.empty()) {
    return (base / "tests" / "files" / xmlFileName).string();
  }

  return xmlFileName;
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
  char tmpNameBuffer[L_tmpnam];
  if (std::tmpnam(tmpNameBuffer) == nullptr) {
    throw std::runtime_error("Failed to generate temporary file name");
  }
  std::filesystem::path namepath = tmpNameBuffer;
  std::string result{ std::filesystem::temp_directory_path().string() };
  result.push_back(std::filesystem::path::preferred_separator);
  return result + namepath.filename().string();
}