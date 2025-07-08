#pragma once

namespace XML_Lib {
// =======================
// What is Node variant ?
// =======================
template<typename T> bool isA(const Node &xNode)
{
  if constexpr (std::is_same_v<T, Prolog>) {
    return xNode.getVariant().getNodeType() == Variant::Type::prolog;
  } else if constexpr (std::is_same_v<T, Declaration>) {
    return xNode.getVariant().getNodeType() == Variant::Type::declaration;
  } else if constexpr (std::is_same_v<T, Element>) {
    return xNode.getVariant().getNodeType() == Variant::Type::element;
  } else if constexpr (std::is_same_v<T, Self>) {
    return xNode.getVariant().getNodeType() == Variant::Type::self;
  } else if constexpr (std::is_same_v<T, Root>) {
    return xNode.getVariant().getNodeType() == Variant::Type::root;
  } else if constexpr (std::is_same_v<T, Content>) {
    return xNode.getVariant().getNodeType() == Variant::Type::content;
  } else if constexpr (std::is_same_v<T, EntityReference>) {
    return xNode.getVariant().getNodeType() == Variant::Type::entity;
  } else if constexpr (std::is_same_v<T, Comment>) {
    return xNode.getVariant().getNodeType() == Variant::Type::comment;
  } else if constexpr (std::is_same_v<T, CDATA>) {
    return xNode.getVariant().getNodeType() == Variant::Type::cdata;
  } else if constexpr (std::is_same_v<T, PI>) {
    return xNode.getVariant().getNodeType() == Variant::Type::pi;
  } else if constexpr (std::is_same_v<T, DTD>) {
    return xNode.getVariant().getNodeType() == Variant::Type::dtd;
  } else {
    return false;
  }
}
// =========================
// Node reference converter
// =========================
template<typename T> void checkNodeType(const Node &xNode)
{
  if constexpr (std::is_same_v<T, Prolog>) {
    if (!isA<T>(xNode)) { throw Node::Error("Node not a prolog."); }
  } else if constexpr (std::is_same_v<T, Declaration>) {
    if (!isA<T>(xNode)) { throw Node::Error("Node not a declaration."); }
  } else if constexpr (std::is_same_v<T, Element>) {
    if (!isA<Root>(xNode) && !isA<Self>(xNode)  && !isA<Element>(xNode)) { throw Node::Error("Node not an element."); }
  } else if constexpr (std::is_same_v<T, Self>) {
    if (!isA<Self>(xNode)) { throw Node::Error("Node not a (root) element."); }
  } else if constexpr (std::is_same_v<T, Root>) {
    if (!isA<Root>(xNode)) { throw Node::Error("Node not a (self) element."); }
  } else if constexpr (std::is_same_v<T, Content>) {
    if (!isA<T>(xNode)) { throw Node::Error("Node not content."); }
  } else if constexpr (std::is_same_v<T, EntityReference>) {
    if (!isA<T>(xNode)) { throw Node::Error("Node not an entity."); }
  } else if constexpr (std::is_same_v<T, Comment>) {
    if (!isA<T>(xNode)) { throw Node::Error("Node not a comment."); }
  } else if constexpr (std::is_same_v<T, CDATA>) {
    if (!isA<T>(xNode)) { throw Node::Error("Node not CDATA."); }
  } else if constexpr (std::is_same_v<T, PI>) {
    if (!isA<T>(xNode)) { throw Node::Error("Node not a PI."); }
  } else if constexpr (std::is_same_v<T, DTD>) {
    if (!isA<T>(xNode)) { throw Node::Error("Node not DTD_Validator."); }
  }
}
template<typename T> T &NRef(Node &xNode)
{
  checkNodeType<T>(xNode);
  return static_cast<T &>(xNode.getVariant());
}
template<typename T> const T &NRef(const Node &xNode)
{
  checkNodeType<T>(xNode);
  return static_cast<const T &>(xNode.getVariant());
}
}// namespace XML_Lib