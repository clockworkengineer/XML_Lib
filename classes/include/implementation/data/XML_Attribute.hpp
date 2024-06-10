#pragma once

#include <string>
#include <algorithm>

namespace XML_Lib {

struct XMLAttribute : XMLValue
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
  [[nodiscard]] static bool contains(const std::vector<XMLAttribute> &attributes, const std::string &name)
  {
    return std::find_if(attributes.rbegin(), attributes.rend(), [&name](const XMLAttribute &attr) {
      return attr.getName() == name;
    }) != attributes.rend();
  }
  // Return attribute entry
  [[nodiscard]] static const XMLAttribute &find(const std::vector<XMLAttribute> &attributes, const std::string &name)
  {
    return *std::find_if(
      attributes.rbegin(), attributes.rend(), [&name](const XMLAttribute &ns) { return ns.getName() == name; });
  }

private:
  // Attribute name
  std::string name;
};
}// namespace XML_Lib