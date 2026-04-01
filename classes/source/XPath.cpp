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

XPath::XPath(const Node &root) : implementation(std::make_unique<XPath_Impl>(root)) {}

XPath::~XPath() = default;

std::vector<const Node *> XPath::evaluate(const std::string_view expression) const
{
  return implementation->evaluate(expression);
}

std::string XPath::evaluateString(const std::string_view expression) const
{
  return implementation->evaluateString(expression);
}

bool XPath::evaluateBool(const std::string_view expression) const { return implementation->evaluateBool(expression); }

double XPath::evaluateNumber(const std::string_view expression) const
{
  return implementation->evaluateNumber(expression);
}

}// namespace XML_Lib
