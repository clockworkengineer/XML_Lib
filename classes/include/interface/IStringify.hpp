#pragma once

namespace XML_Lib {

// ====================
// Forward declarations
// ====================
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
    explicit Error(const std::string &message) : std::runtime_error("IStringify Error: " + message) {}
  };
  // ========================
  // Constructors/destructors
  // ========================
  virtual ~IStringify() = default;
  // ====================
  // Stringify XNode tree
  // ====================
  virtual void stringify(const XNode &XNode, IDestination &destination) const = 0;
};
// Make custom stringify to pass to XML constructor: Note pointer is tidied up internally.
template <typename T> [[maybe_unused]] IStringify *makeStringify() {
  return std::make_unique<T>().release();
}
}// namespace XML_Lib