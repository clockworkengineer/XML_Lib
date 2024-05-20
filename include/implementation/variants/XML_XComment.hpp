#pragma once

#include <string>

namespace XML_Lib {

struct XComment final : Variant
{
  // Constructors/Destructors
  explicit XComment(const std::string &comment = "") : Variant(Variant::Type::comment), xmlComment(comment) {}
  XComment(const XComment &other) = delete;
  XComment &operator=(const XComment &other) = delete;
  XComment(XComment &&other) = default;
  XComment &operator=(XComment &&other) = default;
  ~XComment() = default;
  // Return reference to comment
  [[nodiscard]] const std::string &comment() const { return xmlComment; }

private:
  std::string xmlComment;
};
}// namespace XML_Lib