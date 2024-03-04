#pragma once

#include <string>

namespace XML_Lib {

struct XPI : Variant
{
  // Constructors/Destructors
  XPI(const std::string &name, const std::string &parameters)
    : Variant(Variant::Type::pi), piName(name), piParameters(parameters)
  {}
  XPI(const XPI &other) = delete;
  XPI &operator=(const XPI &other) = delete;
  XPI(XPI &&other) = default;
  XPI &operator=(XPI &&other) = default;
  ~XPI() = default;
  // Return reference to name
  [[nodiscard]] const std::string &name() const { return (piName); }
  // Return reference to parameters
  [[nodiscard]] const std::string &parameters() const { return (piParameters); }

private:
  std::string piName;
  std::string piParameters;
};
}// namespace XML_Lib