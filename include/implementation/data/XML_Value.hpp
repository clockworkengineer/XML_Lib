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
  // Constructors/Destructors
  XMLValue(const std::string &unparsed = "", const std::string &parsed = "") : unparsed(unparsed), parsed(parsed) {}
  XMLValue(const XMLValue &other) = default;
  XMLValue &operator=(const XMLValue &other) = default;
  XMLValue(XMLValue &&other) = default;
  XMLValue &operator=(XMLValue &&other) = default;
  ~XMLValue() = default;
  // Is a reference value ?
  [[nodiscard]] bool isReference() const { return (unparsed[0] == '&' && unparsed.back() == ';'); }
  [[nodiscard]] bool isEntityReference() const { return (isReference() && unparsed[1] != '#'); }
  [[nodiscard]] bool isCharacterReference() const { return (isReference() && unparsed[1] == '#'); }
  // Get value 
  const std::string &getUnparsed() const { return (unparsed); }
  const std::string &getParsed() const { return (parsed); }

private:
  // Parsed/Unparsed value
  std::string unparsed{};
  std::string parsed{};
};
}// namespace XML_Lib