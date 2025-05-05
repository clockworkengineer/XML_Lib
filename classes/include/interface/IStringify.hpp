#pragma once

namespace XML_Lib {

// ====================
// Forward declarations
// ====================
class IDestination;
struct XNode;

// ==================================
// Interface for XML stringification
// ==================================
class IStringify
{
public:
  // ================
  // IStringify Error
  // ================
  struct Error final : std::runtime_error
  {
    explicit Error(const std::string_view &message) : std::runtime_error(std::string("IStringify Error: ").append(message)) {}
  };
  // ========================
  // Constructors/destructors
  // ========================
  virtual ~IStringify() = default;
  // ====================
  // Stringify XNode tree
  // ====================
  virtual void stringify(const XNode &xNode, IDestination &destination, unsigned long indent) const = 0;
  // =========================
  // Set/Get print indentation
  // =========================
  [[nodiscard]] virtual long getIndent() const { return 0; }
  virtual void setIndent([[maybe_unused]] long indent)  {}
};
// Make custom stringify to pass to XML constructor: The note pointer is tidied up internally.
template <typename T> IStringify *makeStringify() {
  return std::make_unique<T>().release();
}
}// namespace XML_Lib