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
  XMLValue(const std::string &unparsed = "", const std::string &parsed = "") : m_unparsed(unparsed), m_parsed(parsed) {}
  XMLValue(const XMLValue &other) = default;
  XMLValue &operator=(const XMLValue &other) = default;
  XMLValue(XMLValue &&other) = default;
  XMLValue &operator=(XMLValue &&other) = default;
  ~XMLValue() = default;
  // Is a reference value ?
  [[nodiscard]] bool isReference() const { return (m_unparsed[0] == '&' && m_unparsed.back() == ';'); }
  [[nodiscard]] bool isEntityReference() const { return (isReference() && m_unparsed[1] != '#'); }
  [[nodiscard]] bool isCharacterReference() const { return (isReference() && m_unparsed[1] == '#'); }
  // Get value 
  const std::string &getUnparsed() const { return (m_unparsed); }
  const std::string &getParsed() const { return (m_parsed); }

private:
  // Parsed/Unparsed value
  std::string m_unparsed{};
  std::string m_parsed{};
};
}// namespace XML_Lib