#pragma once

namespace XML_Lib {

struct Self final : Element
{
  // Constructors/Destructors
  explicit Self(const std::string_view &name="") : Element(name,Type::self) {}
  Self(const std::string_view &name,
    const std::vector<XMLAttribute> &attributes,
    const std::vector<XMLAttribute> &namespaces)
    : Element(name, attributes, namespaces, Type::self)
  {}
  XML_LIB_NO_COPY_MOVE_DTOR(Self);
};
}// namespace XML_Lib