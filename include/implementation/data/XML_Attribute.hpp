#pragma once

#include <string>

namespace XML_Lib {

struct XMLAttribute
{
  // Constructors/Destructors
  XMLAttribute(const std::string &name, const XMLValue &value) : name(name), value(value) {}
  XMLAttribute(const XMLAttribute &other) = default;
  XMLAttribute &operator=(const XMLAttribute &other) = default;
  XMLAttribute(XMLAttribute &&other) = default;
  XMLAttribute &operator=(XMLAttribute &&other) = default;
  ~XMLAttribute() = default;
  // Get attribute details
  const std::string &getName() const { return (name); }
  const std::string &getValue() const { return (value.getParsed()); }
  const std::string &getUnparsed() const { return (value.getUnparsed()); }

  static bool isAttrubutePresent(const std::vector<XMLAttribute> &attributes, const std::string & attributeName) {
    return(std::find_if(attributes.rbegin(),
          attributes.rend(),
          [&attributeName](const XMLAttribute &attr) { return (attr.getName() == attributeName); })
        != attributes.rend());
  }

private:
  // Attribute name and value
  std::string name;
  XMLValue value;
};
}// namespace XML_Lib