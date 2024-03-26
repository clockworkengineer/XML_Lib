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
  // Check what XNode variant
  [[nodiscard]] bool isEmpty() const { return (xmlVariant.get() == nullptr); }
  [[nodiscard]] bool isIndexable() const { return (xmlVariant->isIndexable()); }
  [[nodiscard]] bool isProlog() const { return (xmlVariant->isProlog()); }
  [[nodiscard]] bool isDeclaration() const { return (xmlVariant->isDeclaration()); }
  [[nodiscard]] bool isRoot() const { return (xmlVariant->isRoot()); }
  [[nodiscard]] bool isSelf() const { return (xmlVariant->isSelf()); }
  [[nodiscard]] bool isElement() const { return (xmlVariant->isElement()); }
  [[nodiscard]] bool isContent() const { return (xmlVariant->isContent()); }
  [[nodiscard]] bool isEntity() const { return (xmlVariant->isEntity()); }
  [[nodiscard]] bool isComment() const { return (xmlVariant->isComment()); }
  [[nodiscard]] bool isCDATA() const { return (xmlVariant->isCDATA()); }
  [[nodiscard]] bool isPI() const { return (xmlVariant->isPI()); }
  [[nodiscard]] bool isDTD() const { return (xmlVariant->isDTD()); }
  // Return XNode contents
  [[nodiscard]] std::string getContents() const { return (xmlVariant->getContents()); }
  // XNode Index overloads
  [[nodiscard]] const XNode &operator[](int index) const;
  [[nodiscard]] const XNode &operator[](const std::string &name) const;
  // Add child
  void addChild(XNode &child) { xmlVariant->addChildren(child); }
  void addChild(XNode &&child) { xmlVariant->addChildren(child); }
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