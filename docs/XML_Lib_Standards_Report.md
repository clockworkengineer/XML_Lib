# XML_Lib Standards Compliance Report

## Overview
**XML_Lib** is an enterprise-grade C++23 library for parsing, streaming, validating, manipulating, and generating XML documents. It provides high-throughput, modern XML support for desktop, server, cloud, and resource-constrained embedded systems.

---

## Standards Compliance Summary

| Specification | Version / Status | Implementation Class | Conformance Status |
| :--- | :--- | :--- | :--- |
| **W3C XML** | 1.0 (Fifth Edition) | `XML`, `XMLReader`, `XMLWriter` | **100% Conformance** (1,965 W3C tests evaluated, 0 failures) |
| **W3C Namespaces in XML** | 1.0 (Third Edition) | `XML`, `NamespaceValidator` | **100% Conformance** (Full QName scoping and URI mapping) |
| **W3C XML Schema (XSD)** | 1.0 Structures & Datatypes | `XSD_Validator`, `XSD_Schema` | **Full Support** (Compositors, facets, identity constraints) |
| **W3C XPath** | 1.0 Recommendation | `XPath`, `XPathExpression` | **Full Support** (All 13 axes, 28+ functions, AST caching) |
| **OASIS XML Catalogs** | V1.1 Standard | `OASIS_Catalog` | **Full Support** (`system`, `public`, `rewriteSystem`, `rewriteURI`) |
| **ISO/IEC C++ Standard** | ISO/IEC 14882:2023 (C++23) | Entire Codebase | **Full Compliance** (`std::expected`, monadic optionals, ranges) |

---

## Official W3C XML Conformance Status
- **100% Pass Rate** on the official [W3C XML Conformance Test Suite](https://www.w3.org/XML/Test/) (`xmlconf`).
- **1,965 test cases evaluated, 0 failures**.
- 100% of `valid` tests pass; 100% of `not-wf` tests correctly detected and rejected.
- See the dedicated [W3C Conformance Documentation](Conformance.md) for complete technical breakdown and test execution instructions.

---

## Full Test Suite Metrics
- **Total Test Cases**: 127
- **Total Assertions**: 2,837
- **Pass Rate**: 100% (0 failures, 0 leaks, 0 sanitizer warnings)

---

## Supported XML Standards Breakdown

### 1. XML 1.0 (Fifth Edition) Core & Streaming
- **DOM & Streaming**: Dual engines providing both tree-based DOM (`XML`) and constant-memory ($O(1)$) pull parsing (`XMLReader`) and push serialization (`XMLWriter`).
- **DTD Support**: Internal and external DTDs are parsed and validated. DTD element models (`EMPTY`, `ANY`, sequence, choice), attribute types (`CDATA`, `ID`, `IDREF`, `NMTOKEN`, `ENTITY`, `NOTATION`), and value constraints (`#REQUIRED`, `#IMPLIED`, `#FIXED`) are enforced.
- **Encoding**: Automatic BOM detection and transcoding for UTF-8, UTF-8 BOM, UTF-16BE, UTF-16LE, UTF-32BE, and UTF-32LE.
- **Zero-Copy I/O**: `MMapSource` maps files directly into address space using OS page cache primitives (`mmap` on POSIX, `CreateFileMappingA` on Windows).
- **Error Handling**: Throwing (`SyntaxError`, `Node::Error`, `IValidator::Error`) and non-throwing C++23 `std::expected` (`XML::parseExpected`, `XMLReader::readExpected`, `XML::validateExpected`).

### 2. W3C XML Schema (XSD)
- Named and anonymous `xs:complexType` with `xs:sequence`, `xs:choice`, and `xs:all` compositors.
- `xs:any` wildcard content and `xs:anyAttribute`.
- Named `xs:simpleType` with all restriction facets (`minInclusive`, `maxInclusive`, `pattern`, `enumeration`, `length`, etc.).
- All builtin primitive and derived types (`xs:string`, `xs:boolean`, `xs:integer` hierarchy, `xs:dateTime`, `xs:date`, `xs:decimal`, `xs:base64Binary`, etc.).
- `xs:key`, `xs:keyref`, `xs:unique` identity constraints.
- Pre-compiled immutable schemas via `XSD_Schema` for thread-safe concurrent validation.

### 3. W3C XPath 1.0
- All 13 XPath 1.0 axes supported (`child`, `descendant`, `parent`, `ancestor`, `following-sibling`, etc.).
- 28+ built-in XPath core functions.
- Union expressions (`|`), comparisons, arithmetic, and positional predicates.
- Pre-compiled query AST caching via `XPathExpression`.

### 4. OASIS XML Catalogs 1.1
- Native implementation of OASIS XML Catalogs 1.1 (`OASIS_Catalog`).
- Offline mapping of external schemas, DTDs, and entities without network requests.
- Full support for `system`, `public`, `rewriteSystem`, `rewriteURI`, and direct in-memory content.
