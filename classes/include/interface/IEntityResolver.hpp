#pragma once

#include <optional>
#include <string>
#include <string_view>

namespace XML_Lib {

/// @brief Hook interface for custom external entity resolution.
///
/// Implement this interface and supply a pointer via `ParseOptions::entityResolver`
/// to control how SYSTEM/PUBLIC external entity references are fetched.
///
/// When `ParseOptions::allowExternalEntities` is `false` (the default) and no
/// `entityResolver` is provided, any external entity reference throws `SyntaxError`
/// (XXE defence).  When an `IEntityResolver` is supplied it is called instead; the
/// implementation may return `std::nullopt` to deny a specific entity (which also
/// throws `SyntaxError`).
///
/// Example:
/// @code
/// struct SafeResolver : XML_Lib::IEntityResolver {
///     std::optional<std::string>
///     resolve(const std::string_view &systemId, const std::string_view &) override {
///         if (systemId == "allowed.dtd") return loadFile(systemId);
///         return std::nullopt; // deny everything else
///     }
/// };
/// @endcode
class IEntityResolver
{
public:
  virtual ~IEntityResolver() = default;

  /// @brief Resolve an external entity reference.
  /// @param systemId  The SYSTEM identifier (file path or URI) of the entity.
  /// @param publicId  The PUBLIC identifier, or an empty string if absent.
  /// @return The replacement text for the entity, or `std::nullopt` to deny resolution
  ///         (which causes a `SyntaxError` to be thrown by the parser).
  [[nodiscard]] virtual std::optional<std::string>
    resolve(const std::string_view &systemId, const std::string_view &publicId) = 0;
};

}// namespace XML_Lib
