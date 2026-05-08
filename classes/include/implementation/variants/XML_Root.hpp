#pragma once

namespace XML_Lib {

struct Root final : Element
{
  // Constructors/Destructors
  explicit Root(const std::string_view &name="") : Element(name ,Type::root) {}
  Root(const std::string_view &name,
    const std::vector<XMLAttribute> &attributes,
    const std::vector<XMLAttribute> &namespaces)
    : Element(name, attributes, namespaces, Type::root)
  {}
  XML_LIB_NO_COPY_MOVE_DTOR(Root);
};
}// namespace XML_Lib