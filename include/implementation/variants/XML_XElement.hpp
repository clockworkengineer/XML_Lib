#pragma once

#include <string>
#include <algorithm>

namespace XML_Lib {

struct XElement : Variant
{
  // Constructors/Destructors
  XElement(Variant::Type nodeType = Variant::Type::element) : Variant(nodeType) {}
  XElement(const std::string &name,
    const std::vector<XMLAttribute> &attributes,
    const std::vector<XMLAttribute> &namespaces,
    Variant::Type nodeType = Variant::Type::element)
    : Variant(nodeType), elementName(name), attributes(attributes), namespaces(namespaces)
  {}
  XElement(const XElement &other) = delete;
  XElement &operator=(const XElement &other) = delete;
  XElement(XElement &&other) = default;
  XElement &operator=(XElement &&other) = default;
  ~XElement() = default;
  // Is an attribute present ?
  [[nodiscard]] bool isAttributePresent(const std::string &name) const
  {
    return (std::find_if(attributes.rbegin(), attributes.rend(), [&name](const XMLAttribute &attr) {
      return (attr.getName() == name);
    }) != attributes.rend());
  }
  // Add an attribute
  void addAttribute(const std::string &name, const XMLValue &value) const { attributes.emplace_back(name, value); }
  [[nodiscard]] const XMLAttribute &getAttribute(const std::string &name) const
  {
    return (*std::find_if(
      attributes.rbegin(), attributes.rend(), [&name](const XMLAttribute &attr) { return (attr.getName() == name); }));
  }
  // Return reference to attribute list
  [[nodiscard]] const std::vector<XMLAttribute> &getAttributeList() const { return (attributes); }
  [[nodiscard]] bool isNameSpacePresent(const std::string &name) const
  {
    return (std::find_if(namespaces.rbegin(), namespaces.rend(), [&name](const XMLAttribute &attr) {
      return (attr.getName() == name);
    }) != namespaces.rend());
  }
  // Is a namespace present ?
  [[nodiscard]] const XMLAttribute &getNameSpace(const std::string &name) const
  {
    return (*std::find_if(
      namespaces.rbegin(), namespaces.rend(), [&name](const XMLAttribute &ns) { return (ns.getName() == name); }));
  }
  // Return reference to namespace list
  [[nodiscard]] const std::vector<XMLAttribute> &getNamespaceList() const { return (namespaces); }
  // Return reference to element tag name
  [[nodiscard]] const std::string &name() const { return (elementName); }
  // XElement Index overloads
  [[nodiscard]] const XElement &operator[](int index) const;
  [[nodiscard]] const XElement &operator[](const std::string &name) const;

private:
  std::string elementName;
  mutable std::vector<XMLAttribute> attributes;
  std::vector<XMLAttribute> namespaces;
};
}// namespace XML_Lib