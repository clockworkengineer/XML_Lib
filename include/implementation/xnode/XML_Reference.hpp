#pragma once

#include <type_traits>

namespace XML_Lib {
// =========================
// XNode reference converter
// =========================
template<typename T> void CheckXNodeType(const XNode &xNode)
{
  if constexpr (std::is_same_v<T, XProlog>) {
    if (xNode.getType() != Variant::Type::prolog) { throw XNode::Error("Node not a prolog."); }
  } else if constexpr (std::is_same_v<T, XDeclaration>) {
    if (xNode.getType() != Variant::Type::declaration) { throw XNode::Error("Node not a declaration."); }
  } else if constexpr (std::is_same_v<T, XElement>) {
    if ((xNode.getType() != Variant::Type::root) && (xNode.getType() != Variant::Type::self)
        && (xNode.getType() != Variant::Type::element)) {
      throw XNode::Error("Node not a element.");
    }
  } else if constexpr (std::is_same_v<T, XContent>) {
    if (xNode.getType() != Variant::Type::content) { throw XNode::Error("Node not content."); }
  } else if constexpr (std::is_same_v<T, XEntityReference>) {
    if (xNode.getType() != Variant::Type::entity) { throw XNode::Error("Node not an entity."); }
  } else if constexpr (std::is_same_v<T, XComment>) {
    if (xNode.getType() != Variant::Type::comment) { throw XNode::Error("Node not a comment."); }
  } else if constexpr (std::is_same_v<T, XCDATA>) {
    if (xNode.getType() != Variant::Type::cdata) { throw XNode::Error("Node not CDATA."); }
  } else if constexpr (std::is_same_v<T, XPI>) {
    if (xNode.getType() != Variant::Type::pi) { throw XNode::Error("Node not a PI."); }
  } else if constexpr (std::is_same_v<T, XDTD>) {
    if (xNode.getType() != Variant::Type::dtd) { throw XNode::Error("Node not DTD."); }
  }
}
template<typename T> T &XRef(XNode &xNode)
{
  CheckXNodeType<T>(xNode);
  return (static_cast<T &>(xNode.getVariant()));
}
template<typename T> const T &XRef(const XNode &xNode) 
{
  CheckXNodeType<T>(xNode);
  return (static_cast<const T &>(xNode.getVariant()));
}
}// namespace XML_Lib