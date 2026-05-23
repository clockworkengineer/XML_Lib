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

/// <summary>
/// Construct an XPath evaluator for the given document root.
/// </summary>
XPath::XPath(const Node &root) : implementation(std::make_unique<XPath_Impl>(root)) {}

/// <summary>
/// Destroy the XPath evaluator.
/// </summary>
XPath::~XPath() = default;

/// <summary>
/// Evaluate an XPath expression and return matching nodes.
/// </summary>
std::vector<const Node *> XPath::evaluate(const std::string_view expression) const
{
  return implementation->evaluate(expression);
}

/// <summary>
/// Evaluate an XPath expression and return the result as a string.
/// </summary>
std::string XPath::evaluateString(const std::string_view expression) const
{
  return implementation->evaluateString(expression);
}

/// <summary>
/// Evaluate an XPath expression and return the result as a boolean.
/// </summary>
bool XPath::evaluateBool(const std::string_view expression) const { return implementation->evaluateBool(expression); }

/// <summary>
/// Evaluate an XPath expression and return the result as a number.
/// </summary>
double XPath::evaluateNumber(const std::string_view expression) const
{
  return implementation->evaluateNumber(expression);
}

}// namespace XML_Lib
