# XML_Lib Phase 2 Compliance Plan

## Goal

Complete Phase 2 by implementing full XML Schema Definition (XSD) 1.0 validation support in XML_Lib, including schema parsing, schema composition, type derivation, constraint validation, and integration tests.

## Scope

Phase 2 covers:
- XSD 1.0 schema parsing and in-memory schema model
- XSD validation of XML documents against schemas
- support for common complex/simple type derivation and restriction
- schema composition via `xs:include` / `xs:import`
- validation of identity constraints and structural content models
- public API and documentation updates

## Success criteria

Phase 2 is complete when:
- XML_Lib can parse and validate XML documents using XSD 1.0 schemas
- schema includes/imports can be resolved and merged into the validation model
- complex types, simple type facets, and attribute constraints are enforced
- identity constraints (`key`, `keyref`, `unique`) are validated
- public API and docs reflect XSD support
- regression tests cover all implemented Phase 2 cases

## Work breakdown

### 1. Define schema data model and public XSD API

#### Tasks
- Add `XSD_Validator.hpp` and `XSD_Impl.hpp`
- Define schema model types:
  - `XSD_SimpleType`
  - `XSD_ComplexType`
  - `XSD_Particle`
  - `XSD_AttributeDecl`
  - `XSD_ElementDecl`
  - `XSD_Restriction`
- Add public `XSD_Validator` methods:
  - `parse(ISource &source)`
  - `validate(const Node &xNode)`
  - `stringify(IDestination &destination)`

#### Target files
- `classes/include/XSD_Validator.hpp`
- `classes/include/implementation/XSD_Impl.hpp`
- `classes/source/implementation/xsd/XSD_Validator.cpp`
- `classes/source/implementation/xsd/XSD_Validator_Impl.cpp`

#### Deliverables
- clean compile of new header/API
- schema model defined and documented

### 2. Parse core XSD schema constructs

#### Tasks
- Parse `xs:schema`, `xs:element`, `xs:complexType`, `xs:simpleType`
- Parse content model particles:
  - `xs:sequence`
  - `xs:choice`
  - `xs:all`
- Parse attribute declarations and simple type restrictions
- Support built-in XSD datatypes used in Phase 2 validation

#### Target files
- `classes/source/implementation/xsd/XSD_Validator_Impl_Parse.cpp`
- `classes/source/implementation/xsd/XSD_Validator_Impl_Parse_Types.cpp`
- `classes/source/implementation/xsd/XSD_Validator_Impl_Parse_Elements.cpp`

#### Deliverables
- parser tests for valid XSD components
- error tests for invalid schemas and duplicate declarations

### 3. Implement schema composition

#### Tasks
- Add support for `xs:include` and `xs:import`
- Resolve included/imported schemas during parse
- Merge included schema components into the current model
- Detect circular includes/imports and report errors

#### Target files
- `classes/source/implementation/xsd/XSD_Validator_Impl_Parse.cpp`
- `classes/source/implementation/xsd/XSD_Validator_Impl_Parse_Elements.cpp`

#### Deliverables
- tests for cross-file schema composition
- ability to validate XML against composed schemas

### 4. Validate XML against the schema

#### Tasks
- Implement element content validation for `sequence`, `choice`, `all`
- Enforce `minOccurs` / `maxOccurs` / `unbounded`
- Enforce required/prohibited/optional attributes
- Validate simple type values against facets:
  - `minLength`, `maxLength`, `length`
  - `pattern`
  - `enumeration`
  - `minInclusive`, `maxInclusive`, `minExclusive`, `maxExclusive`
  - `totalDigits`, `fractionDigits`
  - `whiteSpace`
- Support `default` and `fixed` values in validation reporting
- Validate `xs:any` / `xs:anyAttribute` (Phase 2 target)

#### Target files
- `classes/source/implementation/xsd/XSD_Validator_Impl_Validate.cpp`
- `classes/source/implementation/xsd/XSD_Validator_Impl_Parse_Types.cpp`

#### Deliverables
- XML validation tests covering element, attribute, and simple type rules
- precise error messages via `IValidator::Error`

### 5. Add identity and uniqueness constraints

#### Tasks
- Parse `xs:key`, `xs:keyref`, and `xs:unique`
- Validate key definitions across the document
- Validate key references resolve to declared keys
- Report duplicate keys and missing referents

#### Target files
- `classes/source/implementation/xsd/XSD_Validator_Impl_Parse.cpp`
- `classes/source/implementation/xsd/XSD_Validator_Impl_Validate.cpp`

#### Deliverables
- tests for key/keyref/unique constraint validation
- schema parse coverage for identity constraints

### 6. Complete XSD stringification and docs

#### Tasks
- Add schema stringify support for round-trip tests
- Update public docs and API references
- Add examples in `docs/Guide.md`, `docs/API.md`, and `README.md`

#### Target files
- `classes/source/implementation/xsd/XSD_Validator_Impl_Stringify.cpp`
- `docs/XML_Lib_Standards_Report.md`
- `docs/API.md`
- `docs/Guide.md`
- `README.md`

#### Deliverables
- schema round-trip tests
- documentation of XSD feature coverage and limitations

## Milestones

### Milestone 1 — Schema model and parsing
- Add XSD schema model and public API
- Parse `xs:schema`, `xs:element`, `xs:complexType`, `xs:simpleType`
- Add basic XSD parser tests

### Milestone 2 — Core validation
- Validate element content models and attribute rules
- Validate simple type facets and built-in types
- Add integration test via `XML::validate(xsdSource)`

### Milestone 3 — Composition and constraints
- Add `xs:include` / `xs:import`
- Add `xs:key` / `xs:keyref` / `xs:unique`
- Add extended schema validation tests

### Milestone 4 — Documentation and release
- Update documentation with supported XSD subset
- Finalize standards report and compliance checklist
- Confirm all tests pass

## File-level targets

- `classes/include/XSD_Validator.hpp`
- `classes/include/implementation/XSD_Impl.hpp`
- `classes/source/implementation/xsd/XSD_Validator.cpp`
- `classes/source/implementation/xsd/XSD_Validator_Impl.cpp`
- `classes/source/implementation/xsd/XSD_Validator_Impl_Parse.cpp`
- `classes/source/implementation/xsd/XSD_Validator_Impl_Parse_Elements.cpp`
- `classes/source/implementation/xsd/XSD_Validator_Impl_Parse_Types.cpp`
- `classes/source/implementation/xsd/XSD_Validator_Impl_Validate.cpp`
- `classes/source/implementation/xsd/XSD_Validator_Impl_Stringify.cpp`
- `tests/source/xsd/XML_Lib_Tests_XSD_Parse.cpp`
- `tests/source/xsd/XML_Lib_Tests_XSD_Validate_Elements.cpp`
- `tests/source/xsd/XML_Lib_Tests_XSD_Validate_Attributes.cpp`
- `tests/source/xsd/XML_Lib_Tests_XSD_Validate_SimpleTypes.cpp`
- `tests/source/xsd/XML_Lib_Tests_XSD_Stringify.cpp`
- `tests/files/xsd/`

## Acceptance checklist

- [x] `XSD_Validator` public API added
- [x] XSD schema model defined
- [x] Core schema constructs parsed
- [x] Schema composition supported
- [ ] XML validation against XSD supported
- [ ] Attribute and simple type facet validation covered
- [ ] Identity constraints supported
- [ ] Public docs updated with XSD support
- [ ] Phase 2 regression tests added and passing

## Notes

This plan assumes Phase 1 is complete and that the XML parser and DTD validator are stable. Phase 2 should proceed feature-by-feature with continuous test-driven validation.
