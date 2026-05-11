#include "xsd/XSD_NodeHelpers.hpp"
#include "XML_NodeKindHelpers.hpp"

namespace XML_Lib {

std::string_view localTagView(const Node &node)
{
  const auto &name = NRef<Element>(node).name();
  const auto pos = name.find(':');
  const auto view = std::string_view(name);
  return pos != std::string::npos ? view.substr(pos + 1) : view;
}

std::string_view attrValueView(const Node &node, const std::string_view &attrName)
{
  const auto &elem = NRef<Element>(node);
  if (elem.hasAttribute(attrName)) { return elem[attrName].getParsed(); }
  return std::string_view{};
}

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
