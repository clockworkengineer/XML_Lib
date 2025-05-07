#pragma once

namespace XML_Lib {

struct Root final : Element
{
  // Constructors/Destructors
  explicit Root(const std::string_view &name="") : Element(name ,Type::root) {}
  Root(const std::string &name,
    const std::vector<XMLAttribute> &attributes,
    const std::vector<XMLAttribute> &namespaces)
    : Element(name, attributes, namespaces, Type::root)
  {}
  Root(const Root &other) = delete;
  Root &operator=(const Root &other) = delete;
  Root(Root &&other) = default;
  Root &operator=(Root &&other) = default;
  ~Root() override = default;
};
}// namespace XML_Lib