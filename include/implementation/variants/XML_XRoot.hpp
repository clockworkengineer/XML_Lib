#pragma once

#include <string>
#include <algorithm>

namespace XML_Lib {

struct XRoot : XElement
{
  // Constructors/Destructors
  XRoot() : XElement(Variant::Type::root) {}
  XRoot(const std::string &name,
    const std::vector<XMLAttribute> &attributes,
    const std::vector<XMLAttribute> &namespaces)
    : XElement(name, attributes, namespaces, Variant::Type::root)
  {}
  XRoot(const XRoot &other) = delete;
  XRoot &operator=(const XRoot &other) = delete;
  XRoot(XRoot &&other) = default;
  XRoot &operator=(XRoot &&other) = default;
  ~XRoot() = default;
};
}// namespace XML_Lib