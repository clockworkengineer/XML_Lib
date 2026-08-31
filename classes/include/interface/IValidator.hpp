#pragma once

#include "ISchemaValidator.hpp"
#include <stdexcept>
#include <string_view>

namespace XML_Lib {

class ISource;
class IDestination;
struct Node;

/// @brief Composite interface for XML schema validators (DTD, XSD, RNG, etc.).
class IValidator : public ISchemaValidator
{
public:
  /// @brief Exception thrown when validation or schema parsing fails.
  struct Error final : std::runtime_error
  {
    explicit Error(const std::string_view &message) : std::runtime_error(std::string("IValidator Error: ").append(message)) {}
  };

  ~IValidator() noexcept override = default;

  /// @brief Parse schema grammar from source input.
  virtual void parse(ISource &source) = 0;

  /// @brief Stringify schema definition to text destination.
  virtual void stringify(IDestination &destination) = 0;

  /// @brief Validate XML document tree against schema rules.
  virtual void validate(const Node &xNode) = 0;

  /// @brief ISchemaValidator role interface implementation.
  void validateDocument(const Node &rootNode) override { validate(rootNode); }
};

} // namespace XML_Lib