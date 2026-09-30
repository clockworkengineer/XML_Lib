#pragma once

#if defined(XML_LIB_ENABLE_XPATH)
 
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace XML_Lib {

// ====================
// Forward declarations
// ====================
class XPath_Impl;
struct XPathExpr;
struct Node;

/// @brief Pre-compiled, immutable, thread-safe XPath 1.0 expression.
///
/// Compiles and caches the parsed XPath AST once. Can be evaluated repeatedly across
/// multiple `Node` trees or `XML` documents without re-lexing or re-parsing overhead.
class XPathExpression
{
public:
  explicit XPathExpression(std::string_view expression);
  XPathExpression(const XPathExpression &);
  XPathExpression &operator=(const XPathExpression &);
  XPathExpression(XPathExpression &&) noexcept;
  XPathExpression &operator=(XPathExpression &&) noexcept;
  ~XPathExpression();

  /// @brief Original expression string.
  [[nodiscard]] std::string_view expression() const noexcept;

  /// @brief Evaluate compiled expression against @p contextNode and return matching nodes.
  [[nodiscard]] std::vector<const Node *> evaluate(const Node &contextNode) const;

  /// @brief Evaluate compiled expression and convert the result to a string.
  [[nodiscard]] std::string evaluateString(const Node &contextNode) const;

  /// @brief Evaluate compiled expression and convert the result to a boolean.
  [[nodiscard]] bool evaluateBool(const Node &contextNode) const;

  /// @brief Evaluate compiled expression and convert the result to a number.
  [[nodiscard]] double evaluateNumber(const Node &contextNode) const;

  /// @brief Internal compiled AST accessor.
  [[nodiscard]] const std::shared_ptr<const XPathExpr> &ast() const noexcept;

private:
  std::string exprString;
  std::shared_ptr<const XPathExpr> compiledAst;
};

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

  /// @brief Evaluate a pre-compiled @p expression and return all matching nodes.
  [[nodiscard]] std::vector<const Node *> evaluate(const XPathExpression &compiled) const;

  /// @brief Evaluate a pre-compiled @p expression and convert the result to a string.
  [[nodiscard]] std::string evaluateString(const XPathExpression &compiled) const;

  /// @brief Evaluate a pre-compiled @p expression and convert the result to a boolean.
  [[nodiscard]] bool evaluateBool(const XPathExpression &compiled) const;

  /// @brief Evaluate a pre-compiled @p expression and convert the result to a number.
  [[nodiscard]] double evaluateNumber(const XPathExpression &compiled) const;

private:
  const std::unique_ptr<XPath_Impl> implementation;
};

}// namespace XML_Lib

#endif// XML_LIB_ENABLE_XPATH
