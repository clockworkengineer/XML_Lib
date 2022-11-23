#pragma once
// =======
// C++ STL
// =======
#include <string>
// =================
// LIBRARY NAMESPACE
// =================
namespace XML_Lib {
// CDATA
// =====
struct XCDATA : XNode
{
  // Constructors/Destructors
  explicit XCDATA(XNode::Type nodeType = XNode::Type::cdata) : XNode(nodeType) {}
  XCDATA(const XCDATA &other) = delete;
  XCDATA &operator=(XCDATA &other) = delete;
  XCDATA(XCDATA &&other) = default;
  XCDATA &operator=(XCDATA &&other) = default;
  ~XCDATA() = default;
  [[nodiscard]] std::string CDATA() const { return (m_cdata); }
  void setCDATA(const std::string &cdata) { m_cdata = cdata; }

private:
  std::string m_cdata;
};
}