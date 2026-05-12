#pragma once

#include <span>

namespace XML_Lib {

struct Root final : Element
{
  // Constructors/Destructors
  explicit Root(const std::string_view &name="") : Element(name ,Type::root) {}
  Root(const std::string_view &name,
    std::span<const XMLAttribute> attributes,
    std::span<const XMLAttribute> namespaces)
    : Element(name, attributes, namespaces, Type::root)
  {}
  XML_LIB_NO_COPY_MOVE_DTOR(Root);
};
}// namespace XML_Lib