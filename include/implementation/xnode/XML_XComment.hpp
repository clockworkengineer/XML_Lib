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
  explicit XComment(std::string comment = "", XNode::Type nodeType = XNode::Type::comment)
    : XNode(nodeType), m_comment(std::move(comment))
  {}
  XComment(const XComment &other) = delete;
  XComment &operator=(XComment &other) = delete;
  XComment(XComment &&other) = default;
  XComment &operator=(XComment &&other) = default;
  ~XComment() = default;
  [[nodiscard]] const std::string &comment() const { return (m_comment); }

private:
  std::string m_comment;
};
}