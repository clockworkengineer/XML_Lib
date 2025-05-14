#pragma once

namespace XML_Lib {

struct XNode
{
  // XNode Error
  struct Error final : std::runtime_error
  {
    explicit Error(const std::string_view &message) : std::runtime_error(std::string("XNode Error: ").append(message)) {}
  };
  // Constructors/Destructors
  XNode() = default;
  template<typename T> explicit XNode(T value);
  XNode(const XNode &other) = delete;
  XNode &operator=(const XNode &other) = delete;
  XNode(XNode &&other) = default;
  XNode &operator=(XNode &&other) = default;
  ~XNode() = default;
  // Check what XNode variant
  [[nodiscard]] bool isEmpty() const { return xmlVariant == nullptr; }
  [[nodiscard]] bool isNameable() const { return xmlVariant->isNameable(); }
  [[nodiscard]] bool isIndexable() const { return xmlVariant->isIndexable(); }
  // Return XNode contents
  [[nodiscard]] std::string getContents() const { return xmlVariant->getContents(); }
  // XNode Index overloads
  [[nodiscard]] const XNode &operator[](int index) const;
  [[nodiscard]] const XNode &operator[](const std::string_view &name) const;
  // Add child
  void addChild(XNode &child) const { xmlVariant->addChild(child); }
  void addChild(XNode &&child) const { xmlVariant->addChild(child); }
  // Get XNode children reference
  [[nodiscard]] std::vector<XNode> &getChildren() { return xmlVariant->getChildren(); }
  [[nodiscard]] const std::vector<XNode> &getChildren() const { return xmlVariant->getChildren(); }
  // Get reference to XNode  variant
  [[nodiscard]] Variant &getVariant() { return *xmlVariant; }
  [[nodiscard]] const Variant &getVariant() const { return *xmlVariant; }
  // Make XNode
  template<typename T, typename... Args> static XNode make(Args &&...args)
  {
    return XNode{ std::make_unique<T>(std::forward<Args>(args)...) };
  }

private:
  // XNode Variant
  std::unique_ptr<Variant> xmlVariant;
};
}// namespace XML_Lib