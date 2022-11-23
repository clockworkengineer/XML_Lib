#pragma once
// =======
// C++ STL
// =======
#include <string>
// =================
// LIBRARY NAMESPACE
// =================
namespace XML_Lib {
// ===================================================
// Forward declarations for interfaces/classes/structs
// ===================================================
class IEntityMapper;
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
  [[nodiscard]] std::string comment() const { return (m_comment); }
  void setComment(const std::string &comment) { m_comment = comment; }

private:
  std::string m_comment;
};
}