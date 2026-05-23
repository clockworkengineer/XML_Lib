//
// XML_NodeKindHelpes.cpp
//
// ption: Common node classification predicates shared across XML, XSD,
// DTD and XPath modules.
//
// Dependencies: C++20 - Language standard features used.
//

#include "XML_NodeKindHelpers.hpp"
#include "XML_Core.hpp"

namespace XML_Lib {

/// @brief
/// Return true if the node is element-like (Element, Root, or Self).

bool isElementLikeNode(const Node &node)
{
  return isA<Element>(node) || isA<Root>(node) || isA<Self>(node);
}

/// @brief
/// Return true if the node is a Content node.

bool isContentNode(const Node &node) { return isA<Content>(node); }

} // namespace XML_Lib
