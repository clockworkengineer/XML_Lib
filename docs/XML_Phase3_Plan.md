# XML_Lib Phase 3 Compliance Plan

## Goal

Turn XML_Lib's implementation into measurable compliance by adding a comprehensive test harness, W3C-derived regression coverage, and public compliance tracking.

## Scope

Phase 3 focuses on validation output and test validation rather than new parser features. The goal is to verify the library against recognized XML, DTD, XPath, and XSD conformance criteria and document exactly what is supported.

## Success criteria

Phase 3 is complete when:
- a representative W3C-derived compliance test suite is integrated and automated
- regression tests cover both standard cases and real-world edge cases
- documentation clearly describes supported and unimplemented XML/XSD/XPath features
- compliance status is reflected in the standards report and README

## Milestones

### Milestone 1 — Compliance test infrastructure

#### Tasks
- add fixture directories for W3C-derived test cases:
  - `tests/files/w3c/xml/`
  - `tests/files/w3c/dtd/`
  - `tests/files/w3c/xpath/`
  - `tests/files/w3c/xsd/`
- add CMake support for reading fixture files in tests
- add a new test runner module for compliance tests:
  - `tests/source/compliance/XML_Lib_Tests_Compliance.cpp`
- create helper utilities to load fixtures and compare results consistently

#### Deliverables
- test harness that can execute XML/DTD/XPath/XSD compliance fixtures
- documented fixture naming and expected-result conventions

### Milestone 2 — W3C-derived compliance coverage

#### Tasks
- import and categorize relevant test cases for:
  - XML 1.0 syntax and encoding
  - DTD declaration and validation semantics
  - XPath 1.0 axis and function support
  - XSD 1.0 feature subset already implemented
- create targeted Catch2 regression tests for:
  - namespace scoping and prefix collision handling
  - entity reference vs character reference normalization
  - mixed content and `ELEMENT` content models
  - `xs:any` / `xs:anyAttribute`
  - schema composition (`xs:include`, `xs:import`)
  - `xs:key`, `xs:keyref`, `xs:unique`
  - extension/restriction inheritance in complex/simple content
- mark tests as pass/fail with clear expectation comments

#### Deliverables
- upgraded regression test suite with W3C-derived XML/XSD/XPath examples
- a repeatable command for compliance verification

### Milestone 3 — Documentation and reporting

#### Tasks
- update `docs/XML_Lib_Standards_Report.md` with:
  - status matrix for XML, DTD, XPath, XSD support
  - supported subset vs remaining gaps
- update `docs/Guide.md` to describe exactly supported XSD/XPath features and limitations
- update `docs/API.md` with supported behavior and error conventions
- update `README.md` with Phase 3 compliance summary and test command
- add a `docs/Compliance.md` or expand `docs/XML_Compliance_Roadmap.md` with:
  - what was verified
  - how to run compliance tests
  - known unsupported cases

#### Deliverables
- published compliance status and usage guidance for library consumers
- traceable roadmap from implementation to validation

### Milestone 4 — Continuous validation and release readiness

#### Tasks
- add a CI job or script to run compliance tests on every merge
- make sure the build uses the same CMake options as release expectations
- add `XML_Size_Report` or equivalent target if relevant for release validation
- finalize release notes that call out Phase 3 compliance coverage

#### Deliverables
- CI validation for standards compliance
- release-ready documentation and test reports

## File-level targets

- `docs/XML_Phase3_Plan.md`
- `docs/XML_Lib_Standards_Report.md`
- `docs/Guide.md`
- `docs/API.md`
- `docs/XML_Compliance_Roadmap.md`
- `tests/source/compliance/XML_Lib_Tests_Compliance.cpp`
- `tests/files/w3c/xml/`
- `tests/files/w3c/dtd/`
- `tests/files/w3c/xpath/`
- `tests/files/w3c/xsd/`
- `tests/source/xsd/XML_Lib_Tests_XSD.cpp`
- `tests/source/xml/XML_Lib_Tests_XML.cpp`
- `tests/source/xpath/XML_Lib_Tests_XPath.cpp`

## Phase 3 priorities

1. Build the infrastructure to run a compliance test suite reliably.
2. Validate the implementation against real XML/XSD/XPath behaviors.
3. Document the exact supported profile and remaining gaps.
4. Automate the tests in CI so Phase 3 compliance remains enforced.
