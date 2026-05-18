#pragma once

// Public convenience header for node type checking and casting.
//
// This header re-exports the core XML node helpers used by client code:
//   - isA<T>(node)
//   - NRef<T>(node)
//
// It intentionally includes only public-facing headers so users do not need
// to depend on internal implementation paths.

#include "XML_NodeRef.hpp"
