#include <utility>

#pragma once

namespace XML_Lib {

struct XEntityReference final : Variant
{
  // Constructors/Destructors
  explicit XEntityReference(XMLValue value) : Variant(Type::entity), entityReferenceValue(std::move(value)) {}
  XEntityReference(const XEntityReference &other) = delete;
  XEntityReference &operator=(const XEntityReference &other) = delete;
  XEntityReference(XEntityReference &&other) = default;
  XEntityReference &operator=(XEntityReference &&other) = default;
  ~XEntityReference() override = default;
  // Return reference to entity reference
  [[nodiscard]] const XMLValue &value() const { return entityReferenceValue; }
  // Return Variant contents
  [[nodiscard]] std::string getContents() const override
  {
    if (std::string result; !getChildren().empty()) {
      for (const auto &xNode : getChildren()) { result += xNode.getContents(); }
      return result;
    }
    return entityReferenceValue.getParsed();
  }

private:
  XMLValue entityReferenceValue;
};
}// namespace XML_Lib