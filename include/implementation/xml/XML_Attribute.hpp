#pragma once
// =======
// C++ STL
// =======
#include <vector>
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
  XMLAttribute(std::string name, XMLValue value) : name(std::move(name)), value(std::move(value)) {}
  std::string name;
  XMLValue value;
};
}// namespace XML_Lib