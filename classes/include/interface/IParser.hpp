#pragma once

#include <stdexcept>
#include <string_view>

namespace XML_Lib {

// ====================
// Forward declarations
// ====================
class ISource;
struct Node;
struct ParseOptions;

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

  virtual ~IParser() noexcept = default;

  /// @brief Parse @p source and return the document root `Node`.
  virtual Node parse(ISource &source, const ParseOptions &options) = 0;

  /// @brief Return `true` if this parser supports validation (DTD/XSD).
  virtual bool canValidate() { return false; }

  /// @brief Validate the document rooted at @p prolog.
  virtual void validate([[maybe_unused]] Node &prolog) {}
};

/// @brief Segregated role interface for parsers capable of validation (DTD/XSD).
class IValidatingParser : public IParser
{
public:
  ~IValidatingParser() noexcept override = default;

  bool canValidate() override { return true; }
  void validate(Node &prolog) override = 0;
};

} // namespace XML_Lib
