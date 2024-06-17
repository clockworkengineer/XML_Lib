#pragma once

#include <string>
#include <utility>

namespace XML_Lib {

struct XPI final : Variant
{
  // Constructors/Destructors
  XPI(std::string name, const std::string &parameters)
    : Variant(Type::pi), piName(std::move(name)), piParameters(parameters)
  {}
  XPI(const XPI &other) = delete;
  XPI &operator=(const XPI &other) = delete;
  XPI(XPI &&other) = default;
  XPI &operator=(XPI &&other) = default;
  ~XPI() override = default;
  // Return reference to name
  [[nodiscard]] const std::string &name() const { return piName; }
  // Return reference to parameters
  [[nodiscard]] const std::string &parameters() const { return piParameters; }

private:
  std::string piName;
  std::string piParameters;
};
}// namespace XML_Lib