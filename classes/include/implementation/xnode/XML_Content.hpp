#pragma once

namespace XML_Lib {

// ==========================
// Get XML content from XNode
// ==========================
[[nodiscard]] inline std::string XEntityReference::getContents() const
{
  if (std::string result; !getChildren().empty()) {
    for (const auto &child : getChildren()) { result += child.getContents(); }
    return result;
  }
  return entityReferenceValue.getParsed();
}
[[nodiscard]] inline std::string XElement::getContents() const
{
  std::string result;
  for (const auto &child : getChildren()) { result += child.getContents(); }
  return result;
}
}// namespace XML_Lib
