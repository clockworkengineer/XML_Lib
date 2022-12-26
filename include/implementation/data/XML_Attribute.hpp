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
  XMLAttribute(const std::string &name, const XMLValue &value) : name(name), value(value) {}
  XMLAttribute(const XMLAttribute &other) = default;
  XMLAttribute &operator=(const XMLAttribute &other) = default;
  XMLAttribute(XMLAttribute &&other) = default;
  XMLAttribute &operator=(XMLAttribute &&other) = default;
  ~XMLAttribute() = default;
  // Attribute name and value
  std::string name;
  XMLValue value;
};
}// namespace XML_Lib