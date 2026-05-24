# XML_Lib Phase 1 Compliance Plan

## Goal
Complete Phase 1 of the XML compliance roadmap by verifying and closing gaps in core XML behavior. This plan defines concrete tasks, priorities, metrics, and file-level targets.

## Timeline
Target duration: 2 weeks.

- Week 1: analysis, parser validation, and encoding hardening
- Week 2: DTD/parser edge case tests, external subset support, and documentation updates

## Success criteria
Phase 1 is complete when:
- XML 1.0 syntax and character handling is verified against key W3C edge cases
- XML declaration and BOM behavior is consistent across supported entrypoints
- external DTD subsets and conditional sections are parsed correctly
- unsupported encodings are rejected with consistent, documented errors
- there are regression tests covering all new Phase 1 cases

## Work breakdown

### 1. XML 1.0 character and syntax conformance

#### Tasks
- Review parser handling for legal and illegal XML characters.
- Add tests for:
  - allowed character ranges
  - forbidden control characters
  - valid and invalid `<![CDATA[...]]>` content
  - processing instructions
  - comment syntax variations
- Validate namespace syntax parsing with edge cases:
  - default namespace declarations
  - namespace prefix redefinition
  - undeclared prefixes
  - `xmlns:xml` handling

#### Target files
- `classes/source/implementation/xml/parser/Default_Parser.cpp`
- `classes/source/XML.cpp`
- `classes/source/implementation/xml/XML_Impl.cpp`

#### Deliverables
- new parser tests in `tests/source/xml` or `tests/source/parser`
- list of confirmed supported XML character cases
- documented parser behavior for invalid XML characters

### 2. XML declaration, BOM, and encoding hardening

#### Tasks
- Confirm the parser accepts and rejects BOMs correctly across input sources.
- Validate XML declaration support for:
  - `<?xml version="1.0"?>`
  - `<?xml version="1.0" encoding="UTF-8"?>`
  - `<?xml version="1.0" standalone="yes"?>`
  - invalid declaration attribute ordering
- Add tests for file- and buffer-based inputs with UTF-8 and UTF-16 BOMs.
- Verify unsupported encoding handling and error messages.

#### Target files
- `classes/source/implementation/xml/file/XML_File.cpp`
- `classes/source/implementation/io/XML_FileSource.hpp`
- `classes/source/implementation/xml/parser/Default_Parser.cpp`

#### Deliverables
- encoding coverage report
- regression tests for BOM and declaration variants
- consistent parser error messages for unsupported encodings

### 3. External DTD subset and conditional section support

#### Tasks
- Verify support for external subset declarations in `<!DOCTYPE>`.
- Add or improve support for conditional sections:
  - `<![ IGNORE [ ... ]]>`
  - `<![ INCLUDE [ ... ]]>`
- Validate parameter entity expansion rules inside DTDs.
- Add tests for external subset loading and conditional content handling.

#### Target files
- `classes/source/implementation/dtd/DTD_Validator_Impl_Parse.cpp`
- `classes/source/implementation/dtd/DTD_Validator_Impl_Validator.cpp`
- `classes/source/implementation/xml/parser/Default_Parser.cpp`

#### Deliverables
- end-to-end DTD parsing tests for external subsets
- tests that verify conditional section ignore/include semantics
- documentation of supported DTD subset behavior

### 4. Parser robustness and regression coverage

#### Tasks
- Review parser error paths and confirm consistent exception types.
- Add regression coverage for parser state recovery on invalid XML.
- Document any parser assumptions or deviations from XML 1.0.

#### Target files
- `classes/source/implementation/xml/parser/Default_Parser.cpp`
- `classes/source/XML.cpp`
- `tests/source/xml` and `tests/source/dtd`

#### Deliverables
- expanded regression suite covering phase 1 edge cases
- a short compliance note added to `docs/XML_Lib_Standards_Report.md`

## Milestones

### Milestone 1 (End of Week 1)
- Completed character/conformance analysis
- Added tests for XML character ranges and declaration variants
- Confirmed BOM handling behavior
- Drafted parser behavior notes

### Milestone 2 (End of Week 2)
- Completed external subset and conditional section support
- Added regression tests for DTD conditional sections and parameter entities
- Updated standards report and roadmap status
- All Phase 1 tests passing

## Acceptance checklist

- [x] XML character range handling verified
- [x] XML declaration and encoding behavior verified
- [x] BOM handling tests added
- [x] External DTD subset parsing supported
- [x] DTD conditional sections supported
- [x] Parameter entity DTD behavior validated
- [x] Parser error handling consistency confirmed
- [x] Regression tests added for all new cases
- [x] Documentation updated with Phase 1 findings

## Notes

- Phase 1 is complete: all plan items have been validated with regression tests and documentation updates.
- Parser robustness regression coverage was added and parser state recovery validation has been documented.
- All relevant XML and DTD unit tests for Phase 1 are passing.

If any Phase 1 item reveals a deeper parser gap, stop and document the gap immediately. The goal is to make the core XML parser reliable before moving to XSD Phase 2.
