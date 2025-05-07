#pragma once

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
          [&name](const XNode &child) { return child.isNameable() && XRef<Element>(child).name() == name; });
        xNode != getChildren().end()) {
      return *xNode;
    }
  }
  throw Error("Element '" + name + "' does not exist.");
}
// =====================
// XElement index access
// =====================
inline const Element &Element::operator[](const int index) const
{
  if (index >= 0 && index < static_cast<int>(getChildren().size())) {
    int number = 0;
    for (const auto &child : getChildren()) {
      if (child.isIndexable()) {
        if (number == index) { return XRef<Element>(child); }
        number++;
      }
    }
  }
  throw XNode::Error("Invalid index used to access XNode array.");
}
inline Element &Element::operator[](const int index)
{
  if (index >= 0 && index < static_cast<int>(getChildren().size())) {
    int number = 0;
    for (auto &child : getChildren()) {
      if (child.isIndexable()) {
        if (number == index) { return XRef<Element>(child); }
        number++;
      }
    }
  }
  throw XNode::Error("Invalid index used to access XNode array.");
}
// =========================
// XElement attribute access
// =========================
inline const XMLAttribute &Element::operator[](const std::string_view &name) const
{
  return XMLAttribute::find(attributes, name);
}
inline XMLAttribute &Element::operator[](const std::string_view &name) { return XMLAttribute::find(attributes, name); }
}// namespace XML_Lib
