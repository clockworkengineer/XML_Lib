#include "XPath_EvalHelpers.hpp"
#include "common/XML_QName.hpp"
#include "common/XML_NodeKindHelpers.hpp"

#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <sstream>

namespace XML_Lib {

/// @brief
/// Implementation of appendNodeStringValue.

void appendNodeStringValue(const Node &node, std::string &out)
{
  if (isA<Content>(node)) {
    out += NRef<Content>(node).getContents();
    return;
  }

  std::vector<const Node *> stack;
  stack.reserve(16);
  stack.push_back(&node);

  while (!stack.empty()) {
    const Node *current = stack.back();
    stack.pop_back();
    if (isA<Content>(*current)) {
      out += NRef<Content>(*current).getContents();
      continue;
    }
    const auto &children = current->getChildren();
    for (auto it = children.rbegin(); it != children.rend(); ++it) {
      stack.push_back(&*it);
    }
  }
}

/// @brief
/// Implementation of nodeStringValue.

std::string nodeStringValue(const Node &node)
{
  std::string result;
  result.reserve(64);
  appendNodeStringValue(node, result);
  return result;
}

/// @brief
/// Implementation of nodeNameView.

std::string_view nodeNameView(const Node &node)
{
  if (const auto *elem = asElementLike(node)) return elem->name();
  if (isA<PI>(node)) return NRef<PI>(node).name();
  return std::string_view{};
}

/// @brief
/// Implementation of nodeLocalNameView.

std::string_view nodeLocalNameView(const Node &node)
{
  return getLocalName(nodeNameView(node));
}

/// @brief
/// Implementation of matchNodeName.

bool matchNodeName(const Node &node, const std::string_view &nameTest)
{
  const std::string_view name = nodeNameView(node);
  if (name.empty()) { return false; }
  if (nameTest == "*") { return true; }
  return name == nameTest || nodeLocalNameView(node) == nameTest;
}

/// @brief
/// Implementation of stringToNumber.

double stringToNumber(std::string_view s)
{
  const auto trimStart = s.find_first_not_of(" \t\n\r\f\v");
  if (trimStart == std::string_view::npos) return std::numeric_limits<double>::quiet_NaN();
  s.remove_prefix(trimStart);
  const auto trimEnd = s.find_last_not_of(" \t\n\r\f\v");
  s.remove_suffix(s.size() - trimEnd - 1);
  if (s.empty()) return std::numeric_limits<double>::quiet_NaN();

  const std::string str(s);
  char *end = nullptr;
  errno = 0;
  const double result = std::strtod(str.c_str(), &end);
  if (end == str.c_str() + str.size()) return result;
  return std::numeric_limits<double>::quiet_NaN();
}

/// @brief
/// Implementation of resultToString.

std::string resultToString(const XPathResult &r)
{
  switch (r.type) {
  case XPathResultType::String:
    return r.stringValue;
  case XPathResultType::Number: {
    if (std::isnan(r.numberValue)) return "NaN";
    if (std::isinf(r.numberValue)) return (r.numberValue > 0) ? "Infinity" : "-Infinity";
    std::ostringstream os;
    os << r.numberValue;
    return os.str();
  }
  case XPathResultType::Boolean:
    return r.boolValue ? "true" : "false";
  case XPathResultType::NodeSet:
    if (r.nodeSet.empty()) return "";
    if (const auto it = r.attrValues.find(r.nodeSet.front()); it != r.attrValues.end()) return it->second;
    return nodeStringValue(*r.nodeSet.front());
  }
  return "";
}

/// @brief
/// Implementation of resultToStringView.

std::string_view resultToStringView(const XPathResult &r, std::string &scratch)
{
  switch (r.type) {
  case XPathResultType::String:
    return r.stringValue;
  case XPathResultType::Number:
  case XPathResultType::Boolean:
  case XPathResultType::NodeSet:
    scratch = resultToString(r);
    return scratch;
  }
  return scratch;
}

} // namespace XML_Lib