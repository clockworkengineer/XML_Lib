#pragma once

namespace XML_Lib {
// =========================
// XNode reference converter
// =========================
template<typename T> void checkXNodeType(const XNode &xNode)
{
  if constexpr (std::is_same_v<T, XProlog>) {
    if (!xNode.isProlog()) { throw XNode::Error("Node not a prolog."); }
  } else if constexpr (std::is_same_v<T, XDeclaration>) {
    if (!xNode.isDeclaration()) { throw XNode::Error("Node not a declaration."); }
  } else if constexpr (std::is_same_v<T, XElement>) {
    if (!xNode.isRoot() && !xNode.isSelf() && !xNode.isElement()) { throw XNode::Error("Node not a element."); }
  } else if constexpr (std::is_same_v<T, XContent>) {
    if (!xNode.isContent()) { throw XNode::Error("Node not content."); }
  } else if constexpr (std::is_same_v<T, XEntityReference>) {
    if (!xNode.isEntity()) { throw XNode::Error("Node not an entity."); }
  } else if constexpr (std::is_same_v<T, XComment>) {
    if (!xNode.isComment()) { throw XNode::Error("Node not a comment."); }
  } else if constexpr (std::is_same_v<T, XCDATA>) {
    if (!xNode.isCDATA()) { throw XNode::Error("Node not CDATA."); }
  } else if constexpr (std::is_same_v<T, XPI>) {
    if (!xNode.isPI()) { throw XNode::Error("Node not a PI."); }
  } else if constexpr (std::is_same_v<T, XDTD>) {
    if (!xNode.isDTD()) { throw XNode::Error("Node not DTD."); }
  }
}
template<typename T> T &XRef(XNode &xNode)
{
  checkXNodeType<T>(xNode);
  return static_cast<T &>(xNode.getVariant());
}
template<typename T> const T &XRef(const XNode &xNode)
{
  checkXNodeType<T>(xNode);
  return static_cast<const T &>(xNode.getVariant());
}
}// namespace XML_Lib