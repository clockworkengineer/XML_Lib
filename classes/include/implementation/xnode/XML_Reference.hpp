#pragma once

namespace XML_Lib {
// =======================
// What is XNode variant ?
// =======================
template<typename T> bool isA(const XNode &xNode)
{
  if constexpr (std::is_same_v<T, XProlog>) {
    return xNode.getVariant().getNodeType() == Variant::Type::prolog;
  } else if constexpr (std::is_same_v<T, XDeclaration>) {
    return xNode.getVariant().getNodeType() == Variant::Type::declaration;
  } else if constexpr (std::is_same_v<T, XElement>) {
    return xNode.getVariant().getNodeType() == Variant::Type::element;
  } else if constexpr (std::is_same_v<T, XSelf>) {
    return xNode.getVariant().getNodeType() == Variant::Type::self;
  } else if constexpr (std::is_same_v<T, XRoot>) {
    return xNode.getVariant().getNodeType() == Variant::Type::root;
  } else if constexpr (std::is_same_v<T, XContent>) {
    return xNode.getVariant().getNodeType() == Variant::Type::content;
  } else if constexpr (std::is_same_v<T, XEntityReference>) {
    return xNode.getVariant().getNodeType() == Variant::Type::entity;
  } else if constexpr (std::is_same_v<T, XComment>) {
    return xNode.getVariant().getNodeType() == Variant::Type::comment;
  } else if constexpr (std::is_same_v<T, XCDATA>) {
    return xNode.getVariant().getNodeType() == Variant::Type::cdata;
  } else if constexpr (std::is_same_v<T, XPI>) {
    return xNode.getVariant().getNodeType() == Variant::Type::pi;
  } else if constexpr (std::is_same_v<T, XDTD>) {
    return xNode.getVariant().getNodeType() == Variant::Type::dtd;
  } else {
    static_assert(false, "Not a valid XNode variant.");
  }
}
// =========================
// XNode reference converter
// =========================
template<typename T> void checkXNodeType(const XNode &xNode)
{
  if constexpr (std::is_same_v<T, XProlog>) {
    if (!isA<T>(xNode)) { throw XNode::Error("Node not a prolog."); }
  } else if constexpr (std::is_same_v<T, XDeclaration>) {
    if (!isA<T>(xNode)) { throw XNode::Error("Node not a declaration."); }
  } else if constexpr (std::is_same_v<T, XElement>) {
    if (!isA<XRoot>(xNode) && !isA<XSelf>(xNode)  && !isA<XElement>(xNode)) { throw XNode::Error("Node not an element."); }
  } else if constexpr (std::is_same_v<T, XSelf>) {
    if (!isA<XSelf>(xNode)) { throw XNode::Error("Node not a (root) element."); }
  } else if constexpr (std::is_same_v<T, XRoot>) {
    if (!isA<XRoot>(xNode)) { throw XNode::Error("Node not a (self) element."); }
  } else if constexpr (std::is_same_v<T, XContent>) {
    if (!isA<T>(xNode)) { throw XNode::Error("Node not content."); }
  } else if constexpr (std::is_same_v<T, XEntityReference>) {
    if (!isA<T>(xNode)) { throw XNode::Error("Node not an entity."); }
  } else if constexpr (std::is_same_v<T, XComment>) {
    if (!isA<T>(xNode)) { throw XNode::Error("Node not a comment."); }
  } else if constexpr (std::is_same_v<T, XCDATA>) {
    if (!isA<T>(xNode)) { throw XNode::Error("Node not CDATA."); }
  } else if constexpr (std::is_same_v<T, XPI>) {
    if (!isA<T>(xNode)) { throw XNode::Error("Node not a PI."); }
  } else if constexpr (std::is_same_v<T, XDTD>) {
    if (!isA<T>(xNode)) { throw XNode::Error("Node not DTD."); }
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