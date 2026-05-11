#pragma once

#include "XPath_Impl.hpp"
#include "XML_NodeKindHelpers.hpp"

#include <string>
#include <string_view>
#include <vector>

namespace XML_Lib {

// isElementLikeNode() is provided by XML_NodeKindHelpers.hpp

/// Returns a pointer to the attribute list for an element-like node, or nullptr
/// if the node is not element-like.
[[nodiscard]] const std::vector<XMLAttribute> *nodeAttributes(const Node &node);

/// Returns the parsed value of the named attribute on an element-like node, or
/// an empty string if the attribute is not present.
[[nodiscard]] std::string findAttributeValue(const Node &node, std::string_view attrName);

} // namespace XML_Lib
