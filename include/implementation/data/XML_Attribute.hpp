#pragma once

#include <string>
#include <algorithm>

namespace XML_Lib {

struct XMLAttribute
{
  // Constructors/Destructors
  XMLAttribute(const std::string &name, const XMLValue &value) : name(name), value(value) {}
  XMLAttribute() = delete;
  XMLAttribute(const XMLAttribute &other) = default;
  XMLAttribute &operator=(const XMLAttribute &other) = default;
  XMLAttribute(XMLAttribute &&other) = default;
  XMLAttribute &operator=(XMLAttribute &&other) = default;
  ~XMLAttribute() = default;
  // Get attribute details
  [[nodiscard]] const std::string &getName() const { return (name); }
  [[nodiscard]] const std::string &getParsed() const { return (value.getParsed()); }
  [[nodiscard]] const std::string &getUnparsed() const { return (value.getUnparsed()); }
  // Search for an attribute in vector of unique attributes
  [[nodiscard]] static bool isAttrubutePresent(const std::vector<XMLAttribute> &attributes, const std::string &attributeName)
  {
    return (std::find_if(attributes.rbegin(), attributes.rend(), [&attributeName](const XMLAttribute &attr) {
      return (attr.getName() == attributeName);
    }) != attributes.rend());
  }

private:
  // Attribute name and value
  std::string name;
  XMLValue value;
};
}// namespace XML_Lib