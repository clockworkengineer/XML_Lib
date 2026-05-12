#pragma once

// Suppress the rule-of-five boilerplate that every Variant subclass repeats.
// Deletes copy ctor/assignment, defaults move ctor/assignment, and overrides dtor.
#define XML_LIB_NO_COPY_MOVE_DTOR(TypeName)              \
  TypeName(const TypeName &) = delete;                   \
  TypeName &operator=(const TypeName &) = delete;        \
  TypeName(TypeName &&) = default;                       \
  TypeName &operator=(TypeName &&) = default;            \
  ~TypeName() override = default

#include <memory_resource>

namespace XML_Lib {

struct Node;  // forward declaration — full definition in node/XML_Node.hpp

struct Variant
{
  // Node Types
  enum class Type { base = 0, prolog, declaration, root, self, element, content, entity, comment, cdata, pi, dtd };

  // Constructors/Destructors
  explicit Variant(Type nodeType = Type::base, std::pmr::memory_resource *resource = std::pmr::get_default_resource());
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
  [[nodiscard]] std::pmr::vector<Node> &getChildren();
  [[nodiscard]] const std::pmr::vector<Node> &getChildren() const;
  // Reserve child storage when parse context predicts children
  void reserveChildren(size_t count);
  // Add child
  void addChild(Node &child) const;
  void addChild(Node &&child) const;
  // Return Variant contents
  [[nodiscard]] virtual std::string getContents() const { return ""; }

protected:
  // Return the memory resource used by this Variant's children storage.
  // Safe to call once the Variant base sub-object is fully constructed.
  [[nodiscard]] std::pmr::memory_resource *memoryResource() const noexcept;

private:
  // Variant type
  Type xmlNodeType{ Type::base };
  // Node children container (inline to improve locality)
  mutable std::pmr::vector<Node> children;
};

}// namespace XML_Lib