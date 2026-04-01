# XML_Lib Standards Compliance Report

## Overview
XML_Lib is a C++20 library for parsing, manipulating, and generating XML documents. It aims to provide efficient, modern XML support for C++ projects.

## Supported XML Standards

### XML 1.0 Compliance
- **Parsing**: Handles well-formed XML documents, including prolog, declaration, elements, attributes, and content.
- **DTD Support**: Internal and external DTDs are parsed and validated. DTD element, attribute, and notation types are supported, including error handling for illegal combinations (e.g., multiple ID attributes).
- **Encoding**: Supports UTF-8, UTF-8 BOM, UTF-16BE, UTF-16LE, UTF-32BE, UTF-32LE. Throws errors for unsupported encodings (e.g., UTF-32 in declaration).
- **Error Handling**: Robust error reporting for syntax errors, unsupported values, extra content, and DTD validation issues.
- **Namespaces**: Full support for W3C XML Namespaces (xmlns, prefix handling, QNames, scoping, per-element URI resolution, and attribute prefix validation).
- **XML Declaration**: Parses version, encoding, and standalone attributes, with strict validation.

### DTD Validation
- **Element Content Models**: Supports EMPTY, ANY, mixed content, and complex content models. Validates element content against DTD specifications using regex.
- **Attribute Types**: Supports CDATA, ID, IDREF, NMTOKEN, ENTITY, NOTATION, and value constraints (REQUIRED, IMPLIED, FIXED).
- **Notation and External References**: Handles public and system identifiers, validates external references.
- **Error Cases**: Detects and reports illegal DTD constructs, missing NOTATION attributes, and invalid content specifications.

### XSD Validation (Phase 1)
XML Schema Definition (XSD) validation is supported via `XML::validate(xsdSource)`. The implementation parses the XSD as XML using the existing parser, then validates the document against the schema.

**Supported Features:**
- Named and anonymous `xs:complexType` with `xs:sequence`, `xs:choice`, and `xs:all` compositors
- Named `xs:simpleType` with all restriction facets: `minInclusive`, `maxInclusive`, `minExclusive`, `maxExclusive`, `pattern`, `enumeration`, `minLength`, `maxLength`, `length`, `totalDigits`, `fractionDigits`, `whiteSpace`
- All builtin types: `xs:string`, `xs:boolean`, `xs:integer` and subtypes (`xs:int`, `xs:long`, `xs:short`, etc.), `xs:decimal`, `xs:float`, `xs:double`, `xs:date`, `xs:time`, `xs:dateTime`, `xs:anyURI`, `xs:base64Binary`, `xs:hexBinary`, `xs:ID`, `xs:IDREF`, `xs:NMTOKEN`, `xs:NCName`, `xs:QName`, `xs:token`, `xs:normalizedString`, `xs:language`
- Attribute declarations: `use="required"`, `use="optional"`, `use="prohibited"`, `fixed`, `default`
- `xs:anyAttribute` wildcard (suppresses undeclared-attribute errors)
- `minOccurs`/`maxOccurs` on elements (0=optional, unbounded=unlimited)
- Inline anonymous complex and simple types on element declarations
- `xs:element` top-level declarations (root element dispatch)

**Not Yet Implemented (Phase 2):**
- `xs:extension` / `xs:restriction` type inheritance in `complexContent`
- `xs:include` and `xs:import`
- `xs:key`, `xs:keyref`, `xs:unique` identity constraints
- `xs:notation`

### Other Limitations & Deviations
- **Namespaces**: Supported — W3C XML Namespace declarations, prefix scoping, QName parsing, URI resolution, and well-formedness enforcement are all implemented.
- **Encoding**: Only a subset of encodings are supported; others are rejected.
- **XPath**: Mentioned in README, but not found in codebase. Likely not implemented.
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
| XPath                  | No        | Not implemented |
| Error Handling         | Yes       | Detailed errors |

## References
- [W3C XML 1.0 Specification](https://www.w3.org/TR/REC-xml/)
- [W3C DTD Specification](https://www.w3.org/TR/REC-xml/#dt-doctype)

## Conclusion
XML_Lib is largely compliant with XML 1.0 and DTD standards, with robust error handling and strict validation. Phase 1 XSD schema validation has been implemented, covering element content models, attributes, all builtin types, and all restriction facets. XPath, advanced XSD type inheritance, and schema composition (include/import) are not yet implemented. For projects requiring DTD or XSD validation and strict XML syntax, XML_Lib is a suitable choice.
