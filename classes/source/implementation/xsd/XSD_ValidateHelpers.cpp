//
// Module: XSD_ValidateHelpers
//
// Description: Shared validation utilities extracted from XSD_Validator_Impl_Validate.
//
// Dependencies: C++20 - Language standard features used.
//

#include "xsd/XSD_ValidateHelpers.hpp"
#include "XML_NodeKindHelpers.hpp"

namespace XML_Lib {

[[noreturn]] void xsdError(const std::string &elementName, const std::string &message)
{
  XML_LIB_THROW(IValidator::Error("XSD Validation Error [Element: " + elementName + "] " + message));
}

std::unordered_map<std::string, uint32_t> collectElementChildCounts(const Node &xNode)
{
  std::unordered_map<std::string, uint32_t> counts;
  for (const auto &child : xNode.getChildren()) {
    if (isElementLikeNode(child)) { ++counts[NRef<Element>(child).name()]; }
  }
  return counts;
}

const XSD_Particle *findDeclaredParticle(const XSD_ComplexType &type, const std::string_view childName)
{
  for (const auto &p : type.particles) {
    if (p.elementName == childName) { return &p; }
  }
  for (const auto &p : type.particles) {
    if (p.elementName == "*") { return &p; }
  }
  return nullptr;
}

void validateUnexpectedChildren(const std::unordered_map<std::string, uint32_t> &childCounts,
  const XSD_ComplexType &type,
  const std::string &elemName)
{
  const bool hasWildcard = std::ranges::any_of(type.particles,
    [](const XSD_Particle &p) { return p.elementName == "*"; });
  if (hasWildcard) { return; }

  for (const auto &entry : childCounts) {
    const auto &childName = entry.first;
    const bool declared =
      std::ranges::any_of(type.particles, [&](const XSD_Particle &p) { return p.elementName == childName; });
    if (!declared) { xsdError(elemName, "unexpected child element <" + childName + ">."); }
  }
}

static uint32_t countWildcardChildren(const Node &xNode, const XSD_ComplexType &type)
{
  uint32_t count = 0;
  for (const auto &child : xNode.getChildren()) {
    if (!isElementLikeNode(child)) { continue; }
    const auto &childName = NRef<Element>(child).name();
    const bool explicitMatch = std::ranges::any_of(type.particles,
      [&](const XSD_Particle &p) { return p.elementName != "*" && p.elementName == childName; });
    if (!explicitMatch) { ++count; }
  }
  return count;
}

void validateParticleOccurrenceBounds(const Node &xNode,
  const std::unordered_map<std::string, uint32_t> &childCounts,
  const XSD_ComplexType &type,
  const std::string &elemName)
{
  for (const auto &particle : type.particles) {
    uint32_t count = 0;
    if (particle.elementName == "*") {
      count = countWildcardChildren(xNode, type);
    } else {
      count = childCounts.contains(particle.elementName) ? childCounts.at(particle.elementName) : 0u;
    }

    if (count < particle.minOccurs) {
      xsdError(elemName,
        "required child element <" + particle.elementName + "> is missing (expected at least "
          + std::to_string(particle.minOccurs) + ", found " + std::to_string(count) + ").");
    }
    if (particle.maxOccurs != 0 && count > particle.maxOccurs) {
      xsdError(elemName,
        "child element <" + particle.elementName + "> appears " + std::to_string(count)
          + " times but maxOccurs=" + std::to_string(particle.maxOccurs) + ".");
    }
  }
}

}// namespace XML_Lib
