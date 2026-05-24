# XML_Lib Compliance Roadmap

## Goal
Produce a concrete implementation roadmap for making XML_Lib fully compliant with XML 1.0 and its standard validation ecosystems (DTD, XPath 1.0, and XSD 1.0). This document is written directly into the repository so it can be tracked and expanded.

## Current status
Based on the repository sources and `docs/XML_Lib_Standards_Report.md`:
- XML 1.0 parsing + well-formedness: strong
- Namespaces: supported
- DTD validation: supported
- XPath 1.0: largely implemented
- XSD validation: Phase 2 support implemented
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
This is the largest body of work and the main remaining compliance gap. A concrete implementation plan is available in `docs/XML_Phase2_Plan.md`.

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
Turn implementation into measurable compliance. See `docs/XML_Phase3_Plan.md` for the concrete execution plan.

1. Add W3C-derived test coverage
   - XML 1.0 conformance and syntax tests
   - DTD conformance tests
   - XPath 1.0 evaluation tests
   - XSD 1.0 feature-subset conformance tests

2. Add regression tests for real edge cases
   - mixed content and `ELEMENT` content models
   - entity reference vs character reference normalization
   - namespace scoping and prefix collision handling
   - `xs:any` / `xs:anyAttribute`
   - schema composition and identity constraints

3. Add documentation and compliance tracking
   - extend `docs/XML_Lib_Standards_Report.md` with a status matrix
   - document exact supported XSD/XPath/XSL subsets and remaining gaps
   - publish the Phase 3 compliance command and expected results

4. Automate compliance validation
   - add CI coverage for the compliance test suite
   - keep the Phase 3 test command and fixtures up to date

## What was verified
- Added `tests/source/compliance/XML_Lib_Tests_Compliance.cpp` and W3C-derived fixtures in `tests/files/w3c/`.
- Verified the compliance harness runs successfully with:

```bash
./build/tests/XML_Lib_Unit_Tests -c "[Compliance]"
```

- Confirmed DTD external subset resolution and XSD validation scenarios are exercising the intended supported features.
- Documented supported XML, DTD, XSD, and XPath subsets in the standards report and guide.

## Current compliance profile
- XML 1.0 syntax and namespaces: supported and tested
- DTD validation: supported and tested for internal/external DTDs and content models
- XPath 1.0: supported and tested for axes, predicates, and functions
- XSD 1.0: supported for the implemented Phase 2 subset; schema composition and identity constraints are covered

## Known unsupported cases
- XPath 2.0/3.x features and `document()` function
- XSD advanced constructs such as `xs:redefine`, substitution groups, and conditional type alternatives
- Full XML Schema 1.1 feature set

## How to run compliance verification
Run the Catch2 compliance tag on the built test binary:

```bash
cd build
../build/tests/XML_Lib_Unit_Tests -c "[Compliance]"
```

Alternately, use the automation script directly from the repository root:

```bash
chmod +x ./scripts/Linux-Run-Compliance.sh
./scripts/Linux-Run-Compliance.sh
```

If the test binary is not yet built, generate the build tree and compile it first:

```bash
cmake -S . -B build
cmake --build build
```

### CI coverage
The Jenkins pipeline includes a dedicated `Compliance` stage that runs the same command on every merge. This ensures the W3C-derived compliance harness is validated automatically alongside normal build and test stages.

### Phase 4 — Optional advanced XML compliance
This is for beyond “true enough for most XML use cases.”

1. XML 1.1 support
   - character range changes
   - named entity and control character semantics
   - XML 1.1 line normalization behavior

2. XPath beyond 1.0
   - optional support for XPath 2.0 features if needed by consumers

See `docs/XML_Phase4_Plan.md` for a concrete Phase 4 roadmap with milestones, tasks, and acceptance criteria.

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
