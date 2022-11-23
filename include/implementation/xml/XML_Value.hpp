#pragma once
// =======
// C++ STL
// =======
#include <string>
// =================
// LIBRARY NAMESPACE
// =================
namespace XML_Lib {
// =========
// XML value
// =========
struct XMLValue
{
  explicit XMLValue(std::string unparsed) : unparsed(std::move(unparsed)) {}
  XMLValue(std::string unparsed, std::string parsed) : unparsed(std::move(unparsed)), parsed(std::move(parsed)) {}
  [[nodiscard]] bool isReference() const { return (unparsed[0] == '&' && unparsed.back() == ';'); }
  [[nodiscard]] bool isEntityReference() const
  {
    return (unparsed[0] == '&' && unparsed[1] != '#' && unparsed.back() == ';');
  }
  [[nodiscard]] bool isCharacterReference() const
  {
    return (unparsed[0] == '&' && unparsed[1] == '#' && unparsed.back() == ';');
  }
  std::string unparsed{};
  std::string parsed{};
};
}// namespace XML_Lib