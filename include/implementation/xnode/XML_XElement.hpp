#pragma once
// =======
// C++ STL
// =======
#include <string>
#include <algorithm>
// ========
// =================
// LIBRARY NAMESPACE
// =================
namespace XML_Lib {
// ===================================================
// Forward declarations for interfaces/classes/structs
// ===================================================
struct XMLAttribute;
struct XMLValue;
// =======
// Element
// =======
struct XElement : XNode
{
  // Constructors/Destructors
  explicit XElement(XNode::Type nodeType = XNode::Type::element) : XNode(nodeType) {}
  explicit XElement(const std::string &name) : XElement() { m_name = name; }
  XElement(const XElement &other) = delete;
  XElement &operator=(XElement &other) = delete;
  XElement(XElement &&other) = default;
  XElement &operator=(XElement &&other) = default;
  ~XElement() = default;
  [[nodiscard]] bool isAttributePresent(const std::string &name) const
  {
    return (std::find_if(m_attributes.rbegin(), m_attributes.rend(), [&name](const XMLAttribute &attr) {
      return (attr.name == name);
    }) != m_attributes.rend());
  }
  void addAttribute(const std::string &name, const XMLValue &value) const { m_attributes.emplace_back(name, value); }
  [[nodiscard]] XMLAttribute getAttribute(const std::string &name) const
  {
    return (*std::find_if(
      m_attributes.rbegin(), m_attributes.rend(), [&name](const XMLAttribute &attr) { return (attr.name == name); }));
  }
  [[nodiscard]] const XMLAttribute::List &getAttributeList() const { return (m_attributes); }
  [[nodiscard]] bool isNameSpacePresent(const std::string &name) const
  {
    return (std::find_if(m_namespaces.rbegin(), m_namespaces.rend(), [&name](const XMLAttribute &attr) {
      return (attr.name == name);
    }) != m_namespaces.rend());
  }
  void addNameSpace(const std::string &name, const XMLValue &value) { m_namespaces.emplace_back(name, value); }
  [[nodiscard]] XMLAttribute getNameSpace(const std::string &name) const
  {
    return (*std::find_if(
      m_namespaces.rbegin(), m_namespaces.rend(), [&name](const XMLAttribute &ns) { return (ns.name == name); }));
  }
  [[nodiscard]] const XMLAttribute::List &getNameSpaceList() const { return (m_namespaces); }
  [[nodiscard]] std::string name() const { return (m_name); }
  void setName(const std::string &name) { m_name = name; }
  XElement &operator[](int index) const;
  XElement &operator[](const std::string &name) const;

private:
  std::string m_name;
  XMLAttribute::List m_namespaces;
  mutable XMLAttribute::List m_attributes;
};
}// namespace XML_Lib