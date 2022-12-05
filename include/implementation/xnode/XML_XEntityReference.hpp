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
  explicit XEntityReference(XMLValue value, XNode::Type nodeType = XNode::Type::entity)
    : XNode(nodeType), m_value(std::move(value))
  {}
  XEntityReference(const XEntityReference &other) = delete;
  XEntityReference &operator=(XEntityReference &other) = delete;
  XEntityReference(XEntityReference &&other) = default;
  XEntityReference &operator=(XEntityReference &&other) = default;
  ~XEntityReference() = default;
  [[nodiscard]] XMLValue value() const { return (m_value); }
  void setValue(const XMLValue &value) { m_value = value; }

private:
  XMLValue m_value;
};
}