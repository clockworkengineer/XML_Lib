#pragma once

#include "XML_Core.hpp"
#include "interface/ISource.hpp"

namespace XML_Lib {

/// @brief Dedicated validator for XML Namespaces (W3C Namespaces in XML 1.0).
///
/// Isolates element and attribute namespace validation rules from the parsing logic,
/// enforcing the Single Responsibility Principle (SRP).
class NamespaceValidator
{
public:
  static void validate(const Element &element, const ISource &source, bool namespacesEnabled, bool strictNamespaces);
};

} // namespace XML_Lib
