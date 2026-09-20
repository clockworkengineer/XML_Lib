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
// XML_Node.hpp is a public alias for this header.
//
// The low-level ISource / IDestination overloads and concrete source/
// destination types are in XML_Sources.hpp / XML_Destinations.hpp.
// ===========================================================================

#include "XML.hpp"
#include "data/XML_Value.hpp"
#include "data/XML_Attribute.hpp"
#include "data/XML_ExternalReference.hpp"
#include "nodes/XML_Variant.hpp"
#include "node/XML_Node.hpp"
#include "nodes/XML_Element.hpp"
#include "nodes/XML_Root.hpp"
#include "nodes/XML_Self.hpp"
#include "nodes/XML_Content.hpp"
#include "nodes/XML_Comment.hpp"
#include "nodes/XML_CDATA.hpp"
#include "nodes/XML_PI.hpp"
#include "nodes/XML_Prolog.hpp"
#include "nodes/XML_Declaration.hpp"
#include "nodes/XML_DTD.hpp"
#include "nodes/XML_EntityReference.hpp"
#include "node/XML_Node_Reference.hpp"
#include "node/XML_Node_Contents.hpp"
#include "node/XML_Node_Index.hpp"
#include "node/XML_Node_Creation.hpp"
