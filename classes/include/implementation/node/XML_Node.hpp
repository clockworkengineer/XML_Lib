#pragma once

#include <memory_resource>
#include <stdexcept>
#include <string>
#include <string_view>
#include "common/XML_Arena.hpp"

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
  explicit Node(Variant *variant, bool ownsVariant = true) noexcept
    : xmlVariant(variant), ownsVariant(ownsVariant)
  {}
  Node(const Node &other) = delete;
  Node &operator=(const Node &other) = delete;
  Node(Node &&other) noexcept
    : xmlVariant(other.xmlVariant), ownsVariant(other.ownsVariant)
  {
    other.xmlVariant = nullptr;
    other.ownsVariant = false;
  }
  Node &operator=(Node &&other) noexcept
  {
    if (this != &other) {
      if (xmlVariant && ownsVariant) { delete xmlVariant; }
      xmlVariant = other.xmlVariant;
      ownsVariant = other.ownsVariant;
      other.xmlVariant = nullptr;
      other.ownsVariant = false;
    }
    return *this;
  }
  ~Node()
  {
    if (xmlVariant && ownsVariant) { delete xmlVariant; }
  }
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
  // Reserve child storage when the parse context expects multiple children
  void reserveChildren(size_t count) { xmlVariant->reserveChildren(count); }
  // Get Node children reference
  [[nodiscard]] std::pmr::vector<Node> &getChildren() { return xmlVariant->getChildren(); }
  [[nodiscard]] const std::pmr::vector<Node> &getChildren() const { return xmlVariant->getChildren(); }
  // Get reference to Node  variant
  [[nodiscard]] Variant &getVariant() { return *xmlVariant; }
  [[nodiscard]] const Variant &getVariant() const { return *xmlVariant; }
  // Make Node
  template<typename T, typename... Args> static Node make(Args &&...args)
  {
#if defined(XML_LIB_EMBEDDED)
    if (XML_Arena *arena = XML_Arena::getCurrent()) {
      void *memory = arena->allocate(sizeof(T), alignof(T));
      return Node{ new (memory) T(std::forward<Args>(args)...), false };
    }
#endif
    return Node{ new T(std::forward<Args>(args)...), true };
  }

private:
  // Node Variant
  Variant *xmlVariant = nullptr;
  bool ownsVariant = true;
};
}// namespace XML_Lib