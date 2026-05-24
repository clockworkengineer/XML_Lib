# XML_Lib Phase 4 Compliance Plan

## Goal

Extend XML_Lib beyond Phase 3 measurable compliance into advanced XML standards coverage and release readiness. Phase 4 focuses on optional XML/XPath/XSD features, performance validation, and final compliance reporting.

## Scope

Phase 4 covers:
- advanced XML compliance beyond XML 1.0 core and DTD/XSD/XPath Phase 3 coverage
- optional XML 1.1 compatibility and parser hardening
- broad XPath support beyond the current XPath 1.0 profile
- advanced XSD 1.0 schema constructs and optional XML Schema 1.1 gateway features
- release validation, performance, and CI enforcement

## Success criteria

Phase 4 is complete when:
- advanced compliance test cases are added and pass for XML, XPath, and XSD scenarios
- unsupported advanced features are documented clearly and intentionally
- the library includes a release-ready compliance matrix and automated CI gating
- performance and build readiness checks are in place for release expectations

## Work breakdown

### 1. Advanced XML parser compliance

#### Tasks
- Verify and implement XML 1.1 character and line normalization rules where feasible.
- Validate handling of XML 1.1 name and character ranges, including valid control characters and normalized line endings.
- Harden external entity parsing and DTD processing for edge cases around system/public identifiers and conditional sections.
- Add regression tests for XML 1.1-style documents and backwards-compatible XML 1.0 edge cases.

#### Target files
- `classes/source/implementation/xml/parser/Default_Parser.cpp`
- `classes/source/implementation/xml/parser/Default_Parser.hpp`
- `classes/source/implementation/xml/file/XML_File.cpp`
- `classes/source/implementation/entity/XML_EntityMapper.cpp`

#### Deliverables
- XML 1.1 compatibility tests or explicit unsupported-feature documentation
- parser tests for new line normalization and higher Unicode ranges
- improved XML robustness in production and test coverage

### 2. Advanced XPath support and reliability

#### Tasks
- Extend XPath support with the `document()` function and more complete namespace axis semantics.
- Add support for additional XPath 1.0 functions and operator edge cases if missing.
- Establish an XPath 2.0/3.x roadmap entry for future work, even if only a compatibility subset is implemented.
- Add regression fixtures for axis-heavy and function-heavy queries using W3C-derived real-world examples.

#### Target files
- `classes/source/implementation/xpath/XPath.cpp`
- `classes/source/implementation/xpath/XPath.hpp`
- `classes/source/implementation/xpath/XPath_Evaluator.cpp`
- `classes/source/implementation/xpath/XPath_Parser.cpp`
- `docs/xpath_implementation_plan.md`

#### Deliverables
- new XPath compliance tests for `document()`, `namespace` axis, and function semantics
- documented XPath coverage matrix in `docs/XML_Lib_Standards_Report.md`
- a concrete compatibility list for XPath beyond the current Phase 3 subset

### 3. Advanced XSD and schema coverage

#### Tasks
- Implement additional XSD 1.0 features:
  - `xs:notation`
  - `xs:substitutionGroup`
  - `xs:nillable`
  - `block` / `final`
  - qualified default `form`/`elementFormDefault`/`attributeFormDefault`
  - simple type `xs:list` and `xs:union` if feasible
- Add robust handling for `xs:redefine` and schema component redefinitions, or document it as unsupported if deferred.
- Add advanced negative tests for prohibited blocks, substitution groups, and `nillable` value handling.

#### Target files
- `classes/source/implementation/xsd/XSD_Validator_Impl_Parse.cpp`
- `classes/source/implementation/xsd/XSD_Validator_Impl_Validate.cpp`
- `classes/source/implementation/xsd/XSD_Validator_Impl.cpp`
- `docs/xsd_implementation_plan.md`

#### Deliverables
- advanced XSD compliance fixtures under `tests/files/w3c/xsd/`
- targeted tests for substitution groups, nillable elements, and block/final constraints
- updated standards report with advanced XSD support and explicit gaps

### 4. Compliance automation and release readiness

#### Tasks
- Add CI test cases and gating for advanced compliance features.
- Create release validation rules for supported build configurations and expected test targets.
- Add performance validation scripts or benchmarks for large-document parsing and schema validation.
- Update release documentation with the Phase 4 compliance matrix and supported profile.

#### Target files
- `Jenkinsfile`
- `scripts/Linux-Run-Compliance.sh`
- `README.md`
- `docs/XML_Lib_Standards_Report.md`
- `docs/XML_Compliance_Roadmap.md`

#### Deliverables
- CI pipeline with advanced compliance verification stage
- release readiness checklist for test, docs, and performance validation
- documented feature status and supported profile for consumers

### 5. Documentation and consumer guidance

#### Tasks
- Publish Phase 4 plan and feature matrix within the docs set.
- Document unsupported advanced features clearly so release claims remain accurate.
- Add examples for any newly supported advanced XSD/XPath features.

#### Deliverables
- `docs/XML_Phase4_Plan.md`
- clear release notes and standards-report sections describing Phase 4 coverage
- updated user guide examples for advanced compliance features

## Milestones

### Milestone 1 — Advanced parser and XML feature readiness
- add XML 1.1 compatibility tests and parser hardening
- document any unsupported XML 1.1 behavior

### Milestone 2 — XPath and XSD advanced feature coverage
- add advanced XPath tests and feature support
- implement advanced XSD features and add schema tests

### Milestone 3 — CI enforcement and release readiness
- add advanced compliance gating to CI
- publish feature matrix and release validation guidance

## Acceptance checklist

- [ ] Advanced XML compliance tests added
- [ ] `document()` and improved XPath axis coverage validated
- [ ] Advanced XSD constructs tested and documented
- [ ] CI pipeline enforces advanced compliance stage
- [ ] Release documentation reflects Phase 4 support and limitations

## Notes

Phase 4 should build on the stable Phase 3 baseline, with priority on measurable test coverage, predictable behavior, and explicit documentation for optional or unsupported advanced features.