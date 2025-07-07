#pragma once

namespace XML_Lib {

struct Node
{
  // Node Error
  struct Error final : std::runtime_error
  {
    explicit Error(const std::string_view &message) : std::runtime_error(std::string("Node Error: ").append(message)) {}
  };
  // Constructors/Destructors
  Node() = default;
  template<typename T> explicit Node(T value);
  Node(const Node &other) = delete;
  Node &operator=(const Node &other) = delete;
  Node(Node &&other) = default;
  Node &operator=(Node &&other) = default;
  ~Node() = default;
  // Check what Node variant
  [[nodiscard]] bool isEmpty() const { return xmlVariant == nullptr; }
  [[nodiscard]] bool isNameable() const { return xmlVariant->isNameable(); }
  [[nodiscard]] bool isIndexable() const { return xmlVariant->isIndexable(); }
  // Return Node contents
  [[nodiscard]] std::string getContents() const { return xmlVariant->getContents(); }
  // Node Index overloads
  [[nodiscard]] const Node &operator[](int index) const;
  [[nodiscard]] const Node &operator[](const std::string_view &name) const;
  // Add child
  void addChild(Node &child) const { xmlVariant->addChild(child); }
  void addChild(Node &&child) const { xmlVariant->addChild(child); }
  // Get Node children reference
  [[nodiscard]] std::vector<Node> &getChildren() { return xmlVariant->getChildren(); }
  [[nodiscard]] const std::vector<Node> &getChildren() const { return xmlVariant->getChildren(); }
  // Get reference to Node  variant
  [[nodiscard]] Variant &getVariant() { return *xmlVariant; }
  [[nodiscard]] const Variant &getVariant() const { return *xmlVariant; }
  // Make Node
  template<typename T, typename... Args> static Node make(Args &&...args)
  {
    return Node{ std::make_unique<T>(std::forward<Args>(args)...) };
  }

private:
  // Node Variant
  std::unique_ptr<Variant> xmlVariant;
};
}// namespace XML_Lib