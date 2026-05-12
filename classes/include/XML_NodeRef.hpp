#pragma once

// ===========================================================================
// Public convenience header: node cast helpers
//
// Exposes isA<T>(node) and NRef<T>(node) — the two primary tools for
// testing and casting a Node to a concrete variant type (Element, Root,
// Self, Content, Comment, CDATA, PI, EntityReference, Prolog, Declaration,
// DTD) — together with all variant type definitions.
//
// Usage:
//   #include "XML_Node.hpp"
//   auto &elem = NRef<Element>(xml.root());
//   if (isA<Element>(node)) { ... }
//
// The low-level ISource / IDestination overloads and concrete source/
// destination types are in XML_Sources.hpp / XML_Destinations.hpp.
// ===========================================================================

#include "XML.hpp"
// XML_Core.hpp is reachable via the implementation/ include path added by
// CMake and brings in all variant types plus the isA<T>/NRef<T> helpers.
#include "XML_Core.hpp"
