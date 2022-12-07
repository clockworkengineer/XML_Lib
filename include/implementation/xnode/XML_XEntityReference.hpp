#pragma once
// =================
// LIBRARY NAMESPACE
// =================
namespace XML_Lib {
// ================
// Entity Reference
// ================
struct XEntityReference : XNode
{
  // Constructors/Destructors
  XEntityReference(const XMLValue &value, XNode::Type nodeType = XNode::Type::entity) : XNode(nodeType), m_value(value)
  {}
  XEntityReference(const XEntityReference &other) = delete;
  XEntityReference &operator=(XEntityReference &other) = delete;
  XEntityReference(XEntityReference &&other) = default;
  XEntityReference &operator=(XEntityReference &&other) = default;
  ~XEntityReference() = default;
  [[nodiscard]] const XMLValue &value() const { return (m_value); }

private:
  XMLValue m_value;
};
}// namespace XML_Lib