#pragma once
// =======
// C++ STL
// =======
#include <type_traits>
// =================
// LIBRARY NAMESPACE
// =================
namespace XML_Lib {
// =========================
// XNode reference converter
// =========================
template<typename T> void CheckXNodeType(const XNode &xNode)
{
  if constexpr (std::is_same_v<T, XProlog>) {
    if (xNode.getType() != XNode::Type::prolog) { throw XNode::Error("Node not a prolog."); }
  } else if constexpr (std::is_same_v<T, XDeclaration>) {
    if (xNode.getType() != XNode::Type::declaration) { throw XNode::Error("Node not a declaration."); }
  } else if constexpr (std::is_same_v<T, XElement>) {
    if ((xNode.getType() != XNode::Type::root) && (xNode.getType() != XNode::Type::self)
        && (xNode.getType() != XNode::Type::element)) {
      throw XNode::Error("Node not a element.");
    }
  } else if constexpr (std::is_same_v<T, XContent>) {
    if (xNode.getType() != XNode::Type::content) { throw XNode::Error("Node not content."); }
  } else if constexpr (std::is_same_v<T, XEntityReference>) {
    if (xNode.getType() != XNode::Type::entity) { throw XNode::Error("Node not an entity."); }
  } else if constexpr (std::is_same_v<T, XComment>) {
    if (xNode.getType() != XNode::Type::comment) { throw XNode::Error("Node not a comment."); }
  } else if constexpr (std::is_same_v<T, XCDATA>) {
    if (xNode.getType() != XNode::Type::cdata) { throw XNode::Error("Node not CDATA."); }
  } else if constexpr (std::is_same_v<T, XPI>) {
    if (xNode.getType() != XNode::Type::pi) { throw XNode::Error("Node not a PI."); }
  } else if constexpr (std::is_same_v<T, XDTD>) {
    if (xNode.getType() != XNode::Type::dtd) { throw XNode::Error("Node not DTD."); }
  }
}
template<typename T> T &XRef(XNode &xNode)
{
  CheckXNodeType<T>(xNode);
  return (static_cast<T &>(xNode));
}
template<typename T> const T &XRef(const XNode &xNode)
{
  CheckXNodeType<T>(xNode);
  return (static_cast<const T &>(xNode));
}
}// namespace XML_Lib