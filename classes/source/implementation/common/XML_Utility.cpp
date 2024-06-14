//
// Class: XML
//
// Description: XML utility core functionality.
//
// Dependencies: C++20 - Language standard features used..
//

#include "XML.hpp"
#include "XML_Core.hpp"

namespace XML_Lib {

/// <summary>
/// Split a string into a vector of strings using the passed in delimiter.
/// </summary>
/// <param name="target">String to split up.</param>
/// <param name="delimiter">Character delimiter to split on.</param>
/// <returns>Vector of split strings.</returns>
std::vector<std::string> splitString(const std::string &target, const char delimiter)
{
  std::stringstream sourceStream(target);
  std::string splitOffItem;
  std::vector<std::string> splitStrings;
  while (std::getline(sourceStream, splitOffItem, delimiter)) { splitStrings.push_back(splitOffItem); }
  return splitStrings;
}

/// <summary>
/// Trim whitespace from beginning and end of a string.
/// </summary>
/// <param name="target">String to trim.</param>
std::string trimString(const std::string &target)
{
  std::string trimmedString{ target };
  trimmedString.erase(trimmedString.begin(),
    std::ranges::find_if(trimmedString, [](const unsigned char ch) { return !std::iswspace(ch); }));
  trimmedString.erase(
    std::ranges::find_if(trimmedString.rbegin(), trimmedString.rend(), [](const unsigned char ch) { return !std::iswspace(ch); })
      .base(),
    trimmedString.end());
  return trimmedString;
}

/// <summary>
/// Convert a string to all uppercase.
/// </summary>
/// <param name="target">String to convert.</param>
std::string toUpperString(const std::string &target)
{
  std::string upperCaseString{ target };
  std::ranges::transform(
    upperCaseString, upperCaseString.begin(), [](const unsigned int c) {
    return static_cast<char>(std::toupper(c));
  });
  return upperCaseString;
}

/// <summary>
/// Convert a string to all lowercase.
/// </summary>
/// <param name="target">String to convert.</param>
std::string toLowerString(const std::string &target)
{
  std::string lowerCaseString{ target };
  std::ranges::transform(
    lowerCaseString, lowerCaseString.begin(), [](const unsigned int c) {
    return static_cast<char>(std::tolower(c));
  });
  return lowerCaseString;
}
}// namespace  XML_Lib
