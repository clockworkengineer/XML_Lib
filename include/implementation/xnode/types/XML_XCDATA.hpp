#pragma once

#include <string>

namespace XML_Lib {
// CDATA
// =====
struct XCDATA : XNode
{
  // Constructors/Destructors
  explicit XCDATA(const std::string &cdata) : XNode(XNode::Type::cdata), m_cdata(cdata) {}
  XCDATA(const XCDATA &other) = delete;
  XCDATA &operator=(const XCDATA &other) = delete;
  XCDATA(XCDATA &&other) = default;
  XCDATA &operator=(XCDATA &&other) = default;
  ~XCDATA() = default;
  // Return reference to cdata
  [[nodiscard]] const std::string &CDATA() const { return (m_cdata); }

private:
  std::string m_cdata;
};
}// namespace XML_Lib