#pragma once

namespace XML_Lib {

struct Self final : Element
{
  // Constructors/Destructors
  explicit Self(const std::string &name="") : Element(name,Type::self) {}
  Self(const std::string &name,
    const std::vector<XMLAttribute> &attributes,
    const std::vector<XMLAttribute> &namespaces)
    : Element(name, attributes, namespaces, Type::self)
  {}
  Self(const Self &other) = delete;
  Self &operator=(const Self &other) = delete;
  Self(Self &&other) = default;
  Self &operator=(Self &&other) = default;
  ~Self() override = default;
};
}// namespace XML_Lib