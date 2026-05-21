# XML_Lib Attribute Support Refactor Plan

## Purpose

This plan aligns XML_Lib's attribute implementation with the documented library attribute behavior in `docs/XML_Lib_Standards_Report.md`, `docs/xsd_implementation_plan.md`, `docs/API.md`, and the XSD attribute examples.

It focuses on the concrete attribute-related gaps currently present in the codebase and defines a step-by-step refactor roadmap.

## Current State Summary

- XML parsing rejects duplicate attributes and enforces valid attribute values.
- `Element` stores attributes and namespace declarations separately, with support for attribute lookup, iteration, and namespace resolution.
- DTD validation already supports attribute value types and DTD attribute constraints (`#REQUIRED`, `#IMPLIED`, `#FIXED`, default injection, ID/IDREF, NMTOKEN, ENTITY, NOTATION, enumeration, etc.).
- XSD validation parses attribute declarations and validates `use="required"`, `use="prohibited"`, `fixed`, `default`, and `xs:anyAttribute`.
- `XSD_Validator_Impl_Validate.cpp` currently enforces required/prohibited/fixed and undeclared attributes, but does not fully use parsed `default` values in validation.
- `XSD_AttributeDecl` currently holds `typeRef`, `use`, `defaultValue`, and `fixedValue`.

## Gaps and Issues

1. **XSD default attribute values are parsed but not applied in validation.**
   - `XSD_Validator_Impl_Validate.cpp::validateAttributes()` checks required/prohibited and fixed values, but missing optional attributes with `defaultValue` are not accounted for.
   - The documented plan and standards report expect `default` support to be present.

2. **`xs:anyAttribute` semantics need regression coverage.**
   - The validator currently skips undeclared attribute checks when `hasAnyAttribute` is true, but tests should prove this with attributes plus namespace-related edge cases.

3. **Attribute validation logic is monolithic and should be refactored for clarity.**
   - `validateAttributes()` should be split into smaller validation stages: presence, fixed/default semantics, simple type validation, unknown attribute validation.

4. **Documentation and test coverage are incomplete for attribute features.**
   - `README.md` and `docs/XML_Lib_Standards_Report.md` declare support for default attribute values, but no explicit test or example currently demonstrates it.

## Refactor Objectives

1. Implement full XSD attribute behavior for Phase 1:
   - `use="required"`
   - `use="optional"`
   - `use="prohibited"`
   - `fixed`
   - `default`
   - `xs:anyAttribute`

2. Preserve and verify existing DTD attribute behavior.

3. Refactor attribute validation into clear, testable subcomponents.

4. Add missing regression coverage for XSD default attribute handling and `xs:anyAttribute`.

## Concrete Tasks

### Task 1: Refactor XSD attribute validation

- In `classes/source/implementation/xsd/XSD_Validator_Impl_Validate.cpp`:
  1. Extract `validateAttributePresence()` from `validateAttributes()`.
  2. Extract `validateAttributeFixedValue()` and `validateAttributeDefaultValue()` logic.
  3. Extract `validateDeclaredAttributes()` and `validateUndeclaredAttributes()`.

- Ensure validation order matches documented XSD plan:
  1. required attributes
  2. prohibited attributes
  3. unknown attributes unless `xs:anyAttribute`
  4. simple type validation
  5. fixed value validation
  6. default value handling in error context

### Task 2: Implement XSD default attribute support

- Change `validateAttributes()` to treat a missing optional attribute with `declAttr.defaultValue` as a logical value for type and fixed checks.
- Do not mutate the in-memory XML tree in Phase 1; use the default value only for validation and error context.
- For example:
  - If `declAttr.defaultValue` exists and the attribute is absent, treat the attribute as present for type validation and fixed-value validation.
  - If `declAttr.defaultValue` exists and the attribute is present, validate the actual value normally.

### Task 3: Harden `xs:anyAttribute` handling

- Confirm that `type.hasAnyAttribute` is set by parsing `xs:anyAttribute` in `classes/source/implementation/xsd/XSD_Validator_Impl_Parse.cpp`.
- Ensure `validateAttributes()` skips undeclared attribute errors for all extra attributes except namespace declarations and XML namespace attributes.
- Add regression tests for:
  - `xs:anyAttribute` with undeclared user attributes
  - `xs:anyAttribute` combined with namespace declarations

### Task 4: Add missing tests

- Extend `tests/source/xsd/XML_Lib_Tests_XSD.cpp` with explicit cases for:
  - Optional attribute with `default` value passes validation.
  - Missing default attribute does not generate an error.
  - `default` attribute values are used for type/fixed validation when the attribute is absent.
  - `xs:anyAttribute` allows unknown attributes but not prohibited or undeclared attributes when the wildcard is absent.

- Optionally add an example in `examples/source/XML_XSD_Attributes_Example.cpp` demonstrating default attribute behavior.

### Task 5: Update documentation

- Update `docs/XML_Lib_Standards_Report.md` and `docs/API.md` to clearly describe:
  - XSD default attribute support semantics
  - `xs:anyAttribute` behavior
  - The fact that Phase 1 validation uses default values read-only (no AST mutation)

- Add a short plan entry to `docs/xsd_implementation_plan.md` if needed, confirming the default-value behavior and test coverage.

### Task 6: Code quality and maintenance

- Review `classes/include/implementation/nodes/XML_Element.hpp` and `XMLAttribute` API for any attribute-related API improvements that support the refactor.
- Keep the existing reverse-search semantics of `XMLAttribute::contains()` and `XMLAttribute::find()` to preserve duplicate-attribute detection and consistent lookup behavior.
- Prefer `std::span` for internal attribute operations where possible to avoid accidental copies.

## Files to Modify

- `classes/source/implementation/xsd/XSD_Validator_Impl_Validate.cpp`
- `classes/source/implementation/xsd/XSD_Validator_Impl_Parse.cpp`
- `tests/source/xsd/XML_Lib_Tests_XSD.cpp`
- `docs/XML_Lib_Standards_Report.md`
- `docs/API.md`
- Optional: `examples/source/XML_XSD_Attributes_Example.cpp`

## Success Criteria

- `XSD_Validator_Impl_Validate.cpp` correctly applies `default` values during attribute validation without mutating the XML tree.
- Required, optional, prohibited, fixed, and anyAttribute behaviors all pass regression tests.
- Documentation reflects the implemented attribute semantics accurately.
- Existing DTD attribute validation remains unchanged and continues to pass.

## Next Step

Begin by refactoring `validateAttributes()` into smaller functions and implementing `default` value logic in `classes/source/implementation/xsd/XSD_Validator_Impl_Validate.cpp`, then add the missing test cases.
