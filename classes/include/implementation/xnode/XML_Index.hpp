#pragma once

#include <string>

namespace XML_Lib {
// ====================
// XNode index access
// ====================
inline const XNode &XNode::operator[](const int index) const
{
  if (index >= 0 && index < static_cast<int>(getChildren().size())) { return getChildren()[index]; }
  throw Error("Invalid index used to access XNode array.");
}
// =================
// XNode name access
// =================
inline const XNode &XNode::operator[](const std::string &name) const
{
  if (isIndexable()) {
    if (const auto xNode = std::ranges::find_if(getChildren(),
          [&name](const XNode &child) { return child.isNameable() && XRef<XElement>(child).name() == name; });
        xNode != getChildren().end()) { return *xNode; }
  }
  throw Error("Element '" + name + "' does not exist.");
}
// =====================
// XElement index access
// =====================
inline const XElement &XElement::operator[](const int index) const
{
  if (index >= 0 && index < static_cast<int>(getChildren().size())) {
    int number = 0;
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
  if (const auto attribute = std::ranges::find_if(
        attributes, [&name](const XMLAttribute &attr) { return attr.getName() == name; });
      attribute != attributes.end()) { return *attribute; }
  throw XNode::Error("Attribute '" + name + "' does not exist.");
}

}// namespace XML_Lib
