#pragma once
// =======
// C++ STL
// =======
#include <string>
// =================
// LIBRARY NAMESPACE
// =================
namespace XML_Lib {
// ==
// PI
// ==
struct XPI : XNode
{
  // Constructors/Destructors
  XPI(const std::string &name, const std::string &parameters)
    : XNode(XNode::Type::pi), m_name(name), m_parameters(parameters)
  {}
  XPI(const XPI &other) = delete;
  XPI &operator=(XPI &other) = delete;
  XPI(XPI &&other) = default;
  XPI &operator=(XPI &&other) = default;
  ~XPI() = default;
  // Return reference to name
  [[nodiscard]] const std::string &name() const { return (m_name); }
  // Return reference to parameters
  [[nodiscard]] const std::string &parameters() const { return (m_parameters); }

private:
  std::string m_name;
  std::string m_parameters;
};
}// namespace XML_Lib