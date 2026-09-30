//
// Class: XPath
//
// Description: Thin public forwarder to XPath_Impl (Pimpl pattern) and XPathExpression.
//
// Dependencies: C++20 - Language standard features used.
//

#include "XPath_Impl.hpp"
#include "XPath.hpp"

namespace XML_Lib {

// ========================================================================
// XPathExpression implementation
// ========================================================================

XPathExpression::XPathExpression(const std::string_view expression)
  : exprString(expression), compiledAst(compileXPath(expression))
{}

XPathExpression::XPathExpression(const XPathExpression &) = default;
XPathExpression &XPathExpression::operator=(const XPathExpression &) = default;
XPathExpression::XPathExpression(XPathExpression &&) noexcept = default;
XPathExpression &XPathExpression::operator=(XPathExpression &&) noexcept = default;
XPathExpression::~XPathExpression() = default;

std::string_view XPathExpression::expression() const noexcept
{
  return exprString;
}

const std::shared_ptr<const XPathExpr> &XPathExpression::ast() const noexcept
{
  return compiledAst;
}

std::vector<const Node *> XPathExpression::evaluate(const Node &contextNode) const
{
  XPath xp(contextNode);
  return xp.evaluate(*this);
}

std::string XPathExpression::evaluateString(const Node &contextNode) const
{
  XPath xp(contextNode);
  return xp.evaluateString(*this);
}

bool XPathExpression::evaluateBool(const Node &contextNode) const
{
  XPath xp(contextNode);
  return xp.evaluateBool(*this);
}

double XPathExpression::evaluateNumber(const Node &contextNode) const
{
  XPath xp(contextNode);
  return xp.evaluateNumber(*this);
}

// ========================================================================
// XPath implementation
// ========================================================================

/// @brief
/// Construct an XPath evaluator for the given document root.

XPath::XPath(const Node &root) : implementation(std::make_unique<XPath_Impl>(root)) {}

/// @brief
/// Destroy the XPath evaluator.

XPath::~XPath() = default;

/// @brief
/// Evaluate an XPath expression and return matching nodes.

std::vector<const Node *> XPath::evaluate(const std::string_view expression) const
{
  return implementation->evaluate(expression);
}

/// @brief
/// Evaluate an XPath expression and return the result as a string.

std::string XPath::evaluateString(const std::string_view expression) const
{
  return implementation->evaluateString(expression);
}

/// @brief
/// Evaluate an XPath expression and return the result as a boolean.

bool XPath::evaluateBool(const std::string_view expression) const
{
  return implementation->evaluateBool(expression);
}

/// @brief
/// Evaluate an XPath expression and return the result as a number.

double XPath::evaluateNumber(const std::string_view expression) const
{
  return implementation->evaluateNumber(expression);
}

std::vector<const Node *> XPath::evaluate(const XPathExpression &compiled) const
{
  return implementation->evaluate(*compiled.ast());
}

std::string XPath::evaluateString(const XPathExpression &compiled) const
{
  return implementation->evaluateString(*compiled.ast());
}

bool XPath::evaluateBool(const XPathExpression &compiled) const
{
  return implementation->evaluateBool(*compiled.ast());
}

double XPath::evaluateNumber(const XPathExpression &compiled) const
{
  return implementation->evaluateNumber(*compiled.ast());
}

}// namespace XML_Lib
