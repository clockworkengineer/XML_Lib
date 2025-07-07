//
// Class: XML_Variant
//
// Description: XML entity reference mapper.
//
// Dependencies: C++20 - Language standard features used.
//

#include "XML_Impl.hpp"

namespace XML_Lib {
void Variant::addChild(Node &child) const
{
  if (children == nullptr) { children = std::make_unique<std::vector<Node>>(); }
  children->push_back(std::move(child));
}
void Variant::addChild(Node &&child) const
{
  if (children == nullptr) { children = std::make_unique<std::vector<Node>>(); }
  children->push_back(std::move(child));
}
// Get Node children reference
std::vector<Node> &Variant::getChildren()
{
  if (children == nullptr) { children = std::make_unique<std::vector<Node>>(); }
  return *children;
}
const std::vector<Node> &Variant::getChildren() const
{
  if (children == nullptr) { children = std::make_unique<std::vector<Node>>(); }
  return *children;
}
Variant::Variant(const Type nodeType) : xmlNodeType(nodeType) {}
}// namespace XML_Lib