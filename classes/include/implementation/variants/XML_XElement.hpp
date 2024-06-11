#pragma once

#include <string>
#include <algorithm>
#include <utility>

namespace XML_Lib {

struct XElement : Variant
{
  // Constructors/Destructors
  explicit XElement(std::string name="",const Type nodeType = Type::element) : Variant(nodeType), elementName(std::move(name)) {}
  XElement(std::string name,
    const std::vector<XMLAttribute> &attributes,
    const std::vector<XMLAttribute> &namespaces,
    const Type nodeType = Type::element)
    : Variant(nodeType), elementName(std::move(name)), attributes(attributes), namespaces(namespaces)
  {}
  XElement(const XElement &other) = delete;
  XElement &operator=(const XElement &other) = delete;
  XElement(XElement &&other) = default;
  XElement &operator=(XElement &&other) = default;
  ~XElement() override = default;
  // Is an attribute present ?
  [[nodiscard]] bool hasAttribute(const std::string &attributeName) const
  {
    return XMLAttribute::contains(attributes, attributeName);
  }
  // Add an attribute
  void addAttribute(const std::string &name, const XMLValue &value) const { attributes.emplace_back(name, value); }
  // Return reference to attribute list
  [[nodiscard]] const std::vector<XMLAttribute> &getAttributes() const { return attributes; }
  // Is namespace present  ?
  [[nodiscard]] bool hasNameSpace(const std::string &name) const
  {
    return XMLAttribute::contains(namespaces, name);
  }
  // Add an namespace
  void addNameSpace(const std::string &name, const XMLValue &value) const { namespaces.emplace_back(name, value); }
  // Is a namespace present ?
  [[nodiscard]] const XMLAttribute &getNameSpace(const std::string &name) const
  {
    return XMLAttribute::find(namespaces, name);
  }
  // Return reference to namespace list
  [[nodiscard]] const std::vector<XMLAttribute> &getNameSpaces() const { return namespaces; }
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