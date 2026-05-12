#pragma once

#if defined(XML_LIB_ENABLE_XPATH)

namespace XML_Lib {

// ====================
// Forward declarations
// ====================
class XPath_Impl;
struct Node;

/// @brief XPath 1.0 evaluator.
///
/// Evaluates XPath expressions against a parsed XML document tree.
/// Construct with a reference to the root `Node` of the document.
///
/// @note Copying and moving are disabled.
class XPath
{
public:
  /// @brief Exception thrown when an XPath expression is malformed or evaluation fails.
  struct Error final : std::runtime_error
  {
    explicit Error(const std::string_view &message) : std::runtime_error(std::string("XPath Error: ").append(message))
    {}
  };

  /// @brief Construct an XPath evaluator bound to @p root.
  /// @param root The root `Node` of the parsed XML document.
  explicit XPath(const Node &root);
  XPath() = delete;
  XPath(const XPath &) = delete;
  XPath &operator=(const XPath &) = delete;
  XPath(XPath &&) = delete;
  XPath &operator=(XPath &&) = delete;
  ~XPath();

  /// @brief Evaluate @p expression and return all matching nodes.
  /// @param expression XPath 1.0 expression string.
  /// @return Pointers into the existing node tree — valid only while the owning `XML` object is alive.
  [[nodiscard]] std::vector<const Node *> evaluate(std::string_view expression) const;

  /// @brief Evaluate @p expression and convert the result to a string (XPath `string()` semantics).
  [[nodiscard]] std::string evaluateString(std::string_view expression) const;

  /// @brief Evaluate @p expression and convert the result to a boolean (XPath `boolean()` semantics).
  [[nodiscard]] bool evaluateBool(std::string_view expression) const;

  /// @brief Evaluate @p expression and convert the result to a number (XPath `number()` semantics).
  [[nodiscard]] double evaluateNumber(std::string_view expression) const;

private:
  const std::unique_ptr<XPath_Impl> implementation;
};

}// namespace XML_Lib

#endif// XML_LIB_ENABLE_XPATH
