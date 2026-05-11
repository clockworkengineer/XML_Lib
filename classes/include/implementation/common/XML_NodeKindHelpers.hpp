#pragma once

#include "XML_Core.hpp"

namespace XML_Lib {

/// Returns true if the node is an element-like node (Element, Root, or Self).
[[nodiscard]] bool isElementLikeNode(const Node &node);

/// Returns true if the node is a text content node (Content).
[[nodiscard]] bool isContentNode(const Node &node);

} // namespace XML_Lib
