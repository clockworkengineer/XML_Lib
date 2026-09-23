#include "xsd/XSD_NodeHelpers.hpp"
#include "XML_Core.hpp"
#include "common/XML_NodeKindHelpers.hpp"
#include "common/XML_QName.hpp"

namespace XML_Lib {

/// @brief
/// Implementation of localTagView.

std::string_view localTagView(const Node &node)
{
  return getLocalName(NRef<Element>(node).name());
}

/// @brief
/// Implementation of attrValueView.

std::string_view attrValueView(const Node &node, const std::string_view &attrName)
{
  const auto &elem = NRef<Element>(node);
  if (elem.hasAttribute(attrName)) { return elem[attrName].getParsed(); }
  return std::string_view{};
}

/// @brief
/// Implementation of childElementRefs.

std::vector<std::reference_wrapper<const Node>> childElementRefs(const Node &node)
{
  const auto &children = node.getChildren();
  size_t count = 0;
  for (const auto &child : children) {
    if (isElementLikeNode(child)) { ++count; }
  }

  std::vector<std::reference_wrapper<const Node>> result;
  result.reserve(count);
  for (const auto &child : children) {
    if (isElementLikeNode(child)) { result.emplace_back(child); }
  }
  return result;
}

/// @brief
/// Implementation of childElementViews.

std::vector<XSD_NodeView> childElementViews(const Node &node)
{
  auto refs = childElementRefs(node);
  std::vector<XSD_NodeView> views;
  views.reserve(refs.size());
  for (const auto &ref : refs) {
    const auto &child = ref.get();
    views.push_back(XSD_NodeView{ child, localTagView(child) });
  }
  return views;
}

} // namespace XML_Lib