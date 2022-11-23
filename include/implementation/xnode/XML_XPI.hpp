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
// ==
// PI
// ==
struct XPI : XNode
{
  // Constructors/Destructors
  explicit XPI(XNode::Type nodeType = XNode::Type::pi) : XNode(nodeType) {}
  XPI(const XPI &other) = delete;
  XPI &operator=(XPI &other) = delete;
  XPI(XPI &&other) = default;
  XPI &operator=(XPI &&other) = default;
  ~XPI() = default;
  [[nodiscard]] std::string name() const { return (m_name); }
  void setName(const std::string &name) { m_name = name; }
  [[nodiscard]] std::string parameters() const { return (m_parameters); }
  void setParameters(const std::string &parameters) { m_parameters = parameters; }

private:
  std::string m_name;
  std::string m_parameters;
};
}// namespace XML_Lib