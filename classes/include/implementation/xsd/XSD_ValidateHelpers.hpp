#pragma once

#include "XML_Core.hpp"
#include "XSD_Impl.hpp"

#include <string>
#include <string_view>
#include <unordered_map>

namespace XML_Lib {

/// Throw an XSD validation error for the given element and message.
[[noreturn]] void xsdError(const std::string &elementName, const std::string &message);

/// Collect element-child occurrence counts from xNode's children.
[[nodiscard]] std::unordered_map<std::string, uint32_t> collectElementChildCounts(const Node &xNode);

/// Find a declared particle in a complex type by element name; returns nullptr if not found.
[[nodiscard]] const XSD_Particle *findDeclaredParticle(const XSD_ComplexType &type, std::string_view childName);

/// Throw if any child element name in childCounts is not declared in type.particles.
void validateUnexpectedChildren(const std::unordered_map<std::string, uint32_t> &childCounts,
  const XSD_ComplexType &type,
  const std::string &elemName);

/// Throw if any declared particle has an occurrence count outside [minOccurs, maxOccurs].
void validateParticleOccurrenceBounds(const std::unordered_map<std::string, uint32_t> &childCounts,
  const XSD_ComplexType &type,
  const std::string &elemName);

}// namespace XML_Lib
