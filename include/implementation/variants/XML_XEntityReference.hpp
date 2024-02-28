#pragma once

namespace XML_Lib {

struct XEntityReference : Variant
{
  // Constructors/Destructors
  explicit XEntityReference(const XMLValue &value) : Variant(Variant::Type::entity), entityReferenceValue(value) {}
  XEntityReference(const XEntityReference &other) = delete;
  XEntityReference &operator=(const XEntityReference &other) = delete;
  XEntityReference(XEntityReference &&other) = default;
  XEntityReference &operator=(XEntityReference &&other) = default;
  ~XEntityReference() = default;
  // Return reference to entity reference
  [[nodiscard]] const XMLValue &value() const { return (entityReferenceValue); }

private:
  XMLValue entityReferenceValue;
};
}// namespace XML_Lib