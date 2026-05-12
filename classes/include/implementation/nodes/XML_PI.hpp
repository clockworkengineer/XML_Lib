#pragma once

namespace XML_Lib {

struct PI final : Variant
{
  // Constructors/Destructors
  PI(const std::string_view &name, const std::string_view &parameters)
    : Variant(Type::pi), piName(name), piParameters(parameters)
  {}
  XML_LIB_NO_COPY_MOVE_DTOR(PI);
  // Return reference to name
  [[nodiscard]] const std::string &name() const { return piName; }
  // Return reference to parameters
  [[nodiscard]] const std::string &parameters() const { return piParameters; }

private:
  std::string piName;
  std::string piParameters;
};
}// namespace XML_Lib