#pragma once

#include <stdexcept>
#include <string_view>

namespace XML_Lib {

// ====================
// Forward declarations
// ====================
struct Node;

/// @brief Visitor interface for tree traversal events.
///
/// Derive from `IAction` and override the `on*` methods you care about, then pass
/// an instance to `XML::traverse()`.  Each node type generates a matching callback
/// with both mutable and const overloads.
///
/// Example:
/// @code
/// struct MyAction : XML_Lib::IAction {
///     void onElement(const XML_Lib::Node &node) override { /* … */ }
/// };
/// @endcode
class IAction
{
public:
  /// @brief Exception thrown when an action encounters an error during traversal.
  struct Error final : std::runtime_error
  {
    explicit Error(const std::string_view &message) : std::runtime_error(std::string("IAction Error: ").append(message)) {}
  };

  virtual ~IAction() = default;

  virtual void onNode([[maybe_unused]] Node &Node) {}
  virtual void onNode([[maybe_unused]] const Node &Node) {}
  virtual void onCDATA([[maybe_unused]] Node &Node) {}
  virtual void onCDATA([[maybe_unused]] const Node &Node) {}
  virtual void onComment([[maybe_unused]] Node &Node) {}
  virtual void onComment([[maybe_unused]] const Node &Node) {}
  virtual void onContent([[maybe_unused]] Node &Node) {}
  virtual void onContent([[maybe_unused]] const Node &Node) {}
  virtual void onDeclaration([[maybe_unused]] Node &Node) {}
  virtual void onDeclaration([[maybe_unused]] const Node &Node) {}
  virtual void onDTD([[maybe_unused]] Node &Node) {}
  virtual void onDTD([[maybe_unused]] const Node &Node) {}
  virtual void onElement([[maybe_unused]] Node &Node) {}
  virtual void onElement([[maybe_unused]] const Node &Node) {}
  virtual void onEntityReference([[maybe_unused]] Node &Node) {}
  virtual void onEntityReference([[maybe_unused]] const Node &Node) {}
  virtual void onPI([[maybe_unused]] Node &Node) {}
  virtual void onPI([[maybe_unused]] const Node &Node) {}
  virtual void onProlog([[maybe_unused]] Node &Node) {}
  virtual void onProlog([[maybe_unused]] const Node &Node) {}
  virtual void onRoot([[maybe_unused]] Node &Node) {}
  virtual void onRoot([[maybe_unused]] const Node &Node) {}
  virtual void onSelf([[maybe_unused]] Node &Node) {}
  virtual void onSelf([[maybe_unused]] const Node &Node) {}

};
}// namespace XML_Lib