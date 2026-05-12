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
  // name() and child text are already UTF-8 — no re-encoding occurs.
  // Cache the concatenated result and revalidate only when the child count changes.
  const auto currentChildCount = getChildren().size();
  if (currentChildCount != contentCacheChildCount) {
    contentCache.clear();
    for (const auto &child : getChildren()) { contentCache += child.getContents(); }
    contentCacheChildCount = currentChildCount;
  }
  return contentCache;
}
}// namespace XML_Lib
