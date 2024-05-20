#pragma once

#include <string>
#include <algorithm>

namespace XML_Lib {

struct XMLAttribute : public XMLValue
{
  // Constructors/Destructors
  XMLAttribute(const std::string &name, const XMLValue &value) : XMLValue(value) , name(name) {} 
  XMLAttribute() = delete;
  XMLAttribute(const XMLAttribute &other) = default;
  XMLAttribute &operator=(const XMLAttribute &other) = default;
  XMLAttribute(XMLAttribute &&other) = default;
  XMLAttribute &operator=(XMLAttribute &&other) = default;
  ~XMLAttribute() = default;
  // Get attribute name
  [[nodiscard]] const std::string &getName() const { return name; }
  // Search for an attribute in vector of unique attributes
  [[nodiscard]] static bool isAttrubutePresent(const std::vector<XMLAttribute> &attributes, const std::string &attributeName)
  {
    return std::find_if(attributes.rbegin(), attributes.rend(), [&attributeName](const XMLAttribute &attr) {
      return attr.getName() == attributeName;
    }) != attributes.rend();
  }

private:
  // Attribute name
  std::string name;
};
}// namespace XML_Lib