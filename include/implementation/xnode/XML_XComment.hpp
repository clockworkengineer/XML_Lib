#pragma once
// =======
// C++ STL
// =======
#include <string>
// =================
// LIBRARY NAMESPACE
// =================
namespace XML_Lib {
// =======
// Comment
// =======
struct XComment : XNode
{
  // Constructors/Destructors
  explicit XComment(const std::string comment = "") : XNode(XNode::Type::comment), m_comment(comment) {}
  XComment(const XComment &other) = delete;
  XComment &operator=(XComment &other) = delete;
  XComment(XComment &&other) = default;
  XComment &operator=(XComment &&other) = default;
  ~XComment() = default;
  // Return reference to comment
  [[nodiscard]] const std::string &comment() const { return (m_comment); }

private:
  std::string m_comment;
};
}// namespace XML_Lib