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
  if (getType() <= Variant::Type::element) {
    for (const auto &element : getChildren()) {
      if (XRef<XElement>(element).name() == name) { return (element); }
    }
  }
  throw XNode::Error("Invalid index used to access array.");
}
// =====================
// XElement index access
// =====================
inline const XElement &XElement::operator[]([[maybe_unused]] int index) const
{
  int number = 0;
  if ((index >= 0) && (index < (static_cast<int>(getChildren().size())))) {
    for (const auto &child : getChildren()) {
      if (child.getType() <= Variant::Type::element) {
        if (number == index) { return (XRef<XElement>(child)); }
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
  if (getType() <= Variant::Type::element) {
    for (const auto &element : getChildren()) {
      if (XRef<XElement>(element).elementName == name) { return (XRef<XElement>(element)); }
    }
  }
  throw XNode::Error("Invalid index used to access array.");
}

}// namespace XML_Lib
