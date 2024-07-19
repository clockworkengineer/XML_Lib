#pragma once

namespace XML_Lib {

struct Comment final : Variant
{
  // Constructors/Destructors
  explicit Comment(std::string comment = "") : Variant(Type::comment), xmlComment(std::move(comment)) {}
  Comment(const Comment &other) = delete;
  Comment &operator=(const Comment &other) = delete;
  Comment(Comment &&other) = default;
  Comment &operator=(Comment &&other) = default;
  ~Comment() override = default;
  // Return reference to comment
  [[nodiscard]] const std::string &value() const { return xmlComment; }

private:
  std::string xmlComment;
};
}// namespace XML_Lib