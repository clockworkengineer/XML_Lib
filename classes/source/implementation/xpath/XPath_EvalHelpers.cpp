#include "XPath_EvalHelpers.hpp"

#include <charconv>
#include <cmath>
#include <limits>
#include <sstream>

namespace XML_Lib {

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

std::string nodeStringValue(const Node &node)
{
  std::string result;
  result.reserve(64);
  appendNodeStringValue(node, result);
  return result;
}

std::string_view nodeNameView(const Node &node)
{
  if (isA<Element>(node)) return NRef<Element>(node).name();
  if (isA<Root>(node)) return NRef<Root>(node).name();
  if (isA<Self>(node)) return NRef<Self>(node).name();
  if (isA<PI>(node)) return NRef<PI>(node).name();
  return std::string_view{};
}

std::string_view nodeLocalNameView(const Node &node)
{
  const std::string_view nm = nodeNameView(node);
  const auto pos = nm.find(':');
  return (pos != std::string_view::npos) ? nm.substr(pos + 1) : nm;
}

bool matchNodeName(const Node &node, const std::string_view &nameTest)
{
  const std::string_view name = nodeNameView(node);
  if (name.empty()) { return false; }
  if (nameTest == "*") { return true; }
  return name == nameTest || nodeLocalNameView(node) == nameTest;
}

double stringToNumber(std::string_view s)
{
  const auto trimStart = s.find_first_not_of(" \t\n\r\f\v");
  if (trimStart == std::string_view::npos) return std::numeric_limits<double>::quiet_NaN();
  s.remove_prefix(trimStart);
  const auto trimEnd = s.find_last_not_of(" \t\n\r\f\v");
  s.remove_suffix(s.size() - trimEnd - 1);

  double result = 0.0;
  const auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), result);
  if (ec == std::errc() && ptr == s.data() + s.size()) return result;
  return std::numeric_limits<double>::quiet_NaN();
}

std::string resultToString(const XPathResult &r)
{
  switch (r.type) {
  case XPathResultType::String:
    return r.stringValue;
  case XPathResultType::Number: {
    if (std::isnan(r.numberValue)) return "NaN";
    if (std::isinf(r.numberValue)) return (r.numberValue > 0) ? "Infinity" : "-Infinity";
    char buffer[64];
    const auto [ptr, ec] = std::to_chars(buffer, buffer + sizeof(buffer), r.numberValue);
    if (ec == std::errc()) return std::string(buffer, ptr);
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
