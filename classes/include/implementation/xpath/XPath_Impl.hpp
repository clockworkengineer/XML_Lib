#pragma once

#include "XML.hpp"
#include "XML_Core.hpp"
#include "XPath_AST.hpp"

#include <unordered_map>
#include <unordered_set>

namespace XML_Lib {

// -------------------------------------------------------
// XPath result type
// -------------------------------------------------------
enum class XPathResultType : uint8_t { NodeSet, String, Number, Boolean };

struct XPathResult
{
  XPathResultType type{ XPathResultType::NodeSet };
  std::vector<const Node *> nodeSet;
  // For attribute-axis and namespace-axis results: maps proxy node ptr → value.
  // When non-empty, nodeSet members are "proxy" nodes whose string-value
  // is looked up here rather than from nodeStringValue().
  std::unordered_map<const Node *, std::string> attrValues;
  std::unordered_set<const Node *> namespaceNodes;
  std::unordered_map<const Node *, std::string> nodeNames;
  std::string stringValue;
  double numberValue{ 0.0 };
  bool boolValue{ false };
};

// -------------------------------------------------------
// Pimpl class
// -------------------------------------------------------
class XPath_Impl
{
public:
  explicit XPath_Impl(const Node &root);
  XPath_Impl(const XPath_Impl &) = delete;
  XPath_Impl &operator=(const XPath_Impl &) = delete;
  XPath_Impl(XPath_Impl &&) = delete;
  XPath_Impl &operator=(XPath_Impl &&) = delete;
  ~XPath_Impl() = default;

  [[nodiscard]] std::vector<const Node *> evaluate(std::string_view expression) const;
  [[nodiscard]] std::string evaluateString(std::string_view expression) const;
  [[nodiscard]] bool evaluateBool(std::string_view expression) const;
  [[nodiscard]] double evaluateNumber(std::string_view expression) const;

private:
  const Node &xmlRoot;
};

}// namespace XML_Lib
