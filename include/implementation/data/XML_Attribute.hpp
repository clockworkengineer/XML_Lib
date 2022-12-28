#pragma once
// =======
// C++ STL
// =======
#include <string>
// =================
// LIBRARY NAMESPACE
// =================
namespace XML_Lib {
// =============
// XML Attribute
// =============
struct XMLAttribute
{
  // Constructors/Destructors
  XMLAttribute(const std::string &name, const XMLValue &value) : m_name(name), m_value(value) {}
  XMLAttribute(const XMLAttribute &other) = default;
  XMLAttribute &operator=(const XMLAttribute &other) = default;
  XMLAttribute(XMLAttribute &&other) = default;
  XMLAttribute &operator=(XMLAttribute &&other) = default;
  ~XMLAttribute() = default;
  // Get attribute details
  const std::string &getName() const { return (m_name); }
  const std::string &getValue() const { return (m_value.getParsed()); }
  const std::string &getUnparsed() const { return (m_value.getUnparsed()); }

private:
  // Attribute name and value
  std::string m_name;
  XMLValue m_value;
};
}// namespace XML_Lib