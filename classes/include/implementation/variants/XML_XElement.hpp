#pragma once

#include <string>
#include <algorithm>

namespace XML_Lib {

struct XElement : Variant
{
  // Constructors/Destructors
  explicit XElement(const Type nodeType = Type::element) : Variant(nodeType) {}
  XElement(const std::string &name,
    const std::vector<XMLAttribute> &attributes,
    const std::vector<XMLAttribute> &namespaces,
    const Type nodeType = Type::element)
    : Variant(nodeType), elementName(name), attributes(attributes), namespaces(namespaces)
  {}
  XElement(const XElement &other) = delete;
  XElement &operator=(const XElement &other) = delete;
  XElement(XElement &&other) = default;
  XElement &operator=(XElement &&other) = default;
  ~XElement() override = default;
  // Is an attribute present ?
  [[nodiscard]] bool isAttributePresent(const std::string &attributeName) const
  {
    return XMLAttribute::isAttrubutePresent(attributes, attributeName);
  }
  // Add an attribute
  void addAttribute(const std::string &name, const XMLValue &value) const { attributes.emplace_back(name, value); }
  // Return reference to attribute list
  [[nodiscard]] const std::vector<XMLAttribute> &getAttributeList() const { return attributes; }
  // Is namespace present  ?
  [[nodiscard]] bool isNameSpacePresent(const std::string &name) const
  {
    return XMLAttribute::isAttrubutePresent(namespaces, name);
  }
  // Add an namespace
  void addNameSpace(const std::string &name, const XMLValue &value) const { namespaces.emplace_back(name, value); }
  // Is a namespace present ?
  [[nodiscard]] const XMLAttribute &getNameSpace(const std::string &name) const
  {
    return *std::find_if(
      namespaces.rbegin(), namespaces.rend(), [&name](const XMLAttribute &ns) { return ns.getName() == name; });
  }
  // Return reference to namespace list
  [[nodiscard]] const std::vector<XMLAttribute> &getNamespaceList() const { return namespaces; }
  // Return reference to element tag name
  [[nodiscard]] const std::string &name() const { return elementName; }
  // XElement Index overloads
  [[nodiscard]] const XElement &operator[](int index) const;
  [[nodiscard]] const XMLAttribute &operator[](const std::string &name) const;
  // Return Variant contents
  [[nodiscard]] std::string getContents() const override
  {
    std::string result;
    for (const auto &xNode : getChildren()) { result += xNode.getContents(); }
    return result;
  }

private:
  std::string elementName;
  mutable std::vector<XMLAttribute> attributes;
  mutable std::vector<XMLAttribute> namespaces;
};
}// namespace XML_Lib