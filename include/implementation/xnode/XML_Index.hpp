#pragma once

#include <string>

namespace XML_Lib {
// ====================
// XNode index access
// ====================
inline const XNode &XNode::operator[](int index) const
{
  if ((index >= 0) && (index < (static_cast<int>(getChildren().size())))) { return (((getChildren()[index]))); }
  throw XNode::Error("Invalid index used to access array.");
}
// =================
// XNode name access
// =================
inline const XNode &XNode::operator[](const std::string &name) const
{
  if (isIndexable()) {
    for (const auto &element : getChildren()) {
      if (element.isNameable() && XRef<XElement>(element).name() == name) { return (element); }
    }
    // auto xNode = std::find_if(getChildren().begin(), getChildren().end(), [&name](const XNode &xNode) {
    //   return (XRef<XElement>(xNode).name() == name);
    // });
    // if (xNode!= getChildren().end()) {
    //   return(*xNode);
    // }
  }
  throw XNode::Error("Invalid index used to access array.");
}
// =====================
// XElement index access
// =====================
inline const XElement &XElement::operator[](int index) const
{
  int number = 0;
  if ((index >= 0) && (index < (static_cast<int>(getChildren().size())))) {
    for (const auto &child : getChildren()) {
      if (child.isIndexable()) {
        if (number == index) { return (XRef<XElement>(child)); }
        number++;
      }
    }
  }
  throw XNode::Error("Invalid index used to access array.");
}
// =========================
// XElement attribute access
// =========================
inline const XMLAttribute &XElement::operator[](const std::string &name) const
{
  auto attribute = std::find_if(
    attributes.begin(), attributes.end(), [&name](const XMLAttribute &attr) { return (attr.getName() == name); });

  if (attribute != attributes.end()) { return (*attribute); }
  throw XNode::Error("Attribute '" + name + "' does not exist.");
}

}// namespace XML_Lib
