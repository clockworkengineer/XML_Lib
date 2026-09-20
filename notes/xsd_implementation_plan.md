# XSD Implementation Plan for XML_Lib

## Overview

This document provides a concrete, phased plan for adding XML Schema Definition (XSD) validation to XML_Lib. The architecture mirrors the existing `DTD_Validator` / `DTD_Impl` pattern: a public `XSD_Validator` class that implements `IValidator`, backed by a `XSD_Impl` Pimpl, with its own schema data model and validation logic.

The plan is split into six sections (A–F) addressing the schema data model, schema parsing, the public API, validation against a parsed XML tree, tests, and documentation.

---

## Section A — Schema Data Model

**Goal**: Define the in-memory representation of an XSD document.

### New header: `classes/include/XSD_Validator.hpp`
Public-facing class, mirrors `DTD_Validator.hpp`:
```cpp
class XSD_Validator final : public IValidator {
public:
  explicit XSD_Validator(Node &xNode);
  void parse(ISource &source) override;
  void stringify(IDestination &destination) override;
  void validate(const Node &xNode) override;
private:
  std::unique_ptr<XSD_Impl> implementation;
};
```

### New header: `classes/include/implementation/XSD_Impl.hpp`
Internal implementation, mirrors `DTD_Impl.hpp`. Owns the schema data model (see below) and exposes `parse`, `stringify`, `validate`.

### New header: `classes/include/implementation/variants/XML_XSD.hpp`
Schema data model structs (analogous to the `DTD` struct in `XML_DTD.hpp`):

```
XSD_SimpleType
  name        : string
  baseType    : string            (e.g. "xs:string", "xs:integer")
  restrictions: XSD_Restriction[] (minLength, maxLength, pattern, enumeration, minInclusive, maxInclusive, etc.)

XSD_ComplexType
  name        : string
  sequence    : XSD_Particle[]    (ordered child elements)
  choice      : XSD_Particle[]    (alternative child elements)
  all         : XSD_Particle[]    (unordered child elements)
  attributes  : XSD_AttributeDecl[]
  mixed       : bool

XSD_Particle
  elementName : string
  minOccurs   : uint32_t  (default 1)
  maxOccurs   : uint32_t  (0 = unbounded, default 1)
  typeRef     : string    (resolved to XSD_ComplexType or XSD_SimpleType)

XSD_AttributeDecl
  name        : string
  typeRef     : string
  use         : enum { optional, required, prohibited }
  defaultValue: string
  fixedValue  : string

XSD_ElementDecl
  name        : string
  typeRef     : string
  minOccurs   : uint32_t
  maxOccurs   : uint32_t

XSD_Restriction
  facet       : enum { minLength, maxLength, pattern, enumeration,
                       minInclusive, maxInclusive, minExclusive,
                       maxExclusive, totalDigits, fractionDigits,
                       whiteSpace, length }
  value       : string
```

All structs live in `namespace XML_Lib` and are stored inside `XSD_Impl` as:
```cpp
std::unordered_map<std::string, XSD_SimpleType>   simpleTypes;
std::unordered_map<std::string, XSD_ComplexType>  complexTypes;
std::vector<XSD_ElementDecl>                      rootElements;
std::string                                       targetNamespace;
std::string                                       elementFormDefault;  // "unqualified" | "qualified"
```

---

## Section B — Schema Parsing

**Goal**: Parse an XSD document (itself well-formed XML) into the data model above.

### Strategy
An XSD document is valid XML, so `XSD_Impl::parse(ISource &)` proceeds in two passes:
1. Use XML_Lib itself to parse the XSD source into a `Node` tree (a nested `XML` instance).
2. Walk that `Node` tree to populate `simpleTypes`, `complexTypes`, and `rootElements`.

This avoids writing a second XML parser and keeps the schema-parsing code purely structural.

### New source files under `classes/source/implementation/xsd/`

| File | Responsibility |
|---|---|
| `XSD_Validator.cpp` | Thin forwarding to `XSD_Impl` (mirrors `DTD_Validator.cpp`) |
| `XSD_Validator_Impl.cpp` | `XSD_Impl` constructor/destructor |
| `XSD_Validator_Impl_Parse.cpp` | Entry point: parses XSD XML, dispatches per top-level element |
| `XSD_Validator_Impl_Parse_Types.cpp` | Parses `xs:simpleType`, `xs:complexType`, `xs:restriction`, facets |
| `XSD_Validator_Impl_Parse_Elements.cpp` | Parses top-level `xs:element` declarations, `xs:attribute` |
| `XSD_Validator_Impl_Stringify.cpp` | Reconstructs schema as XML text (for round-trip tests) |
| `XSD_Validator_Impl_Validate.cpp` | Validates a parsed XML `Node` tree against the loaded schema |

### Parsing rules to implement

- `xs:schema` root: read `targetNamespace`, `elementFormDefault`.
- `xs:element` at top level: record in `rootElements`.
- `xs:complexType` (named or inline): parse `xs:sequence`, `xs:choice`, `xs:all`, `xs:anyAttribute`, `xs:attribute`.
- `xs:simpleType` (named or inline): parse `xs:restriction` base + facets.
- `xs:include` / `xs:import`: load external XSD files via `XML::fromFile`, merge into same model (single-level only for Phase 1).
- Built-in types (`xs:string`, `xs:integer`, `xs:boolean`, `xs:decimal`, `xs:date`, `xs:dateTime`, `xs:anyURI`) recognised by name without explicit declaration.

### Error handling
Throw `IValidator::Error` for:
- Non-`xs:schema` root element.
- Unresolvable type references after full parse.
- Duplicate named type definitions.

---

## Section C — Public API

**Goal**: Expose XSD validation through the existing `XML` class with minimal API changes.

### `XML` class additions (`classes/include/XML.hpp` and `XML.cpp`)
The existing `validate()` path already calls `IValidator::validate()` through the DTD node. XSD validation needs its own entry point because the schema is an external file, not embedded in the XML document.

Add one overload:
```cpp
// Validate XML against an XSD schema loaded from a file path or string
void validate(const std::string_view &xsdSource) const;
```

Internally this:
1. Instantiates `XSD_Validator(root())`.
2. Creates a `BufferSource` from `xsdSource`.
3. Calls `xsdValidator.parse(source)`.
4. Calls `xsdValidator.validate(root())`.

### `XSD_Validator` public surface
```cpp
XSD_Validator(Node &xmlRoot);         // binds to the XML tree to validate
void parse(ISource &xsdSource);       // loads the schema
void stringify(IDestination &dest);   // reconstructs schema XML
void validate(const Node &xmlRoot);   // runs validation, throws IValidator::Error on failure
```

No changes to `IValidator`, `IParser`, `IStringify`, or any existing variant.

---

## Section D — Validation Logic

**Goal**: Walk the `Node` tree and enforce schema constraints.

### Implementation in `XSD_Validator_Impl_Validate.cpp`

Recursive function signature:
```cpp
void XSD_Impl::validateElement(const Node &xNode, const XSD_ComplexType &type);
```

#### Validation checks to implement (in order of priority)

**Phase 1 — Core (implement first)**

1. **Root element name** matches a top-level `xs:element` declaration.
2. **Element name resolution** — for each child element, find its `XSD_Particle` in the parent's `sequence` / `choice` / `all`.
3. **Unexpected elements** — child element not listed in the content model → error.
4. **Missing required elements** — `minOccurs > 0` elements absent → error.
5. **Occurrence counts** — child appears more times than `maxOccurs` → error.
6. **Required attributes** — `use="required"` attribute absent → error.
7. **Prohibited attributes** — `use="prohibited"` attribute present → error.
8. **Unknown attributes** — attribute not declared and `xs:anyAttribute` not present → error.
9. **Simple type validation** — validate text content / attribute values against `XSD_SimpleType` restrictions:
   - `xs:string`, `xs:integer`, `xs:boolean`, `xs:decimal` (built-in type checking via C++ parsing).
   - `minLength`, `maxLength`, `length` (string length).
   - `pattern` (std::regex match).
   - `enumeration` (value in allowed set).
   - `minInclusive`, `maxInclusive`, `minExclusive`, `maxExclusive` (numeric comparison).
10. **Fixed values** — attribute/element with `fixed` must equal the fixed value.
11. **Default values** — inject default attribute value into error message context (read-only; no tree mutation in Phase 1).

**Phase 2 — Extended (implement after Phase 1 passes)**

12. `xs:choice` content validation (exactly one branch must match).
13. `xs:all` content validation (each element 0 or 1 times, order irrelevant).
14. Mixed content (`mixed="true"`) — text nodes allowed alongside elements.
15. `xs:anyAttribute` — skip unknown attribute validation when present.
16. `xs:import` / `xs:include` namespace-aware merging.
17. Type inheritance (`xs:extension`, `xs:restriction` on complex types).

### Error message format
Match the existing style used by `DTD_Impl`:
```
XSD Validation Error [Element: <name>] <description>.
```
Throw as `IValidator::Error`.

---

## Section E — Testing

**Goal**: Achieve coverage comparable to the DTD test suite.

### New test files under `tests/source/xsd/`

| File | What It Tests |
|---|---|
| `XML_Lib_Tests_XSD_Parse.cpp` | Parse valid schemas; verify type/element maps populated correctly |
| `XML_Lib_Tests_XSD_Parse_Error.cpp` | Malformed schemas; unresolvable references; duplicate types |
| `XML_Lib_Tests_XSD_Validate_Elements.cpp` | Element presence, occurrence counts, unexpected elements |
| `XML_Lib_Tests_XSD_Validate_Attributes.cpp` | Required/prohibited/unknown attributes; fixed/default values |
| `XML_Lib_Tests_XSD_Validate_SimpleTypes.cpp` | Built-in type checking; all restriction facets |
| `XML_Lib_Tests_XSD_Stringify.cpp` | Round-trip: parse XSD → stringify → re-parse → same model |

### New XSD test data files under `tests/files/xsd/`
Create small, focused `.xsd` files for each test scenario:

```
tests/files/xsd/
  simple_string.xsd          xs:string element, no constraints
  simple_integer.xsd         xs:integer with minInclusive/maxInclusive
  simple_pattern.xsd         xs:restriction with pattern facet
  simple_enum.xsd            xs:restriction with enumeration facets
  complex_sequence.xsd       xs:complexType with xs:sequence
  complex_choice.xsd         xs:complexType with xs:choice
  complex_attributes.xsd     required, optional, prohibited attributes
  complex_mixed.xsd          mixed="true" complex type
  include_base.xsd           base schema for xs:include test
  include_child.xsd          includes include_base.xsd
  invalid_root.xsd           non-xs:schema root (parse error test)
```

### Helper function
Add to the existing test helper pattern:
```cpp
// Validate xmlSource against xsdPath; return empty string on success,
// exception message on failure.
std::string validateXSD(const std::string &xmlSource, const std::string &xsdPath);
```

### Minimum test counts to aim for
- XSD parsing: ≥ 20 assertions
- Element validation: ≥ 30 assertions
- Attribute validation: ≥ 20 assertions
- Simple type validation: ≥ 30 assertions
- Stringify round-trip: ≥ 10 assertions

---

## Section F — Build System Integration

**Goal**: Add new source files and test files to CMake without breaking existing builds.

### `CMakeLists.txt` changes

1. Add glob or explicit list for `classes/source/implementation/xsd/*.cpp` to the `XML_Lib` library target.
2. Add `classes/include/XSD_Validator.hpp` to the public install headers.
3. Add `tests/source/xsd/*.cpp` to the `XML_Lib_Unit_Tests` target.
4. Copy `tests/files/xsd/` to the test working directory via `configure_file` or `file(COPY ...)`.

No new CMake targets are required — XSD is part of the same library and test executable.

---

## Section G — Documentation Updates

After all tests pass, update:

1. **`docs/XML_Lib_Standards_Report.md`**:
   - Change `| Schema (XSD) | No | Not implemented |` → `| Schema (XSD) | Yes | W3C XSD 1.0 — element/attribute validation, simple types, occurrence constraints |`
   - Update Limitations section accordingly.

2. **`docs/API.md`**: Add an "XSD Validation" section documenting `XML::validate(xsdSource)`, `XSD_Validator`, and error format.

3. **`docs/Guide.md`**: Add an "XML Schema Validation" section with two worked examples (inline schema string, schema from file).

4. **`README.md`**: Add XSD to the Features list and Standard Compliance section.

---

## Implementation Order

Work through sections in this order to allow incremental builds and test-driven progress:

| Step | Section | Deliverable | Verification |
|------|---------|-------------|--------------|
| 1 | A | Data model headers | Compiles cleanly with no tests yet |
| 2 | B (pass 1) | `parse()` for `xs:schema`, `xs:element`, `xs:complexType` sequence | `XSD_Parse` tests for basic schemas pass |
| 3 | D (Phase 1, items 1–5) | Element name + occurrence validation | `XSD_Validate_Elements` tests pass |
| 4 | D (Phase 1, items 6–8) | Attribute presence validation | `XSD_Validate_Attributes` tests pass |
| 5 | B (pass 2) | `parse()` for `xs:simpleType` + restriction facets | `XSD_Parse` tests extended |
| 6 | D (Phase 1, items 9–11) | Simple type + fixed value validation | `XSD_Validate_SimpleTypes` tests pass |
| 7 | C | `XML::validate(xsdSource)` overload | Integration test via `XML::validate()` |
| 8 | B (stringify) | `stringify()` round-trip | `XSD_Stringify` tests pass |
| 9 | D (Phase 2) | choice, all, mixed, anyAttribute, extension | Extended tests pass |
| 10 | F | CMake wiring | Clean build, all existing + new tests pass |
| 11 | G | Documentation | Standards Report, API.md, Guide.md, README updated |

Run the full test suite (`XML_Lib_Unit_Tests.exe --reporter compact` from `tests/files`) after each step to catch regressions.

---

## Key Constraints

- Stay within `namespace XML_Lib` throughout.
- Do not modify `IValidator`, `IParser`, `IStringify`, `IAction`, or any existing variant struct.
- `XSD_Impl` uses the Pimpl pattern exactly as `DTD_Impl` does — no implementation details leak into `XSD_Validator.hpp`.
- The XSD parser uses the existing XML parser (a nested `XML` instance) to read the schema file — no hand-rolled XML parsing.
- All error reporting uses `IValidator::Error` with the message format shown in Section D.
- No external libraries (e.g. libxml2, xerces) — pure C++20.
