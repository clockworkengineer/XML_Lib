#pragma once
// =======
// C++ STL
// =======
#include <string>
// =================
// LIBRARY NAMESPACE
// =================
namespace XML_Lib {
// ====================
// XNode index access
// ====================
inline XNode &XNode::operator[](int index) const
{
  if ((index >= 0) && (index < (static_cast<int>(XRef<XNode>(*this).getChildren().size())))) {
    return (*((XRef<XNode>(*this).getChildren()[index])));
  }
  throw XNode::Error("Invalid index used to access array.");
}
// =================
// XNode name access
// =================
inline XNode &XNode::operator[](const std::string &name) const
{
  if (m_type <= XNode::Type::element) {
    for (const auto &element : XRef<XElement>(*this).getChildren()) {
      if (XRef<XElement>(*element).name() == name) { return (*element); }
    }
  }
  throw XNode::Error("Invalid index used to access array.");
}
// =====================
// XElement index access
// =====================
inline XElement &XElement::operator[](int index) const
{
  int number = 0;
  if ((index >= 0) && (index < (static_cast<int>(XRef<XElement>(*this).getChildren().size())))) {
    for (const auto &child : XRef<XNode>(*this).getChildren()) {
      if (XRef<XNode>(*child).getType() <= XNode::Type::element) {
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
inline XElement &XElement::operator[](const std::string &name) const
{
  if (getType() <= XNode::Type::element) {
    for (const auto &element : XRef<XElement>(*this).getChildren()) {
      if (XRef<XElement>(*element).m_name == name) { return (XRef<XElement>(*element)); }
    }
  }
  throw XNode::Error("Invalid index used to access array.");
}
// ==================
// XNode get contents
// ==================
inline std::string XNode::getContents() const
{
  std::string result;
  for (const auto &node : getChildren()) {
    if (node->getType() == XNode::Type::content) {
      result += XRef<XContent>(*node).content();
    } else if (node->getType() == XNode::Type::entity) {
      if (!XRef<XEntityReference>(*node).getChildren().empty()) {
        result += XRef<XEntityReference>(*node).getContents();
      } else {
        result += XRef<XEntityReference>(*node).value().parsed;
      }
    } else if (node->getType() == XNode::Type::cdata) {
      result += XRef<XCDATA>(*node).CDATA();
    }
  }
  return (result);
}
}// namespace XML_Lib
