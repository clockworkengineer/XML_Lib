#pragma once
// =======
// C++ STL
// =======
#include <string>
// =================
// LIBRARY NAMESPACE
// =================
namespace XML_Lib {
// ===========
// Declaration
// ===========
struct XDeclaration : XNode
{
  // Constructors/Destructors
  XDeclaration(const std::string &version, const std::string &encoding, const std::string &standalone)
    : XNode(XNode::Type::declaration)
  {
    if (!version.empty()) { m_version = version; }
    if (!encoding.empty()) { m_encoding = encoding; }
    if (!standalone.empty()) { m_standalone = standalone; }
  }
  XDeclaration(const XDeclaration &other) = delete;
  XDeclaration &operator=(XDeclaration &other) = delete;
  XDeclaration(XDeclaration &&other) = default;
  XDeclaration &operator=(XDeclaration &&other) = default;
  ~XDeclaration() = default;
  // Get reference to declaration attibutes
  [[nodiscard]] const std::string &version() const { return (m_version); }
  [[nodiscard]] const std::string &encoding() const { return (m_encoding); }
  [[nodiscard]] const std::string &standalone() const { return (m_standalone); }

private:
  std::string m_version{ "1.0" };
  std::string m_encoding{ "UTF-8" };
  std::string m_standalone{ "no" };
};
}// namespace XML_Lib