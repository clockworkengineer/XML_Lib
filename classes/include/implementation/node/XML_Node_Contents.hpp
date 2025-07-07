#pragma once

namespace XML_Lib {

// ==========================
// Get XML content from Node
// ==========================
[[nodiscard]] inline std::string EntityReference::getContents() const
{
  if (std::string result; !getChildren().empty()) {
    for (const auto &child : getChildren()) { result += child.getContents(); }
    return result;
  }
  return entityReferenceValue.getParsed();
}
[[nodiscard]] inline std::string Element::getContents() const
{
  std::string result;
  for (const auto &child : getChildren()) { result += child.getContents(); }
  return result;
}
}// namespace XML_Lib
