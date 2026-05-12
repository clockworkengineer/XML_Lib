#pragma once

namespace XML_Lib {

struct EntityReference final : Variant
{
  // Constructors/Destructors
  explicit EntityReference(XMLValue value) : Variant(Type::entity), entityReferenceValue(std::move(value)) {}
  XML_LIB_NO_COPY_MOVE_DTOR(EntityReference);
  // Return reference to entity reference
  [[nodiscard]] const XMLValue &value() const { return entityReferenceValue; }
  // Return Variant contents
  [[nodiscard]] std::string getContents() const override;

private:
  XMLValue entityReferenceValue;
};
}// namespace XML_Lib