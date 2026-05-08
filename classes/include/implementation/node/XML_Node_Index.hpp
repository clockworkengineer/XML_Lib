#pragma once
#include "common/XML_Error.hpp"

#include <algorithm>
#include <string>
#include <string_view>

namespace XML_Lib {
// ====================
// Node index access
// ====================
inline const Node &Node::operator[](const int index) const
{
  if (index >= 0 && index < static_cast<int>(getChildren().size())) { return getChildren()[index]; }
  XML_LIB_THROW(Error("Invalid index used to access Node array."));
}
// =================
// Node name access
// =================
inline const Node &Node::operator[](const std::string_view &name) const
{
  if (isIndexable()) {
    if (const auto xNode = std::find_if(getChildren().begin(), getChildren().end(),
          [&name](const Node &child) { return child.isNameable() && NRef<Element>(child).name() == name; });
        xNode != getChildren().end()) {
      return *xNode;
    }
  }
  XML_LIB_THROW(Error("Element '" + std::string(name) + "' does not exist."));
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
        if (number == index) { return NRef<Element>(child); }
        number++;
      }
    }
  }
  XML_LIB_THROW(Node::Error("Invalid index used to access Node array."));
}
inline Element &Element::operator[](const int index)
{
  if (index >= 0 && index < static_cast<int>(getChildren().size())) {
    int number = 0;
    for (auto &child : getChildren()) {
      if (child.isIndexable()) {
        if (number == index) { return NRef<Element>(child); }
        number++;
      }
    }
  }
  XML_LIB_THROW(Node::Error("Invalid index used to access Node array."));
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
