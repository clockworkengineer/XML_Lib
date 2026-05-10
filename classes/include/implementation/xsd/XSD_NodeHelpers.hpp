#pragma once

#include "XML_Core.hpp"

#include <functional>
#include <string_view>
#include <vector>

namespace XML_Lib {

struct XSD_NodeView
{
  std::reference_wrapper<const Node> node;
  std::string_view tag;
};

[[nodiscard]] std::string_view localTagView(const Node &node);
[[nodiscard]] std::string_view attrValueView(const Node &node, const std::string_view &attrName);
[[nodiscard]] std::vector<std::reference_wrapper<const Node>> childElementRefs(const Node &node);
[[nodiscard]] std::vector<XSD_NodeView> childElementViews(const Node &node);

} // namespace XML_Lib
