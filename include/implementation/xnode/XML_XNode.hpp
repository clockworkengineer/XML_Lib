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
  // XNode Types
  enum class Type { base = 0, prolog, declaration, root, self, element, content, entity, comment, cdata, pi, dtd };
  // Constructors/Destructors
  explicit XNode(Type nodeType = Type::base) : m_type(nodeType) {}
  XNode(const XNode &other) = delete;
  XNode &operator=(const XNode &other) = delete;
  XNode(XNode &&other) = default;
  XNode &operator=(XNode &&other) = default;
  ~XNode() = default;
  // Get/Set XNode type
  [[nodiscard]] Type getType() const { return (m_type); }
  // Return XNode contents
  [[nodiscard]] std::string getContents() const;
  // XNode Index overloads
  [[nodiscard]] const XNode &operator[](int index) const;
  [[nodiscard]] const XNode &operator[](const std::string &name) const;
  // Add child
  void addChild(std::unique_ptr<XNode> &child);
  void addChild(std::unique_ptr<XNode> &&child);
  // Get XNode children reference
  std::vector<std::unique_ptr<XNode>> &getChildren();
  const std::vector<std::unique_ptr<XNode>> &getChildren() const;
  // Make XNode
  template<typename T, typename... Args> static std::unique_ptr<T> make(Args &&...args)
  {
    return (std::make_unique<T>(std::forward<Args>(args)...));
  }

private:
  // XNode Type
  Type m_type;
  // XNode Children
  std::unique_ptr<std::vector<std::unique_ptr<XNode>>> m_children;
  // Default XNode has no children
  inline static std::vector<std::unique_ptr<XNode>> no_children;
};
}// namespace XML_Lib