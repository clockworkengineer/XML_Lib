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
  XMLAttribute(const std::string &name, const XMLValue &value) : name(name), value(value) {}
  std::string name;
  XMLValue value;
};
}// namespace XML_Lib