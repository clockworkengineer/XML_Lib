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
/// Split a string into a vector of strings using the passed in delimeter.
/// </summary>
/// <param name="stringToSplit">String to split up.</param>
/// <param name="delimeter">Character delimeter to split on.</param>
/// <returns>Vector of split strings.</returns>
std::vector<std::string> splitString(const std::string &target, char delimeter)
{
  std::stringstream sourceStream(target);
  std::string splitOffItem;
  std::vector<std::string> splitStrings;
  while (std::getline(sourceStream, splitOffItem, delimeter)) { splitStrings.push_back(splitOffItem); }
  return splitStrings;
}

/// <summary>
/// Trimm whitespace from beginning and end of a string.
/// </summary>
/// <param name="stringToTrimm">String to trimm.</param>
std::string trimmString(const std::string &target)
{
  std::string trimmedString{ target };
  trimmedString.erase(trimmedString.begin(),
    std::find_if(trimmedString.begin(), trimmedString.end(), [](unsigned char ch) { return !std::iswspace(ch); }));
  trimmedString.erase(
    std::find_if(trimmedString.rbegin(), trimmedString.rend(), [](unsigned char ch) { return !std::iswspace(ch); })
      .base(),
    trimmedString.end());
  return (trimmedString);
}

/// <summary>
/// Convert a string to all uppercase.
/// </summary>
/// <param name="stringToTrimm">String to convert.</param>
std::string toUpperString(const std::string &target)
{
  std::string upperCaseString{ target };
  std::transform(upperCaseString.begin(), upperCaseString.end(), upperCaseString.begin(), [](unsigned int c) {
    return static_cast<XML_Lib::Char>(std::toupper(c));
  });
  return (upperCaseString);
}

/// <summary>
/// Convert a string to all lowercase.
/// </summary>
/// <param name="stringToTrimm">String to convert.</param>
std::string toLowerString(const std::string &target)
{
  std::string lowerCaseString{ target };
  std::transform(lowerCaseString.begin(), lowerCaseString.end(), lowerCaseString.begin(), [](unsigned int c) {
    return static_cast<XML_Lib::Char>(std::tolower(c));
  });
  return (lowerCaseString);
}
}// namespace  XML_Lib
