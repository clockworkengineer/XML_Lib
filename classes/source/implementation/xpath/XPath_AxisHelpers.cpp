//
// XPath_AxisHelpers.cpp
//
// Description: Helpe utilities for XPath axis traversal — attribute access
// and element-like node classification for Element, Root, and Self nodes.
//
// Dependencies: C++20 - Language standard features used.
//

#include "XPath_AxisHelpers.hpp"
#include "common/XML_NodeKindHelpers.hpp"

namespace XML_Lib {

const std::pmr::vector<XMLAttribute> *nodeAttributes(const Node &node)
{
  return getNodeAttributes(node);
}

const std::pmr::vector<XMLAttribute> *nodeNameSpaces(const Node &node)
{
  return getNodeNamespaces(node);
}

/// @brief
/// Implementation of findAttributeValue.

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