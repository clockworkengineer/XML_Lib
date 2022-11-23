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
// ===========
// Declaration
// ===========
struct XDeclaration : XNode
{
  // Constructors/Destructors
  explicit XDeclaration(XNode::Type nodeType = XNode::Type::declaration) : XNode(nodeType) {}
  XDeclaration(const XDeclaration &other) = delete;
  XDeclaration &operator=(XDeclaration &other) = delete;
  XDeclaration(XDeclaration &&other) = default;
  XDeclaration &operator=(XDeclaration &&other) = default;
  ~XDeclaration() = default;
  [[nodiscard]] bool isValidVersion() const { return (m_version == "1.0" || m_version == "1.1"); }
  [[nodiscard]] bool isValidEncoding() const { return (m_encoding == "UTF-8" || m_encoding == "UTF-16"); }
  [[nodiscard]] bool isValidStandalone() const { return (m_standalone == "yes" || m_standalone == "no"); }
  [[nodiscard]] std::string version() const { return (m_version); }
  void setVersion(const std::string &version) { m_version = version; }
  [[nodiscard]] std::string encoding() const { return (m_encoding); }
  void setEncoding(const std::string &encoding) { m_encoding = encoding; }
  [[nodiscard]] std::string standalone() const { return (m_standalone); }
  void setStandalone(const std::string &standalone) { m_standalone = standalone; }
private:
  std::string m_version{ "1.0" };
  std::string m_encoding{ "UTF-8" };
  std::string m_standalone{ "no" };
};
}// namespace XML_Lib