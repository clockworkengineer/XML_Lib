#pragma once

#include <string>

namespace XML_Lib {

struct XDeclaration : XNode
{
  // Constructors/Destructors
  XDeclaration(const std::string &version, const std::string &encoding, const std::string &standalone)
    : XNode(XNode::Type::declaration), m_version(version), m_encoding(encoding), m_standalone(standalone)
  {}
  XDeclaration(const XDeclaration &other) = delete;
  XDeclaration &operator=(const XDeclaration &other) = delete;
  XDeclaration(XDeclaration &&other) = default;
  XDeclaration &operator=(XDeclaration &&other) = default;
  ~XDeclaration() = default;
  // Get reference to declaration attibutes
  [[nodiscard]] const std::string &version() const { return (m_version); }
  [[nodiscard]] const std::string &encoding() const { return (m_encoding); }
  [[nodiscard]] const std::string &standalone() const { return (m_standalone); }

private:
  std::string m_version;
  std::string m_encoding;
  std::string m_standalone;
};
}// namespace XML_Lib