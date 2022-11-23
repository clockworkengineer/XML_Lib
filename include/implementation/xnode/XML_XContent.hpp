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
// Content
// =======
struct XContent : XNode
{
  // Constructors/Destructors
  explicit XContent(bool isWhiteSpace = true, XNode::Type nodeType = XNode::Type::content)
    : XNode(nodeType), m_isWhiteSpace(isWhiteSpace)
  {}
  XContent(const XContent &other) = delete;
  XContent &operator=(XContent &other) = delete;
  XContent(XContent &&other) = default;
  XContent &operator=(XContent &&other) = default;
  ~XContent() = default;
  [[nodiscard]] std::string content() const { return (m_content); }
  void addContent(const std::string &content) { m_content += content; }
  [[nodiscard]] bool isWhiteSpace() const { return (m_isWhiteSpace); }
  void setIsWhiteSpace(bool isWhiteSpace) { m_isWhiteSpace = isWhiteSpace; }

private:
  std::string m_content;
  bool m_isWhiteSpace;
};
}// namespace XML_Lib