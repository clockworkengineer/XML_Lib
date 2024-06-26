#pragma once

namespace XML_Lib {

struct XComment final : Variant
{
  // Constructors/Destructors
  explicit XComment(std::string comment = "") : Variant(Type::comment), xmlComment(std::move(comment)) {}
  XComment(const XComment &other) = delete;
  XComment &operator=(const XComment &other) = delete;
  XComment(XComment &&other) = default;
  XComment &operator=(XComment &&other) = default;
  ~XComment() override = default;
  // Return reference to comment
  [[nodiscard]] const std::string &value() const { return xmlComment; }

private:
  std::string xmlComment;
};
}// namespace XML_Lib