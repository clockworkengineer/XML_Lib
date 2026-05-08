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
  children.push_back(std::move(child));
}
void Variant::addChild(Node &&child) const
{
  children.push_back(std::move(child));
}
// Get Node children reference
std::pmr::vector<Node> &Variant::getChildren()
{
  return children;
}
const std::pmr::vector<Node> &Variant::getChildren() const
{
  return children;
}
void Variant::reserveChildren(const size_t count)
{
  children.reserve(count);
}
Variant::Variant(const Type nodeType, std::pmr::memory_resource *resource)
  : xmlNodeType(nodeType), children(resource)
{}
}// namespace XML_Lib