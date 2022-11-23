#pragma once
// =======
// C++ STL
// =======
#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
// =================
// LIBRARY NAMESPACE
// =================
namespace XML_Lib {
// ====
// Base
// ====
struct XNode
{
  // XNode Error
  struct Error : public std::runtime_error
  {
    Error(const std::string &message) : std::runtime_error("XNode Error: " + message) {}
  };
  // XNode Types
  enum class Type { base = 0, prolog, declaration, root, self, element, content, entity, comment, cdata, pi, dtd };
  // Constructors/Destructors
  explicit XNode(Type nodeType = Type::base) : m_type(nodeType) {}
  XNode(const XNode &other) = delete;
  XNode &operator=(XNode &other) = delete;
  XNode(XNode &&other) = default;
  XNode &operator=(XNode &&other) = default;
  ~XNode() = default;
  // Get/Set XNode type
  [[nodiscard]] Type getType() const { return (m_type); }
  void setType(Type nodeType) { m_type = nodeType; }
  // Return XNode contents
  [[nodiscard]] std::string getContents() const;
  // XNode Index overloads
  XNode &operator[](int index) const;
  XNode &operator[](const std::string &name) const;
  // Add child
  void addChild(std::unique_ptr<XNode> &child)
  {
    if (m_children == nullptr) { m_children = std::make_unique<std::vector<std::unique_ptr<XNode>>>(); }
    m_children->push_back(std::move(child));
  }
  void addChild(std::unique_ptr<XNode> &&child)
  {
    if (m_children == nullptr) { m_children = std::make_unique<std::vector<std::unique_ptr<XNode>>>(); }
    m_children->push_back(std::move(child));
  }
  // Get XNode children reference
  std::vector<std::unique_ptr<XNode>> &getChildren()
  {
    if (m_children == nullptr) {
      return (no_children);
    } else {
      return (*m_children);
    }
  };
  const std::vector<std::unique_ptr<XNode>> &getChildren() const
  {
    if (m_children == nullptr) {
      return (no_children);
    } else {
      return (*m_children);
    }
  };

private:
  // XNode Type
  Type m_type;
  // XNode Children
  std::unique_ptr<std::vector<std::unique_ptr<XNode>>> m_children;
  // Default XNode has no children
  inline static std::vector<std::unique_ptr<XNode>> no_children;
};
}// namespace XML_Lib