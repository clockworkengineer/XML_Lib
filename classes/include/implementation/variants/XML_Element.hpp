#pragma once

namespace XML_Lib {

struct Element : Variant
{
  // Constructors/Destructors
  explicit Element(const std::string_view &name = "", const Type nodeType = Type::element)
    : Variant(nodeType), elementName(name)
  {}
  Element(const std::string_view &name,
    const std::vector<XMLAttribute> &attributes,
    const std::vector<XMLAttribute> &namespaces,
    const Type nodeType = Type::element)
    : Variant(nodeType), elementName(name), attributes(attributes), namespaces(namespaces)
  {
    for (const auto &attribute : attributes) {
      if (attribute.getName().starts_with("xmlns")) {
        this->namespaces.emplace_back(attribute.getName().size() > 5 ? attribute.getName().substr(6) : ":",
          XMLValue{ attribute.getUnparsed(), attribute.getParsed() });
      }
    }
  }
  Element(const Element &other) = delete;
  Element &operator=(const Element &other) = delete;
  Element(Element &&other) = default;
  Element &operator=(Element &&other) = default;
  ~Element() override = default;
  // Is an attribute present?
  [[nodiscard]] bool hasAttribute(const std::string_view &attributeName) const
  {
    return XMLAttribute::contains(attributes, attributeName);
  }
  // Add an attribute
  void addAttribute(const std::string_view &name, const XMLValue &value) const { attributes.emplace_back(name, value); }
  // Return reference to an attribute list
  [[nodiscard]] const std::vector<XMLAttribute> &getAttributes() const { return attributes; }
  // Is namespace present?
  [[nodiscard]] bool hasNameSpace(const std::string_view &name) const { return XMLAttribute::contains(namespaces, name); }
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
  [[nodiscard]] const std::vector<XMLAttribute> &getNameSpaces() const { return namespaces; }
  // Return reference to the element tag name
  [[nodiscard]] const std::string &name() const { return elementName; }
  // XElement Index overloads
  [[nodiscard]] const Element &operator[](int index) const;
  [[nodiscard]] Element &operator[](int index);
  [[nodiscard]] const XMLAttribute &operator[](const std::string_view &name) const;
  [[nodiscard]] XMLAttribute &operator[](const std::string_view &name);
  // Return Variant contents
  [[nodiscard]] std::string getContents() const override;

private:
  std::string elementName;
  mutable std::vector<XMLAttribute> attributes;
  mutable std::vector<XMLAttribute> namespaces;
};
}// namespace XML_Lib