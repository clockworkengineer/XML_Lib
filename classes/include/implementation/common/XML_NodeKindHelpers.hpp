#pragma once

#include <vector>
#include <memory_resource>

namespace XML_Lib {
struct Node;
struct Element;
struct XMLAttribute;

/// Returns true if the node is an element-like node (Element, Root, or Self).
[[nodiscard]] bool isElementLikeNode(const Node &node);

/// Returns true if the node is a text content node (Content).
[[nodiscard]] bool isContentNode(const Node &node);

/// Return pointer to Element if node is element-like, otherwise nullptr.
[[nodiscard]] const Element *asElementLike(const Node &node);
[[nodiscard]] Element *asElementLike(Node &node);

/// Return pointer to element attributes vector if node is element-like, otherwise nullptr.
[[nodiscard]] const std::pmr::vector<XMLAttribute> *getNodeAttributes(const Node &node);

/// Return pointer to element namespaces vector if node is element-like, otherwise nullptr.
[[nodiscard]] const std::pmr::vector<XMLAttribute> *getNodeNamespaces(const Node &node);

} // namespace XML_Lib
