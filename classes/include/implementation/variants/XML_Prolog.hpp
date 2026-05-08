#pragma once

namespace XML_Lib {

struct Prolog final : Variant
{
  // Constructors/Destructors
  Prolog() : Variant(Type::prolog) {}
  XML_LIB_NO_COPY_MOVE_DTOR(Prolog);
};
}// namespace XML_Lib