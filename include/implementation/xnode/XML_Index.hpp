#pragma once

#include <string>

namespace XML_Lib {
// ====================
// XNode index access
// ====================
inline const XNode &XNode::operator[](const int index) const
{
  if (index >= 0 && index < static_cast<int>(getChildren().size())) { return getChildren()[index]; }
  throw XNode::Error("Invalid index used to access XNode array.");
}
// =================
// XNode name access
// =================
inline const XNode &XNode::operator[](const std::string &name) const
{
  if (isIndexable()) {
    const auto xNode = std::ranges::find_if(getChildren(), [&name](const XNode &xNode) {
      return xNode.isNameable() && XRef<XElement>(xNode).name() == name;
    });
    if (xNode != getChildren().end()) { return *xNode; }
  }
  throw XNode::Error("Element '" + name + "' does not exist.");
}
// =====================
// XElement index access
// =====================
inline const XElement &XElement::operator[](int index) const
{
  int number = 0;
  if (index >= 0 && index < static_cast<int>(getChildren().size())) {
    for (const auto &child : getChildren()) {
      if (child.isIndexable()) {
        if (number == index) { return XRef<XElement>(child); }
        number++;
      }
    }
  }
  throw XNode::Error("Invalid index used to access XNode array.");
}
// =========================
// XElement attribute access
// =========================
inline const XMLAttribute &XElement::operator[](const std::string &name) const
{
  auto attribute = std::ranges::find_if(attributes, [&name](const XMLAttribute &attribute) { return attribute.getName() == name; });
  if (attribute != attributes.end()) { return *attribute; }
  throw XNode::Error("Attribute '" + name + "' does not exist.");
}

}// namespace XML_Lib
