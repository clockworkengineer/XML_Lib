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

/// <summary>
/// Return true if the node is element-like (Element, Root, or Self).
/// </summary>
bool isElementLikeNode(const Node &node)
{
  return isA<Element>(node) || isA<Root>(node) || isA<Self>(node);
}

/// <summary>
/// Return true if the node is a Content node.
/// </summary>
bool isContentNode(const Node &node) { return isA<Content>(node); }

} // namespace XML_Lib
