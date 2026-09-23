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

const Element *asElementLike(const Node &node)
{
  if (isElementLikeNode(node)) {
    return &NRef<Element>(node);
  }
  return nullptr;
}

Element *asElementLike(Node &node)
{
  if (isElementLikeNode(node)) {
    return &NRef<Element>(node);
  }
  return nullptr;
}

const std::pmr::vector<XMLAttribute> *getNodeAttributes(const Node &node)
{
  if (const auto *elem = asElementLike(node)) {
    return &elem->getAttributes();
  }
  return nullptr;
}

const std::pmr::vector<XMLAttribute> *getNodeNamespaces(const Node &node)
{
  if (const auto *elem = asElementLike(node)) {
    return &elem->getNameSpaces();
  }
  return nullptr;
}

} // namespace XML_Lib
