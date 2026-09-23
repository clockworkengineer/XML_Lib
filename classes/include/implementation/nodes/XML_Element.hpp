#pragma once

#include "common/XML_QName.hpp"
#include <limits>
#include <memory_resource>
#include <span>

namespace XML_Lib {

struct Element : Variant
{
  // Constructors/Destructors
  explicit Element(const std::string_view &name = "", const Type nodeType = Type::element)
    : Variant(nodeType), elementName(name),
      attributes(memoryResource()), namespaces(memoryResource())
  {}
  Element(const std::string_view &name,
    std::span<const XMLAttribute> attributes,
    std::span<const XMLAttribute> namespaces,
    const Type nodeType = Type::element)
    : Variant(nodeType), elementName(name),
      attributes(attributes.begin(), attributes.end(), memoryResource()),
      namespaces(namespaces.begin(), namespaces.end(), memoryResource())
  {
    for (const auto &attribute : attributes) {
      if (attribute.getName().starts_with("xmlns")) {
        std::string_view uri = attribute.getParsed();
        while (!uri.empty() && (uri.front() == ' ' || uri.front() == '\t' || uri.front() == '\r' || uri.front() == '\n')) {
          uri.remove_prefix(1);
        }
        while (!uri.empty() && (uri.back() == ' ' || uri.back() == '\t' || uri.back() == '\r' || uri.back() == '\n')) {
          uri.remove_suffix(1);
        }
        this->namespaces.emplace_back(attribute.getName().size() > 5 ? attribute.getName().substr(6) : ":",
          XMLValue{ attribute.getUnparsed(), std::string(uri) });
      }
    }
  }
  XML_LIB_NO_COPY_MOVE_DTOR(Element);
  // Is an attribute present?
  [[nodiscard]] bool hasAttribute(const std::string_view &attributeName) const
  {
    return XMLAttribute::contains(attributes, attributeName);
  }
  // Add an attribute
  void addAttribute(const std::string_view &name, const XMLValue &value) const { attributes.emplace_back(name, value); }
  // Return reference to an attribute list
  [[nodiscard]] const std::pmr::vector<XMLAttribute> &getAttributes() const { return attributes; }
  // Is namespace present?
  [[nodiscard]] bool hasNameSpace(const std::string_view &name) const
  {
    return XMLAttribute::contains(namespaces, name);
  }
  // Add a namespace
  void addNameSpace(const std::string_view &name, const XMLValue &value) const
  {
    namespaces.emplace_back(name, value);
    addAttribute(name, value);
  }
  [[nodiscard]] const XMLAttribute &getNameSpace(const std::string_view &name) const
  {
    return XMLAttribute::find(namespaces, name);
  }
  // Return reference to a namespace list
  [[nodiscard]] const std::pmr::vector<XMLAttribute> &getNameSpaces() const { return namespaces; }
  // Return reference to the element tag name
  [[nodiscard]] const std::string &name() const { return elementName; }
  // QName support: get namespace prefix (empty string if no prefix)
  [[nodiscard]] std::string getPrefix() const
  {
    return std::string(XML_Lib::getPrefix(elementName));
  }
  // QName support: get local name (without prefix)
  [[nodiscard]] std::string getLocalName() const
  {
    return std::string(XML_Lib::getLocalName(elementName));
  }
  // QName support: get namespace URI for this element (based on prefix and in-scope namespaces)
  [[nodiscard]] std::string getNamespaceURI() const
  {
    const auto prefix = getPrefix();
    const auto nsKey = prefix.empty() ? ":" : prefix;
    if (hasNameSpace(nsKey)) { return getNameSpace(nsKey).getParsed(); }
    return "";
  }
  // XElement Index overloads
  [[nodiscard]] const Element &operator[](int index) const;
  [[nodiscard]] Element &operator[](int index);
  [[nodiscard]] const XMLAttribute &operator[](const std::string_view &name) const;
  [[nodiscard]] XMLAttribute &operator[](const std::string_view &name);
  // Return Variant contents
  [[nodiscard]] std::string getContents() const override;

private:
  std::string elementName;
  mutable std::pmr::vector<XMLAttribute> attributes;
  mutable std::pmr::vector<XMLAttribute> namespaces;
  // Lazy content cache — invalidated when the child count changes.
  mutable std::string contentCache;
  mutable std::size_t contentCacheChildCount{ std::numeric_limits<std::size_t>::max() };
};
}// namespace XML_Lib