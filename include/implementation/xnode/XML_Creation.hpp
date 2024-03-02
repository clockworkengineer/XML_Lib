#pragma once

#include <string>

namespace XML_Lib {
// Construct JNode from raw values
template<typename T> XNode::XNode(T value)
{
  if constexpr (std::is_convertible_v<T, std::unique_ptr<Variant>>) { xmlVariant = std::move(value); }
}
// ====================
// XNode index access
// ====================
inline const XNode &XNode::operator[](int index) const
{
  if ((index >= 0) && (index < (static_cast<int>(getChildren().size())))) { return (*((getChildren()[index]))); }
  throw XNode::Error("Invalid index used to access array.");
}
// =================
// XNode name access
// =================
inline const XNode &XNode::operator[](const std::string &name) const
{
  if (getType() <= Variant::Type::element) {
    for (const auto &element : getChildren()) {
      if (XRef<XElement>(*element).name() == name) { return (*element); }
    }
  }
  throw XNode::Error("Invalid index used to access array.");
}
// ===============
// Add child XNode
// ===============
// inline void XNode::addChild(std::unique_ptr<XNode> &child) { children.push_back(std::move(child)); }
// inline void XNode::addChild(std::unique_ptr<XNode> &&child) { children.push_back(std::move(child)); }
// ============================
// Get XNode children reference
// ============================
// inline std::vector<std::unique_ptr<XNode>> &XNode::getChildren() { return (children); };
// inline const std::vector<std::unique_ptr<XNode>> &XNode::getChildren() const { return (children); }
// =====================
// XElement index access
// =====================
inline const XElement &XElement::operator[]([[maybe_unused]] int index) const
{
  int number = 0;
  if ((index >= 0) && (index < (static_cast<int>(getChildren().size())))) {
    for (const auto &child : getChildren()) {
      if (child->getType() <= Variant::Type::element) {
        if (number == index) { return (XRef<XElement>(*child)); }
        number++;
      }
    }
  }
  throw XNode::Error("Invalid index used to access array.");
}
// ====================
// XElement name access
// ====================
inline const XElement &XElement::operator[]([[maybe_unused]] const std::string &name) const
{
  // if (getType() <= Variant::Type::element) {
  //   for (const auto &element : XRef<XElement>(*this).getChildren()) {
  //     if (XRef<XElement>(*element).elementName == name) { return (XRef<XElement>(*element)); }
  //   }
  // }
  throw XNode::Error("Invalid index used to access array.");
}
// ==================
// XNode get contents
// ==================
inline std::string XNode::getContents() const
{
  std::string result;
  for (const auto &node : getChildren()) {
    if (node->getType() == Variant::Type::content) {
      result += XRef<XContent>(*node).getContent();
    } else if (node->getType() == Variant::Type::entity) {
      if (!XRef<XEntityReference>(*node).getChildren().empty()) {
        result += node->getContents();
      } else {
        result += XRef<XEntityReference>(*node).value().getParsed();
      }
    } else if (node->getType() == Variant::Type::cdata) {
      result += XRef<XCDATA>(*node).CDATA();
    }
  }
  return (result);
}
}// namespace XML_Lib
