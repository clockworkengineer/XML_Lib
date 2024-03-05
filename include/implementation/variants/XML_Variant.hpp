#pragma once
#include <vector>
namespace XML_Lib {
// ====
// Base
// ====
struct Variant
{
  // XNode Types
  enum class Type { base = 0, prolog, declaration, root, self, element, content, entity, comment, cdata, pi, dtd };
  // Constructors/Destructors
  explicit Variant(Variant::Type nodeType = Variant::Type::base) : xmlNodeType(nodeType) {}
  Variant(const Variant &other) = default;
  Variant &operator=(const Variant &other) = default;
  Variant(Variant &&other) = default;
  Variant &operator=(Variant &&other) = default;
  virtual ~Variant() = default;

  // Get/Set XNode type
  [[nodiscard]] Type getType() const { return (xmlNodeType); }
  // Get XNode children reference
  std::vector<XNode> &getChildren() { return (children); }
  const std::vector<XNode> &getChildren() const { return (children); }
  // Add child
  void addChild(XNode &child) { children.push_back(std::move(child)); }
  void addChild(XNode &&child) { children.push_back(std::move(child)); }
  // Return Variant contents
  [[nodiscard]] std::string getContents() const;

private:
  // Variant type
  Variant::Type xmlNodeType;
  // XNode Children
  std::vector<XNode> children;
};

}// namespace XML_Lib