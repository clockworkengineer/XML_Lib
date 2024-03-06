#pragma once

#include <string>

namespace XML_Lib {
// ===============================
// Construct JNode from raw values
// ===============================
template<typename T> XNode::XNode(T value)
{
  if constexpr (std::is_convertible_v<T, std::unique_ptr<Variant>>) { xmlVariant = std::move(value); }
}
// ==================
// XNode get contents
// ==================
inline std::string Variant::getContents() const
{
  std::string result;
  for (const auto &node : getChildren()) {
    if (node.getType() == Variant::Type::content) {
      result += XRef<XContent>(node).getContent();
    } else if (node.getType() == Variant::Type::entity) {
      if (!XRef<XEntityReference>(node).getChildren().empty()) {
        result += node.getContents();
      } else {
        result += XRef<XEntityReference>(node).value().getParsed();
      }
    } else if (node.getType() == Variant::Type::cdata) {
      result += XRef<XCDATA>(node).CDATA();
    }
  }
  return (result);
}
}// namespace XML_Lib
