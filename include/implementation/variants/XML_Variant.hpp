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
  std::vector<std::unique_ptr<XNode>> &getChildren() { return (children); }
  const std::vector<std::unique_ptr<XNode>> &getChildren() const { return (children); }
  // Add child
  void addChild(std::unique_ptr<XNode> &child) { children.push_back(std::move(child)); }
  void addChild(std::unique_ptr<XNode> &&child) { children.push_back(std::move(child)); }
  // Return Variant contents
  [[nodiscard]] std::string getContents() const;

private:
  Variant::Type xmlNodeType;
  // XNode Children
  std::vector<std::unique_ptr<XNode>> children;
};

// // ==================
// // XNode get contents
// // ==================
// inline std::string Variant::getContents() const
// {
//   std::string result;
//   for (const auto &node : getChildren()) {
//     if (node->getType() == Variant::Type::content) {
//       result += XRef<XContent>(*node).getContent();
//     } else if (node->getType() == Variant::Type::entity) {
//       if (!XRef<XEntityReference>(*node).getChildren().empty()) {
//         result += node->getContents();
//       } else {
//         result += XRef<XEntityReference>(*node).value().getParsed();
//       }
//     } else if (node->getType() == Variant::Type::cdata) {
//       result += XRef<XCDATA>(*node).CDATA();
//     }
//   }
//   return (result);
// }
}// namespace XML_Lib