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
  [[nodiscard]] bool isProlog() { return (xmlNodeType == Type::prolog); }
  [[nodiscard]] bool isDeclaration() { return (xmlNodeType == Type::declaration); }
  [[nodiscard]] bool isRoot() { return (xmlNodeType == Type::root); }
  [[nodiscard]] bool isSelf() { return (xmlNodeType == Type::self); }
  [[nodiscard]] bool isElement() { return (xmlNodeType == Type::element); }
  [[nodiscard]] bool isContent() { return (xmlNodeType == Type::content); }
  [[nodiscard]] bool isEntity() { return (xmlNodeType == Type::entity); }
  [[nodiscard]] bool isComment() { return (xmlNodeType == Type::comment); }
  [[nodiscard]] bool isCDATA() { return (xmlNodeType == Type::cdata); }
  [[nodiscard]] bool isPI() { return (xmlNodeType == Type::pi); }
  [[nodiscard]] bool isDTD() { return (xmlNodeType == Type::dtd); }
  // Get XNode children reference
  std::vector<XNode> &getChildren() { return (children); }
  const std::vector<XNode> &getChildren() const { return (children); }
  // Add child
  void addChildren(XNode &child) { children.push_back(std::move(child)); }
  void addChildren(XNode &&child) { children.push_back(std::move(child)); }
  // Return Variant contents
  [[nodiscard]] std::string getContents() const;

private:
  // Variant type
  Variant::Type xmlNodeType;
  // XNode Children
  std::vector<XNode> children;
};

}// namespace XML_Lib