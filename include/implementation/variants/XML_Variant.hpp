#pragma once
#include <vector>
namespace XML_Lib {
// ====
// Base
// ====
struct Variant
{
  // XNode Types
protected:
  enum class Type { base = 0, prolog, declaration, root, self, element, content, entity, comment, cdata, pi, dtd };

public:
  // Constructors/Destructors
  explicit Variant(const Variant::Type nodeType = Variant::Type::base) : xmlNodeType(nodeType) {}
  Variant(const Variant &other) = delete;
  Variant &operator=(const Variant &other) = delete;
  Variant(Variant &&other) = default;
  Variant &operator=(Variant &&other) = default;
  virtual ~Variant() = default;
  // Check what XNode variant
  [[nodiscard]] bool isNameable() const { return xmlNodeType >= Type::root && xmlNodeType <= Type::element; }
  [[nodiscard]] bool isIndexable() const { return xmlNodeType <= Type::element; }
  [[nodiscard]] bool isProlog() const { return xmlNodeType == Type::prolog; }
  [[nodiscard]] bool isDeclaration() const { return xmlNodeType == Type::declaration; }
  [[nodiscard]] bool isRoot() const { return xmlNodeType == Type::root; }
  [[nodiscard]] bool isSelf() const { return xmlNodeType == Type::self; }
  [[nodiscard]] bool isElement() const { return xmlNodeType == Type::element; }
  [[nodiscard]] bool isContent() const { return xmlNodeType == Type::content; }
  [[nodiscard]] bool isEntity() const { return xmlNodeType == Type::entity; }
  [[nodiscard]] bool isComment() const { return xmlNodeType == Type::comment; }
  [[nodiscard]] bool isCDATA() const { return xmlNodeType == Type::cdata; }
  [[nodiscard]] bool isPI() const { return xmlNodeType == Type::pi; }
  [[nodiscard]] bool isDTD() const { return xmlNodeType == Type::dtd; }
  // Get XNode children reference
  [[nodiscard]] std::vector<XNode> &getChildren();
  [[nodiscard]] const std::vector<XNode> &getChildren() const;
  // Add child
  void addChild(XNode &child) const;
  void addChild(XNode &&child) const;
  // Return Variant contents
  [[nodiscard]] virtual std::string getContents() const { return ""; }

private:
  // Variant type
  Variant::Type xmlNodeType{ Type::base };
  // XNode Children
  mutable std::unique_ptr<std::vector<XNode>> children;
};

}// namespace XML_Lib