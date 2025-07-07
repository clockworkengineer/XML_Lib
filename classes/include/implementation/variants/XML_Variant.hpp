#pragma once

namespace XML_Lib {

struct Variant
{
  // Node Types
  enum class Type { base = 0, prolog, declaration, root, self, element, content, entity, comment, cdata, pi, dtd };

  // Constructors/Destructors
  explicit Variant(Type nodeType = Type::base);
  Variant(const Variant &other) = delete;
  Variant &operator=(const Variant &other) = delete;
  Variant(Variant &&other) = default;
  Variant &operator=(Variant &&other) = default;
  virtual ~Variant() = default;
  // Check what Node variant
  [[nodiscard]] bool isNameable() const { return xmlNodeType >= Type::root && xmlNodeType <= Type::element; }
  [[nodiscard]] bool isIndexable() const { return xmlNodeType > Type::base && xmlNodeType <= Type::element; }
  // Get Node type
  [[nodiscard]] Type getNodeType() const {
    return xmlNodeType;
  }
  // Get Node children reference
  [[nodiscard]] std::vector<Node> &getChildren();
  [[nodiscard]] const std::vector<Node> &getChildren() const;
  // Add child
  void addChild(Node &child) const;
  void addChild(Node &&child) const;
  // Return Variant contents
  [[nodiscard]] virtual std::string getContents() const { return ""; }

private:
  // Variant type
  Type xmlNodeType{ Type::base };
  // Node Children
  mutable std::unique_ptr<std::vector<Node>> children;
};

}// namespace XML_Lib