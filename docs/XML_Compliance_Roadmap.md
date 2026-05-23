# XML_Lib Compliance Roadmap

## Goal
Produce a concrete implementation roadmap for making XML_Lib fully compliant with XML 1.0 and its standard validation ecosystems (DTD, XPath 1.0, and XSD 1.0). This document is written directly into the repository so it can be tracked and expanded.

## Current status
Based on the repository sources and `docs/XML_Lib_Standards_Report.md`:
- XML 1.0 parsing + well-formedness: strong
- Namespaces: supported
- DTD validation: supported
- XPath 1.0: largely implemented
- XSD validation: partial/Phase 1 support
- Comment and documentation style: now converted to Doxygen

## Concrete roadmap

### Phase 1 — Verify and close gaps in core XML behavior
These are the smallest, highest-confidence wins.

A concrete, actionable Phase 1 implementation plan has been created in `docs/XML_Phase1_Plan.md`.

1. Confirm XML 1.0 character and syntax conformance
   - Validate support for allowed XML character ranges and denial of disallowed characters
   - Validate behavior for BOM-handling, XML declarations, and whitespace normalization
   - Confirm support for comment, CDATA, PI, entity reference, and namespace syntax edge cases

2. Improve parser coverage for external subsets and conditional sections
   - External subset parsing in `<!DOCTYPE ... SYSTEM/PUBLIC ...>`
   - Conditional sections `<![ IGNORE [ ... ]]>` and `<![ INCLUDE [ ... ]]>`
   - Parameter entity expansion behavior within DTDs

3. Harden encoding support
   - Confirm UTF-8 and UTF-16 BOM handling in all parser entrypoints
   - Validate rejection behavior for unsupported encodings with consistent errors

### Phase 2 — Fully complete XSD 1.0 support
This is the largest body of work and the main remaining compliance gap.

1. Implement XSD type derivation and reuse
   - `xs:extension` and `xs:restriction` within `complexContent`
   - `xs:extension` and `xs:restriction` within `simpleContent`
   - Derivation by restriction for simple types with facets

2. Implement schema composition
   - `xs:include`
   - `xs:import`
   - `xs:redefine` (optional, later)

3. Implement identity and uniqueness constraints
   - `xs:key`
   - `xs:keyref`
   - `xs:unique`
   - report schema validation errors on missing or duplicate key values

4. Implement additional schema constructs
   - `xs:notation`
   - `xs:any` and `xs:anyAttribute`
   - substitution groups
   - `xs:choice` / `xs:sequence` / `xs:all` semantics in more complex cases
   - mixed content schema declarations
   - qualified types and namespace-aware schema resolution

5. Complete schema validation engine
   - support for `nillable` on elements
   - default/fixed value processing in schema validation
   - `minOccurs` / `maxOccurs` with nested content models and `unbounded`
   - `block` and `final` semantics (optional advanced compliance)

### Phase 3 — Output and test validation
Turn implementation into measurable compliance.

1. Add W3C test coverage
   - XML 1.0 conformance tests
   - DTD conformance tests
   - XPath 1.0 conformance tests
   - XSD 1.0 conformance tests for the implemented feature set

2. Add regression tests for edge cases
   - mixed content and `ELEMENT` models
   - entity reference vs character reference decoding
   - namespace scoping and prefix collision detection

3. Add documentation and compliance tracking
   - extend `docs/XML_Lib_Standards_Report.md` with a status checklist
   - document the exact supported XSD subset and remaining unimplemented features

### Phase 4 — Optional advanced XML compliance
This is for beyond “true enough for most XML use cases.”

1. XML 1.1 support
   - character range changes
   - named entity and control character semantics
   - XML 1.1 line normalization behavior

2. XPath beyond 1.0
   - optional support for XPath 2.0 features if needed by consumers

3. Schema Profile and performance tuning
   - document supported XSD profile
   - optimize large-schema and large-document validation

## File-level targets
The following files are the primary technical areas for this roadmap:

- `classes/source/implementation/xml/parser/Default_Parser.cpp`
- `classes/source/implementation/xml/XML_Impl.cpp`
- `classes/source/implementation/dtd/DTD_Validator_Impl_Validator.cpp`
- `classes/source/implementation/dtd/DTD_Validator_Impl_Parse.cpp`
- `classes/source/implementation/xsd/XSD_Validator_Impl.cpp`
- `classes/source/implementation/xsd/XSD_Validator_Impl_Parse.cpp`
- `classes/source/implementation/xsd/XSD_NodeHelpers.cpp`
- `classes/source/implementation/xpath/XPath_Evaluator.cpp`
- `classes/source/implementation/xpath/XPath_Parser.cpp`
- `classes/source/implementation/xpath/XPath_Lexer.cpp`

## Recommended next step
Start with a concrete 2-week sprint focused on Phase 2, with the first milestone:
1. implement `xs:extension`/`xs:restriction` support
2. add schema include/import processing
3. add tests for derived complex/simple types

That will turn the existing partial XSD support into a real standard-compliance effort.
