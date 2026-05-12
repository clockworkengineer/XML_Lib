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

  // ================================================================
  // Each macro declares both mutable and const overloads for a single
  // node-type event, keeping the pair in sync with one line of code.
  // ================================================================
#define IACTION_EVENT(name)                                                  \
  virtual void on##name([[maybe_unused]] Node &Node) { }                    \
  virtual void on##name([[maybe_unused]] const Node &Node) { }
  IACTION_EVENT(Node)
  IACTION_EVENT(CDATA)
  IACTION_EVENT(Comment)
  IACTION_EVENT(Content)
  IACTION_EVENT(Declaration)
  IACTION_EVENT(DTD)
  IACTION_EVENT(Element)
  IACTION_EVENT(EntityReference)
  IACTION_EVENT(PI)
  IACTION_EVENT(Prolog)
  IACTION_EVENT(Root)
  IACTION_EVENT(Self)
#undef IACTION_EVENT

};
}// namespace XML_Lib