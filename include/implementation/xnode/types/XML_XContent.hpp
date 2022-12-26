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
  explicit XContent(bool isWhiteSpace = true) : XNode(XNode::Type::content), m_isWhiteSpace(isWhiteSpace) {}
  XContent(const XContent &other) = delete;
  XContent &operator=(const XContent &other) = delete;
  XContent(XContent &&other) = default;
  XContent &operator=(XContent &&other) = default;
  ~XContent() = default;
  // Get reference to content string
  [[nodiscard]] std::string getContent() const { return (m_content); }
  // Add to content
  void addContent(const std::string &content) { m_content += content; }
  // Is content all whitespace
  [[nodiscard]] bool isWhiteSpace() const { return (m_isWhiteSpace); }
  // Set whitespace boolean
  void setIsWhiteSpace(bool isWhiteSpace) { m_isWhiteSpace = isWhiteSpace; }

private:
  std::string m_content;
  bool m_isWhiteSpace;
};
}// namespace XML_Lib