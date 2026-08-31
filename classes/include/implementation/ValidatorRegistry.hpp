#pragma once

#include "interface/IValidatorRegistry.hpp"
#include "interface/IValidator.hpp"
#include <unordered_map>
#include <string>
#include <memory>

namespace XML_Lib {

/// Transparent hash/equality functors for std::string_view lookups in ValidatorRegistry.
struct ValidatorMapHash {
  using is_transparent = void;
  [[nodiscard]] size_t operator()(const std::string_view &sv) const noexcept { return std::hash<std::string_view>{}(sv); }
};

struct ValidatorMapEq {
  using is_transparent = void;
  [[nodiscard]] bool operator()(const std::string_view &lhs, const std::string_view &rhs) const noexcept { return lhs == rhs; }
};

/// @brief Concrete pluggable validator registry implementation (OCP & DIP compliant).
class ValidatorRegistry final : public IValidatorRegistry
{
public:
  ValidatorRegistry() = default;
  ~ValidatorRegistry() noexcept override = default;

  void registerValidator(const std::string_view &schemaType, std::unique_ptr<IValidator> validator) override
  {
    validators[std::string(schemaType)] = std::move(validator);
  }

  [[nodiscard]] IValidator *getValidator(const std::string_view &schemaType) const override
  {
    if (const auto it = validators.find(schemaType); it != validators.end()) {
      return it->second.get();
    }
    return nullptr;
  }

private:
  std::unordered_map<std::string, std::unique_ptr<IValidator>, ValidatorMapHash, ValidatorMapEq> validators;
};

} // namespace XML_Lib
