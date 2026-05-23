//
// Class: XPath
//
// Description: Thin public forwarder to XPath_Impl (Pimpl pattern).
//
// Dependencies: C++20 - Language standard features used.
//

#include "XPath_Impl.hpp"
#include "XPath.hpp"

namespace XML_Lib {

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

bool XPath::evaluateBool(const std::string_view expression) const { return implementation->evaluateBool(expression); }

/// @brief
/// Evaluate an XPath expression and return the result as a number.

double XPath::evaluateNumber(const std::string_view expression) const
{
  return implementation->evaluateNumber(expression);
}

}// namespace XML_Lib
