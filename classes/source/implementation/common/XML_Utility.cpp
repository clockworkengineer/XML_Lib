//
// Class: XML
//
// Description: XML utility core functionality.
//
// Dependencies: C++20 - Language standard features used.
//

#include "XML_Utility.hpp"
#include <algorithm>
#include <cwctype>
#include <sstream>
#include <string>
#include <vector>

namespace XML_Lib {

/// @brief
/// Split a string into a vector of strings using the passed in delimiter.

/// @param target String to split up.
/// @param delimiter Character delimiter to split on.
/// @return Vector of split strings.
std::vector<std::string> splitString(const std::string_view &target, const char delimiter)
{
  std::stringstream sourceStream{std::string(target)};
  std::string splitOffItem;
  std::vector<std::string> splitStrings;
  while (std::getline(sourceStream, splitOffItem, delimiter)) { splitStrings.push_back(splitOffItem); }
  return splitStrings;
}

/// @brief
/// Trim whitespace from the beginning and end of a string.

/// @param target String to trim.
std::string trimString(const std::string_view &target)
{
  std::string trimmedString{ target };
  trimmedString.erase(trimmedString.begin(),
    std::find_if(trimmedString.begin(), trimmedString.end(), [](const unsigned char ch) { return !std::iswspace(ch); }));
  trimmedString.erase(
    std::find_if(trimmedString.rbegin(), trimmedString.rend(), [](const unsigned char ch) { return !std::iswspace(ch); })
      .base(),
    trimmedString.end());
  return trimmedString;
}

namespace {
/// @brief
/// Implementation of transformStringCase.

std::string transformStringCase(const std::string_view &target, int (*fn)(int))
{
  std::string result{ target };
  std::transform(result.begin(), result.end(), result.begin(),
    [fn](const unsigned int c) { return static_cast<char>(fn(c)); });
  return result;
}
} // namespace

/// @brief
/// Convert a string to all uppercase.

/// @param target String to convert.
std::string toUpperString(const std::string_view &target) { return transformStringCase(target, std::toupper); }

/// @brief
/// Convert a string to all lowercase.

/// @param target String to convert.
std::string toLowerString(const std::string_view &target) { return transformStringCase(target, std::tolower); }
}// namespace  XML_Lib