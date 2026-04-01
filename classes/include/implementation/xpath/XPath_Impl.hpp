#pragma once

#include "XML.hpp"
#include "XML_Core.hpp"
#include "XPath_AST.hpp"

namespace XML_Lib {

// -------------------------------------------------------
// XPath result type
// -------------------------------------------------------
enum class XPathResultType : uint8_t { NodeSet, String, Number, Boolean };

struct XPathResult
{
  XPathResultType type{ XPathResultType::NodeSet };
  std::vector<const Node *> nodeSet;
  // For attribute-axis results: maps node ptr → attribute value.
  // When non-empty, nodeSet members are "attribute proxy" nodes whose string-value
  // must be looked up here rather than from nodeStringValue().
  std::unordered_map<const Node *, std::string> attrValues;
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
