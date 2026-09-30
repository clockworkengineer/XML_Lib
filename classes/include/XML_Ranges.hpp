#pragma once

#include "XML_Core.hpp"
#include "node/XML_Node.hpp"
#include "nodes/XML_Element.hpp"
#include "nodes/XML_Root.hpp"
#include "nodes/XML_Self.hpp"
#include "node/XML_Node_Reference.hpp"

#include <ranges>
#include <string_view>
#include <span>

namespace XML_Lib {

/// @brief Helper to test if a Node is an element-kind variant (Element, Root, or Self).
inline bool isElementNode(const Node &node) noexcept
{
  if (node.isEmpty()) { return false; }
  const auto type = node.getVariant().getNodeType();
  return type == Variant::Type::element || type == Variant::Type::root || type == Variant::Type::self;
}

/// @brief Return a range view yielding only the child Element nodes of @p parentNode.
inline auto childElements(const Node &parentNode)
{
  return parentNode.getChildren() | std::views::filter([](const Node &child) {
    return isElementNode(child);
  });
}

/// @brief Return a range view yielding only the child Element nodes of @p parentNode matching @p tagName.
inline auto childElements(const Node &parentNode, std::string_view tagName)
{
  return parentNode.getChildren() | std::views::filter([tagName](const Node &child) {
    return isElementNode(child) && NRef<Element>(child).name() == tagName;
  });
}

/// @brief Return a range view yielding all XML attributes of @p elementNode.
inline auto elementAttributes(const Node &elementNode)
{
  return std::span(NRef<Element>(elementNode).getAttributes());
}

inline auto Node::elements() const { return childElements(*this); }
inline auto Node::elements(std::string_view name) const { return childElements(*this, name); }

} // namespace XML_Lib
