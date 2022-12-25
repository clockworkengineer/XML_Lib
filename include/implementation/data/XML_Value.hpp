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
  XMLValue(const std::string &unparsed = "", const std::string &parsed = "") : unparsed(unparsed), parsed(parsed) {}
  [[nodiscard]] bool isReference() const { return (unparsed[0] == '&' && unparsed.back() == ';'); }
  [[nodiscard]] bool isEntityReference() const { return (isReference() && unparsed[1] != '#'); }
  [[nodiscard]] bool isCharacterReference() const { return (isReference() && unparsed[1] == '#'); }
  std::string unparsed{};
  std::string parsed{};
};
}// namespace XML_Lib