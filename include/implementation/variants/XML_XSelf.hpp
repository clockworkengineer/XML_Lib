#pragma once

#include <string>
#include <algorithm>

namespace XML_Lib {

struct XSelf final : XElement
{
  // Constructors/Destructors
  XSelf() : XElement(Variant::Type::self) {}
  XSelf(const std::string &name,
    const std::vector<XMLAttribute> &attributes,
    const std::vector<XMLAttribute> &namespaces)
    : XElement(name, attributes, namespaces, Variant::Type::self)
  {}
  XSelf(const XSelf &other) = delete;
  XSelf &operator=(const XSelf &other) = delete;
  XSelf(XSelf &&other) = default;
  XSelf &operator=(XSelf &&other) = default;
  ~XSelf() override = default;
};
}// namespace XML_Lib