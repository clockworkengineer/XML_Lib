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
  explicit XEntityReference(const XMLValue &value) : XNode(XNode::Type::entity), m_value(value)
  {}
  XEntityReference(const XEntityReference &other) = delete;
  XEntityReference &operator=(const XEntityReference &other) = delete;
  XEntityReference(XEntityReference &&other) = default;
  XEntityReference &operator=(XEntityReference &&other) = default;
  ~XEntityReference() = default;
  // Return reference to entity reference
  [[nodiscard]] const XMLValue &value() const { return (m_value); }

private:
  XMLValue m_value;
};
}// namespace XML_Lib