#pragma once

#include <string>
#include <algorithm>

namespace XML_Lib {

struct XRoot final : XElement
{
  // Constructors/Destructors
  explicit XRoot(const std::string &name="") : XElement(name ,Type::root) {}
  XRoot(const std::string &name,
    const std::vector<XMLAttribute> &attributes,
    const std::vector<XMLAttribute> &namespaces)
    : XElement(name, attributes, namespaces, Type::root)
  {}
  XRoot(const XRoot &other) = delete;
  XRoot &operator=(const XRoot &other) = delete;
  XRoot(XRoot &&other) = default;
  XRoot &operator=(XRoot &&other) = default;
  ~XRoot() override = default;
};
}// namespace XML_Lib