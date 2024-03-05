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
  [[nodiscard]] std::string getContents() const { return (xmlVariant->getContents()); }
  // XNode Index overloads
  [[nodiscard]] const XNode &operator[](int index) const;
  [[nodiscard]] const XNode &operator[](const std::string &name) const;
  // Add child
  void addChildren(XNode &child) { xmlVariant->addChildren(child); }
  void addChildren(XNode &&child) { xmlVariant->addChildren(child); }
  // Get XNode children reference
  std::vector<XNode> &getChildren() { return (xmlVariant->getChildren()); }
  const std::vector<XNode> &getChildren() const { return (xmlVariant->getChildren()); }
  // Get reference to XNode  variant
  Variant &getVariant() { return (*xmlVariant); }
  const Variant &getVariant() const { return (*xmlVariant); }
  // Make XNode
  template<typename T, typename... Args> static XNode make(Args &&...args)
  {
    return (XNode{ std::make_unique<T>(std::forward<Args>(args)...) });
  }

private:
  // XNode Variant
  std::unique_ptr<Variant> xmlVariant;
};
}// namespace XML_Lib