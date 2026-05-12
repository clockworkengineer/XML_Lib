
#pragma once

#include <stdexcept>
#include <string_view>

namespace XML_Lib {

// ====================
// Forward declarations
// ====================

class ISource;
struct Node;

/// @brief Abstract interface for an XML parser.
///
/// The default implementation is the built-in recursive-descent parser.
/// A custom parser can be injected via the `XML(IStringify*, IParser*)` constructor.
class IParser
{
public:
  /// @brief Exception thrown when parsing fails.
  struct Error final : std::runtime_error
  {
    explicit Error(const std::string_view &message) : std::runtime_error(std::string("IParser Error: ").append(message)) {}
  };

  virtual ~IParser() = default;

  /// @brief Parse @p source and return the document root `Node`.
  virtual Node parse(ISource &source) = 0;

  /// @brief Return `true` if this parser supports validation (DTD/XSD).
  virtual bool canValidate() = 0;

  /// @brief Validate the document rooted at @p prolog.
  virtual void validate(Node &prolog) = 0;
};
}// namespace XML_Lib
