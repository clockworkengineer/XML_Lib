# Plan for Implementing XML Namespace Support in XML_Lib

## 1. Analysis Summary
- **Current State:**
  - No explicit support for XML namespaces (xmlns, prefix handling) in parsing, validation, or serialization.
  - Some code (e.g., `Element`, `addNameSpace`, `hasNameSpace`) hints at partial or planned support, but not full W3C XML Namespace compliance.
  - Namespace attributes are stored, but not used for element/attribute name resolution or scoping.
  - No support for qualified names (QNames) or namespace URI resolution.

## 2. Goals
- Full support for XML Namespaces per [W3C Recommendation](https://www.w3.org/TR/xml-names/):
  - Parse and recognize namespace declarations (`xmlns`, `xmlns:prefix`).
  - Track namespace scope and resolve prefixes to URIs for elements and attributes.
  - Support QNames for elements and attributes.
  - Enforce namespace well-formedness constraints.
  - Serialize XML with correct namespace declarations and prefixes.

## 3. Concrete Implementation Plan

### A. Data Model Changes
- Update `Element` and `XMLAttribute` to store and expose namespace URI and prefix for each element/attribute.
- Add helper functions to resolve prefix → URI and URI → prefix mappings.
- Ensure `Node` and related classes can propagate namespace context.

### B. Parsing Changes
- In `Default_Parser::parseElement` and `parseAttributes`:
  - Detect and process `xmlns` and `xmlns:prefix` attributes.
  - Maintain a stack of active namespace mappings (prefix → URI) per element scope.
  - For each element/attribute, split name into prefix:local and resolve prefix to URI.
  - Store resolved URI and prefix in the data model.
  - Raise errors for undefined prefixes or illegal redeclarations.

### C. API Changes
- Add API to query namespace URI, prefix, and local name for elements/attributes.
- Add API to get all in-scope namespaces for a node.
- Update `addNameSpace` and related methods for correct scoping.

### D. Serialization Changes
- Update stringification logic to emit correct `xmlns` declarations and QNames.
- Ensure output XML is namespace-well-formed.

### E. Testing
- Add unit tests for:
  - Namespace declaration and scoping.
  - QName parsing and resolution.
  - Error cases (undefined prefix, duplicate declaration, etc.).
  - Round-trip parse/serialize with namespaces.

### F. Documentation
- Update README and standards report to document namespace support and usage.

## 4. Milestones
1. Refactor data model for namespace support.
2. Implement namespace-aware parsing.
3. Update API and serialization.
4. Add tests and documentation.

---
This plan provides a step-by-step roadmap to add robust XML namespace support to XML_Lib, ensuring compliance with XML standards and interoperability with namespace-aware XML documents.
