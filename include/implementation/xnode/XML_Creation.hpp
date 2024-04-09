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
  for (const auto &xNode : getChildren()) {
    if (xNode.isContent()) {
      result += XRef<XContent>(xNode).getContent();
    } else if (xNode.isEntity()) {
      if (!XRef<XEntityReference>(xNode).getChildren().empty()) {
        result += xNode.getContents();
      } else {
        result += XRef<XEntityReference>(xNode).value().getParsed();
      }
    } else if (xNode.isCDATA()) {
      result += XRef<XCDATA>(xNode).CDATA();
    } else if (xNode.isElement()) {
         result += xNode.getContents();
    }
  }
  return (result);
}
}// namespace XML_Lib
