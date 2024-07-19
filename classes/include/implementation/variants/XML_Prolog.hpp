#pragma once

namespace XML_Lib {

struct Prolog final : Variant
{
  // Constructors/Destructors
  Prolog() : Variant(Type::prolog) {}
  Prolog(const Prolog &other) = delete;
  Prolog &operator=(const Prolog &other) = delete;
  Prolog(Prolog &&other) = default;
  Prolog &operator=(Prolog &&other) = default;
  ~Prolog() override = default;
};
}// namespace XML_Lib