#pragma once

namespace XML_Lib {

struct Comment final : Variant
{
  // Constructors/Destructors
  explicit Comment(const std::string_view &comment = "") : Variant(Type::comment), xmlComment(comment) {}
  XML_LIB_NO_COPY_MOVE_DTOR(Comment);
  // Return reference to comment
  [[nodiscard]] const std::string &value() const { return xmlComment; }

private:
  std::string xmlComment;
};
}// namespace XML_Lib