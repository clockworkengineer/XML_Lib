#pragma once

#include <memory>
#include <stdexcept>
#include <string_view>

namespace XML_Lib {

// ====================
// Forward declarations
// ====================
class IDestination;
struct Node;

/// @brief Abstract interface for XML serialisation (stringification).
///
/// The default implementation produces compact or indented XML text.
/// A custom serialiser can be injected via the `XML(IStringify*, IParser*)` constructor,
/// or created with the `makeStringify<T>()` helper.
class IStringify
{
public:
  /// @brief Exception thrown when serialisation fails.
  struct Error final : std::runtime_error
  {
    explicit Error(const std::string_view &message) : std::runtime_error(std::string("IStringify Error: ").append(message)) {}
  };

  virtual ~IStringify() = default;

  /// @brief Serialise the subtree rooted at @p xNode into @p destination.
  /// @param xNode       Root of the subtree to serialise.
  /// @param destination Output stream to write to.
  /// @param indent      Current indentation depth (in spaces).
  virtual void stringify(const Node &xNode, IDestination &destination, unsigned long indent) const = 0;
};

/// @brief Segregated role interface for stringifiers supporting indentation configuration.
class IIndentedStringify : public IStringify
{
public:
  ~IIndentedStringify() noexcept override = default;

  [[nodiscard]] virtual long getIndent() const = 0;
  virtual void setIndent(long indent) = 0;
};

/// @brief Factory helper — allocates a concrete `IStringify` implementation on the heap.
///
/// The returned raw pointer is adopted (and freed) by the `XML` constructor.
/// @tparam T A concrete `IStringify` subclass.
template <typename T> IStringify *makeStringify() {
  return std::make_unique<T>().release();
}

} // namespace XML_Lib