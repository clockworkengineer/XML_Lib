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
// =======
// Element
// =======
struct XElement : XNode
{
  // Constructors/Destructors
  XElement(XNode::Type nodeType = XNode::Type::element) : XNode(nodeType) {}
  XElement(const std::string &name,
    const std::vector<XMLAttribute> &attributes,
    const std::vector<XMLAttribute> &namespaces,
    XNode::Type nodeType = XNode::Type::element)
    : XNode(nodeType), m_name(name), m_attributes(attributes), m_namespaces(namespaces)
  {}
  XElement(const XElement &other) = delete;
  XElement &operator=(XElement &other) = delete;
  XElement(XElement &&other) = default;
  XElement &operator=(XElement &&other) = default;
  ~XElement() = default;
  // Is an attribute present ?
  [[nodiscard]] bool isAttributePresent(const std::string &name) const
  {
    return (std::find_if(m_attributes.rbegin(), m_attributes.rend(), [&name](const XMLAttribute &attr) {
      return (attr.name == name);
    }) != m_attributes.rend());
  }
  // Add an attribute
  void addAttribute(const std::string &name, const XMLValue &value) const { m_attributes.emplace_back(name, value); }
  [[nodiscard]] const XMLAttribute &getAttribute(const std::string &name) const
  {
    return (*std::find_if(
      m_attributes.rbegin(), m_attributes.rend(), [&name](const XMLAttribute &attr) { return (attr.name == name); }));
  }
  // Return reference to attribute list
  [[nodiscard]] const std::vector<XMLAttribute> &getAttributeList() const { return (m_attributes); }
  [[nodiscard]] bool isNameSpacePresent(const std::string &name) const
  {
    return (std::find_if(m_namespaces.rbegin(), m_namespaces.rend(), [&name](const XMLAttribute &attr) {
      return (attr.name == name);
    }) != m_namespaces.rend());
  }
  // Is a namespace present ?
  [[nodiscard]] const XMLAttribute &getNameSpace(const std::string &name) const
  {
    return (*std::find_if(
      m_namespaces.rbegin(), m_namespaces.rend(), [&name](const XMLAttribute &ns) { return (ns.name == name); }));
  }
  // Return reference to namespace list
  [[nodiscard]] const std::vector<XMLAttribute> &getNamespaceList() const { return (m_namespaces); }
  // Return reference to element tag name
  [[nodiscard]] const std::string &name() const { return (m_name); }
  // XElement Index overloads
  [[nodiscard]] const XElement &operator[](int index) const;
  [[nodiscard]] const XElement &operator[](const std::string &name) const;

private:
  std::string m_name;
  mutable std::vector<XMLAttribute> m_attributes;
  std::vector<XMLAttribute> m_namespaces;
};
}// namespace XML_Lib