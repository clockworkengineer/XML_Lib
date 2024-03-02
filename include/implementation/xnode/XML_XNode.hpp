#pragma once

#include <vector>
#include <string>
#include <memory>
#include <stdexcept>

namespace XML_Lib {

struct XNode
{
  // XNode Error
  struct Error : public std::runtime_error
  {
    Error(const std::string &message) : std::runtime_error("XNode Error: " + message) {}
  };
  // // XNode Types
  // enum class Type { base = 0, prolog, declaration, root, self, element, content, entity, comment, cdata, pi, dtd };
  // Constructors/Destructors
  XNode() = default;
  template<typename T> explicit XNode(T value);
  XNode(const XNode &other) = delete;
  XNode &operator=(const XNode &other) = delete;
  XNode(XNode &&other) = default;
  XNode &operator=(XNode &&other) = default;
  ~XNode() = default;
  // Get XNode type
  [[nodiscard]] Variant::Type getType() const { return (xmlVariant->getType()); }
  // Return XNode contents
  [[nodiscard]] std::string getContents() const;
  // XNode Index overloads
  [[nodiscard]] const XNode &operator[](int index) const;
  [[nodiscard]] const XNode &operator[](const std::string &name) const;
  // Add child
  void addChild(std::unique_ptr<XNode> &child) { xmlVariant->addChild(child); }
  void addChild(std::unique_ptr<XNode> &&child) { xmlVariant->addChild(child); }
  // Get XNode children reference
  std::vector<std::unique_ptr<XNode>> &getChildren() { return (xmlVariant->getChildren()); }
  const std::vector<std::unique_ptr<XNode>> &getChildren() const { return (xmlVariant->getChildren()); }
  // Get reference to JNode variant
  Variant &getVariant() { return (*xmlVariant); }
  const Variant &getVariant() const { return (*xmlVariant); }
  // Make XNode
  template<typename T, typename... Args> static std::unique_ptr<XNode> make(Args &&...args)
  {
    return (std::make_unique<XNode>(std::make_unique<T>(std::forward<Args>(args)...)));
  }

private:
  // XNode Type
  std::unique_ptr<Variant> xmlVariant;
  // XNode Children
  // std::vector<std::unique_ptr<XNode>> children;
};
}// namespace XML_Lib