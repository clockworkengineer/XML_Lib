#pragma once

namespace XML_Lib {

struct PI final : Variant
{
  // Constructors/Destructors
  PI(std::string name, std::string parameters)
    : Variant(Type::pi), piName(std::move(name)), piParameters(std::move(parameters))
  {}
  PI(const PI &other) = delete;
  PI &operator=(const PI &other) = delete;
  PI(PI &&other) = default;
  PI &operator=(PI &&other) = default;
  ~PI() override = default;
  // Return reference to name
  [[nodiscard]] const std::string &name() const { return piName; }
  // Return reference to parameters
  [[nodiscard]] const std::string &parameters() const { return piParameters; }

private:
  std::string piName;
  std::string piParameters;
};
}// namespace XML_Lib