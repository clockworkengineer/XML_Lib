//
// XPath_AxisHelpers.cpp
//
// Description: Helper utilities for XPath axis traversal — attribute access
// and element-like node classification for Element, Root, and Self nodes.
//
// Dependencies: C++20 - Language standard features used.
//

#include "XPath_AxisHelpers.hpp"

namespace XML_Lib {

bool isElementLikeNode(const Node &node)
{
  return isA<Element>(node) || isA<Root>(node) || isA<Self>(node);
}

const std::vector<XMLAttribute> *nodeAttributes(const Node &node)
{
  if (isA<Element>(node)) return &NRef<Element>(node).getAttributes();
  if (isA<Root>(node)) return &NRef<Root>(node).getAttributes();
  if (isA<Self>(node)) return &NRef<Self>(node).getAttributes();
  return nullptr;
}

std::string findAttributeValue(const Node &node, const std::string_view attrName)
{
  const auto *attrs = nodeAttributes(node);
  if (attrs == nullptr) return {};
  for (const auto &attr : *attrs) {
    if (attr.getName() == attrName) return attr.getParsed();
  }
  return {};
}

} // namespace XML_Lib
