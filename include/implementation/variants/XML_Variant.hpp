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
  // Check what XNode variant
  [[nodiscard]] bool isIndexable() const { return (xmlNodeType <= Type::element); }
  [[nodiscard]] bool isProlog() const { return (xmlNodeType == Type::prolog); }
  [[nodiscard]] bool isDeclaration() const { return (xmlNodeType == Type::declaration); }
  [[nodiscard]] bool isRoot() const { return (xmlNodeType == Type::root); }
  [[nodiscard]] bool isSelf() const { return (xmlNodeType == Type::self); }
  [[nodiscard]] bool isElement() const { return (xmlNodeType == Type::element); }
  [[nodiscard]] bool isContent() const { return (xmlNodeType == Type::content); }
  [[nodiscard]] bool isEntity() const { return (xmlNodeType == Type::entity); }
  [[nodiscard]] bool isComment() const { return (xmlNodeType == Type::comment); }
  [[nodiscard]] bool isCDATA() const { return (xmlNodeType == Type::cdata); }
  [[nodiscard]] bool isPI() const { return (xmlNodeType == Type::pi); }
  [[nodiscard]] bool isDTD() const { return (xmlNodeType == Type::dtd); }
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