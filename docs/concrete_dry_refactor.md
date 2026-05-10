# Concrete DRY Refactor Plan for XML_Lib

## Overview
This refactor plan is a concrete, code-focused DRY improvement for the XML_Lib library. It is based on the current implementation in:

- `classes/source/implementation/common/XML_Parse.cpp`
- `classes/source/implementation/entity/XML_EntityMapper.cpp`
- `classes/source/implementation/xsd/XSD_Validator_Impl_Parse.cpp`
- `classes/source/implementation/xpath/XPath_Evaluator.cpp`
- `CMakeLists.txt`

The goal is to remove repeated code paths, centralize common behaviors, and improve both runtime efficiency and maintainability.

## Key Duplication Findings

### 1. Parser string assembly
- `XML_Parse.cpp` repeatedly builds `std::string` values in parse helpers:
  - `parseName`
  - `parseEntityReference`
  - `parseCharacterReference`
  - `parseCharacter`
  - `parseValueImpl`
  - `parseTagBody`
- The parser converts internal `String`/`Char` values to UTF-8 repeatedly.
- `parseValueImpl` duplicates logic for entity expansion and value concatenation across quoted strings.

### 2. Entity mapper lookup and translation
- `XML_EntityMapper.cpp` repeatedly allocates strings for entity names and file paths.
- Recursion detection uses `std::set<std::string>` and reparses entity strings inside `recurseOverEntityReference`.
- `getEntityMapping`, `isPresent`, `isInternal`, and `isExternal` each convert `std::string_view` to `std::string` or perform redundant lookups.
- `translate()` builds candidate tables on every call and performs repeated substring matching.

### 3. XSD node tag/attribute access
- `XSD_Validator_Impl_Parse.cpp` uses helper methods such as `localTag`, `attrValue`, `childElements`, and `resolveType` in tight loops.
- Many traversal loops call `localTag(child)` and `attrValue(child, "...")` repeatedly.
- `childElements()` enumerates and reserves twice in the same node traversal.

### 4. XPath node-value and result conversion
- `XPath_Evaluator.cpp` duplicates string-value computations:
  - `appendNodeStringValue`
  - `nodeStringValue`
- Node name extraction and local name extraction are repeated for every name test.
- Result conversion to string and number allocates intermediate strings for node-set and numeric cases.
- `collectDescendants` and `matchNodeTest` reimplement generic tree traversal or name matching patterns.

### 5. Build and include boundaries
- `CMakeLists.txt` exports many implementation include paths in the public interface.
- Public targets and internal sources are mixed, weakening encapsulation and build isolation.

## Concrete DRY Refactor Tasks

### Task 1: Extract parser helper utilities
Create a shared parser helper module:

- `classes/include/implementation/common/XML_ParseHelpers.hpp`
- `classes/source/implementation/common/XML_ParseHelpers.cpp`

Move repeated logic into helpers such as:

- `parseQuotedValue(ISource&, IEntityMapper*)`
- `readName(ISource&) -> std::string_view` or `String`
- `readReferenceText(ISource&, char terminator) -> std::string`
- `appendTextSegment(std::string&, const String&)`
- `parseCharacterOrReference(ISource&) -> XMLValue`

Refactor `parseValueImpl` so it reuses the helper for quoted value accumulation and entity expansion.
Refactor `parseEntityReference` and `parseCharacterReference` to share a `readUntil(ISource&, char delim)` helper.

### Task 2: Centralize entity mapping and eliminate repeated lookups
Create an entity mapper helper layer:

- `classes/include/implementation/entity/XML_EntityMapperHelpers.hpp`
- `classes/source/implementation/entity/XML_EntityMapperHelpers.cpp`

Refactor `XML_EntityMapper` to:

- use `std::unordered_map<std::string, XML_EntityMapping>` with reserve(16) and stable string keys.
- replace repeated `getEntityMapping(entityName)` calls with a single lookup when possible.
- add a helper `getCachedFileMapping(const std::string_view&)` that returns a reference to cached content.
- add `matchEntityPrefix(const std::string_view &text, size_t pos) -> std::optional<std::pair<std::string_view, const XML_EntityMapping*>>` for single-pass translation.

Rewrite `translate()` to avoid candidate rebuilding every call. If the type is fixed (`&`), maintain a precomputed sorted prefix table or use a trie-like lookup.

### Task 3: Make XSD traversal less repetitive
Introduce XSD traversal helpers:

- `classes/include/implementation/xsd/XSD_NodeHelpers.hpp`
- `classes/source/implementation/xsd/XSD_NodeHelpers.cpp`

Refactor common behavior:

- `XSD_Impl::localTag()` returns `std::string_view` but should avoid creating temporary `std::string` in callers.
- `attrValue()` should return stable `std::string_view` and callers should preserve that view rather than converting to `std::string` immediately.
- `childElements()` should become a reusable iterator-like helper or range adapter.

Replace repeated parser branches such as:

- the `sequence`/`choice`/`all` loops
- the inline type handling loops in `parseParticle` and `parseComplexType`

with helper functions:

- `parseChildParticleList(const Node&, XSD_ComplexType&)`
- `parseChildAttributes(const Node&, XSD_ComplexType&)`

### Task 4: Abstract XPath node name/value helpers and result conversion
Create XPath helper utilities:

- `classes/include/implementation/xpath/XPath_EvalHelpers.hpp`
- `classes/source/implementation/xpath/XPath_EvalHelpers.cpp`

Move duplicated logic into:

- `nodeStringValue()` and `appendNodeStringValue()` as a single reusable streaming helper
- `matchNodeName(const Node&, const std::string_view&)`
- `nodeLocalNameView(const Node&)`
- `resultToStringView(const XPathResult&, std::string&)`
- `stringToNumber(std::string_view)`

Refactor `matchNodeTest()` to use the new helpers and avoid repeated `find(':')` work.
Refactor node-set result conversions to reuse an external scratch buffer instead of allocating inside `resultToString` and `resultToNumber`.

### Task 5: Tighten public CMake include/export policy
Update `CMakeLists.txt` to enforce DRY and correct dependency scope:

- make only `classes/include` public
- keep `classes/include/interface`, `classes/include/implementation`, and converter/include directories private
- remove duplicate or redundant `target_include_directories` entries
- avoid exporting internal implementation headers accidentally
- keep `add_library(${XML_LIBRARY_NAME})` and source list separate from public header configuration

### Task 6: Add reusable code patterns for future maintenance
Introduce small reusable abstractions rather than copy/paste:

- `XML_Parse.hpp` and helper modules should provide `string_view` accessors and append helpers.
- `XSD_NodeHelpers` should provide `NodeRange` or `ElementRange` helpers for repeated child traversal.
- `XPath_EvalHelpers` should provide `ResultView` helpers for repeated conversion patterns.

## Implementation Example (Concrete)

### Parser helper extraction
Before:
```cpp
std::string unparsed;
unparsed.reserve(32);
parsed.reserve(32);
...
parsed += character.getParsed();
```
After:
```cpp
XMLValue parseQuotedValue(ISource &source, IEntityMapper *entityMapper)
{
  std::string unparsed;
  std::string parsed;
  unparsed.reserve(32);
  parsed.reserve(32);
  ...
}
```
This avoids duplicate quote parsing logic across `parseValue` overloads.

### XSD child traversal helper
Before:
```cpp
for (const auto &childRef : childElements(child)) {
  const auto &particleChild = particleChildRef.get();
  if (localTag(particleChild) == "element") {
    ...
  }
}
```
After:
```cpp
for (const Node &particleChild : elementChildren(child)) {
  if (nodeLocalTag(particleChild) == "element") {
    ...
  }
}
```

## Acceptance Criteria

- Source duplication is reduced in parser, entity mapper, XSD, and XPath modules.
- New helper modules are introduced for repeated behavior and are tested with existing unit tests.
- No public API changes are required; internal helpers remain implementation-only.
- `CMakeLists.txt` exports only the intended public headers and keeps implementation include paths private.
- Performance tests or size-report targets can compare before/after behavior.

## Recommended File Additions

- `docs/concrete_dry_refactor.md`
- `classes/include/implementation/common/XML_ParseHelpers.hpp`
- `classes/source/implementation/common/XML_ParseHelpers.cpp`
- `classes/include/implementation/entity/XML_EntityMapperHelpers.hpp`
- `classes/source/implementation/entity/XML_EntityMapperHelpers.cpp`
- `classes/include/implementation/xsd/XSD_NodeHelpers.hpp`
- `classes/source/implementation/xsd/XSD_NodeHelpers.cpp`
- `classes/include/implementation/xpath/XPath_EvalHelpers.hpp`
- `classes/source/implementation/xpath/XPath_EvalHelpers.cpp`

## Next Step
Implement the refactor in stages, starting from the parser helper extraction and the entity mapper lookup consolidation.
