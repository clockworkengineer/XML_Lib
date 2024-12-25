//
// Class: XML_EntityMapper
//
// Description: XML entity reference mapper.
//
// Dependencies: C++20 - Language standard features used.
//

#include "XML_Impl.hpp"

namespace XML_Lib {
void Variant::addChild(XNode &child) const
{
  if (children == nullptr) { children = std::make_unique<std::vector<XNode>>(); }
  children->push_back(std::move(child));
}
void Variant::addChild(XNode &&child) const
{
  if (children == nullptr) { children = std::make_unique<std::vector<XNode>>(); }
  children->push_back(std::move(child));
}
// Get XNode children reference
std::vector<XNode> &Variant::getChildren()
{
  if (children == nullptr) { children = std::make_unique<std::vector<XNode>>(); }
  return *children;
}
const std::vector<XNode> &Variant::getChildren() const
{
  if (children == nullptr) { children = std::make_unique<std::vector<XNode>>(); }
  return *children;
}
Variant::Variant(const Type nodeType) : xmlNodeType(nodeType) {}
}// namespace XML_Lib