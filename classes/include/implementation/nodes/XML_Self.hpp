#pragma once

#include <span>

namespace XML_Lib {

struct Self final : Element
{
  // Constructors/Destructors
  explicit Self(const std::string_view &name="") : Element(name,Type::self) {}
  Self(const std::string_view &name,
    std::span<const XMLAttribute> attributes,
    std::span<const XMLAttribute> namespaces)
    : Element(name, attributes, namespaces, Type::self)
  {}
  XML_LIB_NO_COPY_MOVE_DTOR(Self);
};
}// namespace XML_Lib