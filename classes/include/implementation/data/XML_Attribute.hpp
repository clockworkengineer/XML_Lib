#pragma once

namespace XML_Lib {

struct XMLAttribute : XMLValue
{
  // XMLAttribute Error
  struct Error final : std::runtime_error
  {
    explicit Error(const std::string &message) : std::runtime_error("Attribute Error: " + message) {}
  };
  // Constructors/Destructors
  XMLAttribute(std::string name, const XMLValue &value) : XMLValue(value), name(std::move(name)) {}
  XMLAttribute() = delete;
  XMLAttribute(const XMLAttribute &other) = default;
  XMLAttribute &operator=(const XMLAttribute &other) = default;
  XMLAttribute(XMLAttribute &&other) = default;
  XMLAttribute &operator=(XMLAttribute &&other) = default;
  XMLAttribute &operator=(const XMLValue &other) override
  {
    setValue(other.getUnparsed(), other.getParsed());
    return *this;
  };
  ~XMLAttribute() = default;
  // Get attribute name
  [[nodiscard]] const std::string &getName() const { return name; }
  // Search for an attribute in vector of unique attributes
  [[nodiscard]] static bool contains(const std::vector<XMLAttribute> &attributes, const std::string &name);
  // Return attribute entry
  [[nodiscard]] static XMLAttribute &find(std::vector<XMLAttribute> &attributes, const std::string &name);

private:
  // Attribute name
  std::string name;
};

[[nodiscard]] inline bool XMLAttribute::contains(const std::vector<XMLAttribute> &attributes, const std::string &name)
{
  return std::find_if(attributes.rbegin(), attributes.rend(), [&name](const XMLAttribute &attr) {
    return attr.getName() == name;
  }) != attributes.rend();
}

[[nodiscard]] inline XMLAttribute &XMLAttribute::find(std::vector<XMLAttribute> &attributes, const std::string &name)
{
  auto attribute = std::find_if(
    attributes.rbegin(), attributes.rend(), [&name](const XMLAttribute &attr) { return attr.getName() == name; });
  if (attribute != attributes.rend()) return *attribute;
  throw Error("Attribute '" + name + "' does not exist.");
}
}// namespace XML_Lib