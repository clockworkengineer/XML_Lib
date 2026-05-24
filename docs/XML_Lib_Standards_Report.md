# XML_Lib Standards Compliance Report

## Overview
XML_Lib is a C++20 library for parsing, manipulating, and generating XML documents. It aims to provide efficient, modern XML support for C++ projects.

## Phase 1 Status
- Phase 1 compliance is complete for core XML syntax, declaration handling, encoding/BOM hardening, external DTD subset support, conditional DTD processing, and parser robustness.
- Regression tests have been added for all Phase 1 edge cases and are passing.

## Phase 2 Checklist
- [x] Add `XSD_Validator` public API and schema model
- [x] Parse core XSD constructs: `xs:schema`, `xs:element`, `xs:complexType`, `xs:simpleType`
- [x] Support `xs:sequence`, `xs:choice`, `xs:all`, and attribute declarations
- [x] Implement schema composition via `xs:include` / `xs:import`
- [x] Validate XML documents against XSD schemas
- [x] Enforce attribute constraints, simple type facets, and occurrence counts
- [x] Add `xs:key`, `xs:keyref`, and `xs:unique` support
- [x] Update API docs and standards report with Phase 2 coverage

## Phase 3 Status
- [x] Integrated a W3C-derived compliance harness for XML, DTD, XPath, and XSD fixtures.
- [x] Added regression coverage in `tests/source/compliance/XML_Lib_Tests_Compliance.cpp`.
- [x] Added supporting fixture directories under `tests/files/w3c/`.
- [x] Documented compliance support, verification commands, and remaining gaps.

## Compliance verification
Run the W3C-derived compliance subset from the built test executable:

```bash
./build/tests/XML_Lib_Unit_Tests -c "[Compliance]"
```

This target executes the compliance fixtures and verifies the supported XML, DTD, XPath, and XSD behaviors.

## Release validation
Release validation is automated using repository scripts and CI gating:

- `scripts/Linux-Build.sh` builds both Release and Debug configurations.
- `scripts/Linux-Run-Tests.sh` runs the complete unit-test suite in both builds.
- `scripts/Linux-Run-Compliance.sh` runs the W3C-derived compliance harness.
- `scripts/Linux-Run-Performance.sh` runs performance regression validation.

A dedicated CI pipeline stage ensures the repository validates build, test, performance, and compliance expectations before release.

## Supported XML Standards

### XML 1.0 Compliance
- **Parsing**: Handles well-formed XML documents, including prolog, declaration, elements, attributes, and content.
- **DTD Support**: Internal and external DTDs are parsed and validated. DTD element, attribute, and notation types are supported, including error handling for illegal combinations (e.g., multiple ID attributes).
- **Encoding**: Supports UTF-8, UTF-8 BOM, UTF-16BE, UTF-16LE, UTF-32BE, UTF-32LE. Throws errors for unsupported encodings (e.g., UTF-32 in declaration).
- **Error Handling**: Robust error reporting for syntax errors, unsupported values, extra content, and DTD validation issues.
- **Parser Robustness**: Syntax failures consistently throw `XML_Lib::SyntaxError`, and parser state is reset before each parse attempt so invalid input does not leave the parser in a corrupted state.
- **Namespaces**: Full support for W3C XML Namespaces (xmlns, prefix handling, QNames, scoping, per-element URI resolution, and attribute prefix validation).
- **XML Declaration**: Parses version, encoding, and standalone attributes, with strict validation.

### DTD Validation
- **Element Content Models**: Supports EMPTY, ANY, mixed content, and complex content models. Validates element content against DTD specifications using regex.
- **Attribute Types**: Supports CDATA, ID, IDREF, NMTOKEN, ENTITY, NOTATION, and value constraints (REQUIRED, IMPLIED, FIXED).
- **Notation and External References**: Handles public and system identifiers, validates external references.
- **Error Cases**: Detects and reports illegal DTD constructs, missing NOTATION attributes, and invalid content specifications.

### XSD Validation (Phase 2)
XML Schema Definition (XSD) validation is supported via `XML::validate(xsdSource)`. The implementation parses the XSD as XML using the existing parser, then validates the document against the schema.

**Supported Features:**
- Named and anonymous `xs:complexType` with `xs:sequence`, `xs:choice`, and `xs:all` compositors
- `xs:any` wildcard content and `xs:anyAttribute`
- Named `xs:simpleType` with all restriction facets: `minInclusive`, `maxInclusive`, `minExclusive`, `maxExclusive`, `pattern`, `enumeration`, `minLength`, `maxLength`, `length`, `totalDigits`, `fractionDigits`, `whiteSpace`
- All builtin types: `xs:string`, `xs:boolean`, `xs:integer` and subtypes (`xs:int`, `xs:long`, `xs:short`, etc.), `xs:decimal`, `xs:float`, `xs:double`, `xs:date`, `xs:time`, `xs:dateTime`, `xs:anyURI`, `xs:base64Binary`, `xs:hexBinary`, `xs:ID`, `xs:IDREF`, `xs:NMTOKEN`, `xs:NCName`, `xs:QName`, `xs:token`, `xs:normalizedString`, `xs:language`
- Attribute declarations: `use="required"`, `use="optional"`, `use="prohibited"`, `fixed`, `default`
- `default` attribute values are applied during validation without mutating the document tree
- `xs:include` / `xs:import` schema composition
- `xs:key`, `xs:keyref`, `xs:unique` identity constraints
- `minOccurs`/`maxOccurs` on elements (0=optional, unbounded=unlimited)
- Inline anonymous complex and simple types on element declarations
- `xs:nillable` elements with `xsi:nil="true"`

### XPath 1.0
XPath 1.0 query evaluation is supported via the `XPath` class and the `xml.xpath()` convenience method.

**Supported Features:**
- All 13 XPath 1.0 axes: `child`, `parent`, `self`, `ancestor`, `ancestor-or-self`, `descendant`, `descendant-or-self`, `attribute`, `following-sibling`, `preceding-sibling`, `following`, `preceding`, `namespace`
- Abbreviated syntax: `/` (absolute path), `//` (descendant-or-self shorthand), `.` (self), `..` (parent), `@` (attribute)
- Predicates: positional (`[1]`, `[last()]`), numeric comparison, boolean, and attribute/element value equality (`[@attr='value']`)
- Node tests: `name`, `*` (wildcard), `node()`, `text()`, `comment()`, `processing-instruction()`
- All four result types: node-set, string, number, boolean
- 28+ built-in functions: `count`, `string`, `number`, `boolean`, `not`, `true`, `false`, `concat`, `contains`, `starts-with`, `substring`, `substring-before`, `substring-after`, `string-length`, `normalize-space`, `translate`, `name`, `local-name`, `namespace-uri`, `position`, `last`, `sum`, `floor`, `ceiling`, `round`, `id`, `lang`
- Union expressions (`expr1 | expr2`)
- All comparison operators: `=`, `!=`, `<`, `<=`, `>`, `>=`
- Arithmetic operators: `+`, `-`, `*`, `div`, `mod`
- Qualified names (namespace-prefixed) in node tests

**Not Yet Implemented:**
- XPath 2.0/3.x features (sequences, types, `for` expressions, etc.)

### Other Limitations & Deviations
- **Namespaces**: Supported — W3C XML Namespace declarations, prefix scoping, QName parsing, URI resolution, and well-formedness enforcement are all implemented.
- **Encoding**: Only a subset of encodings are supported; others are rejected.
- **XPath**: XPath 1.0 fully implemented — all 13 axes, 28+ built-in functions, predicates, all result types, abbreviated syntax. See `XPath` class.
- **Mixed Content**: Strict validation for mixed content; errors for illegal specifications (e.g., #PCDATA not first).
- **Error Messages**: Detailed and standard-compliant error messages for most cases.

## Summary Table
| Feature                | Supported | Notes |
|------------------------|-----------|-------|
| XML 1.0 Syntax         | Yes       | Strict validation |
| DTD Parsing/Validation | Yes       | Internal & external |
| Encoding               | Partial   | UTF-8/16/32 only |
| Namespaces             | Yes       | W3C XML Namespaces — prefix, URI, QName, scoping |
| Schema (XSD)           | Partial   | Phase 1: elements, attributes, simple types, all restriction facets |
| XPath                  | Yes       | XPath 1.0: all 13 axes, 28+ functions, all result types |
| Error Handling         | Yes       | Detailed errors |

## References
- [W3C XML 1.0 Specification](https://www.w3.org/TR/REC-xml/)
- [W3C DTD Specification](https://www.w3.org/TR/REC-xml/#dt-doctype)

## Conclusion
XML_Lib is largely compliant with XML 1.0 and DTD standards, with robust error handling and strict validation. Phase 2 XSD schema validation has been implemented, covering element content models, attributes, builtin simple types, all standard restriction facets, schema composition, and identity constraints. XPath 1.0 has been fully implemented, covering all 13 axes, 28+ built-in functions, all result types (node-set, string, number, boolean), predicates, abbreviated syntax, and union expressions. For projects requiring DTD or XSD validation, XPath queries, and strict XML syntax, XML_Lib is a suitable choice.
