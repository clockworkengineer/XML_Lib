#pragma once

#include <memory>
#include <string>
#include <string_view>

namespace XML_Lib {

class IValidator;

/// @brief Pluggable validator registry interface allowing open extension (OCP) for custom validators.
class IValidatorRegistry
{
public:
  virtual ~IValidatorRegistry() noexcept = default;

  /// @brief Register a validator for a given schema system identifier (e.g. "DTD", "XSD").
  virtual void registerValidator(const std::string_view &schemaType, std::unique_ptr<IValidator> validator) = 0;

  /// @brief Fetch registered validator by schema system identifier.
  [[nodiscard]] virtual IValidator *getValidator(const std::string_view &schemaType) const = 0;
};

} // namespace XML_Lib
