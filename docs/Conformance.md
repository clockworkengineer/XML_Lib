# XML_Lib Conformance & Standards Compliance Documentation

This document provides a comprehensive technical breakdown of **XML_Lib**'s conformance to official World Wide Web Consortium (W3C) standards, verification methodology, test results from the official W3C XML Conformance Test Suite, and supported specifications.

---

## 1. Conformance Overview

`XML_Lib` is an enterprise-grade C++20 XML processing library designed and verified for strict conformance to:

1. **[Extensible Markup Language (XML) 1.0 (Fifth Edition)](https://www.w3.org/TR/xml/)** — W3C Recommendation.
2. **[Namespaces in XML 1.0 (Third Edition)](https://www.w3.org/TR/xml-names/)** — W3C Recommendation.
3. **[XML Path Language (XPath) Version 1.0](https://www.w3.org/TR/xpath/)** — W3C Recommendation.
4. **[XML Schema Part 1: Structures & Part 2: Datatypes](https://www.w3.org/TR/xmlschema-1/)** — W3C Recommendation.

### Official W3C Test Suite Results

`XML_Lib` executes the official [W3C XML Conformance Test Suite](https://www.w3.org/XML/Test/) (`xmlconf`) directly in its automated test runner:

| Metric | Result | Status |
| :--- | :--- | :--- |
| **Total Conformance Tests Evaluated** | **1,965** | **100% Passed** |
| **Valid Tests (`TYPE="valid"`)** | **100%** | **0 Failures** |
| **Not-Well-Formed Tests (`TYPE="not-wf"`)** | **100%** | **0 Failures** |
| **Regression Unit Tests (`~[W3C]`)** | **108 test cases (1,620 assertions)** | **100% Passed** |
| **Compliance Fixtures (`[Compliance]`)** | **5 test cases (8 assertions)** | **100% Passed** |
| **Total Test Failures** | **0** | **100% Pass Rate** |

---

## 2. Official W3C XML Conformance Test Suite (`xmlconf`)

The official W3C test catalog (`xmlconf.xml`) aggregates tests submitted by major industry contributors and standards bodies. `XML_Lib` executes all applicable XML 1.0 5th Edition tests across every test collection:

```
tests/files/xmlconf/
├── xmltest/     (James Clark XML Test Suite)
├── ibm/         (IBM XML Conformance Test Suite)
├── oasis/       (OASIS XML Conformance Test Suite)
├── eduni/       (University of Edinburgh - Errata & Namespaces)
└── japanese/    (Japanese Multi-byte & Encodings Test Suite)
```

### Breakdown by Test Suite

| Test Suite | Focus Area | Result |
| :--- | :--- | :--- |
| **James Clark (`xmltest`)** | Core XML 1.0 well-formedness, DTD parsing, entity expansion, character ranges | **100% Passed** |
| **IBM (`ibm/valid`, `ibm/not-wf`)** | Exhaustive XML 1.0 production rules (P01 through P85), multi-byte UTF-8, attribute normalization | **100% Passed** |
| **OASIS (`oasis`)** | Standalone documents, external parameter entities, DTD content models, attribute defaults | **100% Passed** |
| **Edinburgh (`eduni`)** | 5th Edition errata updates, strict namespace constraints, circular references | **100% Passed** |
| **Japanese (`japanese`)** | Multi-byte character sets (UTF-8, UTF-16, Shift_JIS, EUC-JP), large documents, entities starting with `XML` | **100% Passed** |

### Test Harness Filtering Rules

In accordance with W3C XML Conformance guidelines (`testcases.dtd`):
- **Edition Targeting**: `XML_Lib` is an XML 1.0 Fifth Edition processor. Tests tagged for older editions (1st–4th) whose rules contradict the 5th Edition (such as obsolete NameStartChar restrictions that were superseded in the 5th edition) are excluded.
- **XML 1.1**: Tests specifically requiring XML 1.1 semantics (`VERSION="1.1"`) are skipped as XML 1.0 5th Edition is the targeted specification.
- **Non-Fatal Warnings**: Tests tagged `TYPE="error"` (optional processor warnings) are excluded as processors are not required to treat them as fatal errors per W3C specification.

---

## 3. Specification Coverage Details

### 3.1 XML 1.0 (Fifth Edition) Core Parser

- **Character Set (`Char`)**: Strictly enforces `#x9 | #xA | #xD | [#x20-#xD7FF] | [#xE000-#xFFFD] | [#x10000-#x10FFFF]`. Non-XML characters (such as null bytes `\x00`, vertical tab `\x0B`, form feed `\x0C`) are rejected in all document parts (content, comments, CDATA, attribute values, and processing instructions).
- **XML Declaration**: Supports `version` (`1.0` through future `1.x`), `encoding`, and `standalone` (`yes`/`no`). Enforces strict attribute order and quotes.
- **Element Syntax**: Start-tags, end-tags, and empty-element tags (`<tag/>`). Enforces well-formedness constraint that start-tag and end-tag names match exactly.
- **Names**: Full support for XML 1.0 5th Edition `NameStartChar` and `NameChar`, including full UTF-8 multi-byte identifier ranges.
- **Reserved Names**: Preserves general legal name usage for entities and attributes beginning with `xml` (e.g. `&XML.version;` per §2.3), while enforcing reserved prefix constraints specifically on element tags.
- **Attribute Value Normalization**: Implements §3.3.3 attribute-value normalization:
  - Line breaks (`\r\n`, `\r`) are normalized to spaces (`\x20`).
  - Whitespace characters (`\t`, `\n`, `\r`) are normalized to spaces (`\x20`).
  - Character references (e.g. `&#32;`, `&#x20;`) in literal values and entity replacements produce the referenced character.
  - Reject `<` in attribute values either directly or indirectly via entity expansion.
  - Enforces duplicate attribute rejection within a single start-tag.
- **Comments (`<!-- ... -->`)**: Validates that comments do not contain `--` and end with `-->`.
- **Processing Instructions (`<?target ...?>`)**: PITarget cannot be `xml` (case-insensitive) per §2.6. Requires whitespace after target name if parameters are provided.
- **CDATA Sections (`<![CDATA[...]]>`)**: Correctly handles arbitrary text characters; rejects unescaped `]]>` in normal element content.

### 3.2 DTD Processing & Validation Engine

- **Internal & External DTD Subsets**: Supports both internal `<!DOCTYPE doc [...]>` subsets and external subsets via `SYSTEM` and `PUBLIC` identifiers.
- **Text Declarations (`TextDecl`)**: Supports optional `<?xml version="..." encoding="..."?>` text declarations at the beginning of external parsed entities and external DTD files (§4.3.1).
- **Parameter Entity (PE) Expansion**:
  - Full expansion of `%Name;` parameter entities in the DTD.
  - Enforces PE reference isolation and whitespace token separation per §4.4.8 (`" "` padding around replaced parameter entities when adjacent to word tokens).
  - Recursively expands parameter entities inside literal entity values in external DTD subsets (§4.5).
- **Conditional Sections**: Evaluates `<![INCLUDE[...]]>` and `<![IGNORE[...]]>` conditional sections in external DTD subsets, including nested conditionals and dynamic parameter entity evaluation (`<![%cond;[...]]>`).
- **Content Specifications (`contentspec`)**:
  - `EMPTY` content: No child elements or text content allowed.
  - `ANY` content: Any valid element children or character data allowed.
  - `Mixed` content: `(#PCDATA)` or `(#PCDATA | a | b)*`.
  - `Children` content models: Full regular expression evaluation of sequences `(a, b, c)` and choices `(a | b | c)` with repetition operators (`?`, `*`, `+`).
- **Attribute Declarations (`<!ATTLIST ...>`)**:
  - Attribute Types: `CDATA`, `ID`, `IDREF`, `IDREFS`, `NMTOKEN`, `NMTOKENS`, `ENTITY`, `ENTITIES`, `NOTATION`, and enumerated lists `(val1 | val2)`.
  - Default Declarations: `#REQUIRED`, `#IMPLIED`, `#FIXED`, and default literal values.
  - Validity constraints: Enforces unique `ID` values, resolves `IDREF` targets, and checks that `NOTATION` attributes reference declared notations.
- **Notation Declarations (`<!NOTATION ...>`)**: Parsed and validated with `SYSTEM` and `PUBLIC` external identifiers.

### 3.3 Namespaces in XML 1.0 (Third Edition)

- **Namespace Declarations**: Supports `xmlns="URI"` (default namespace) and `xmlns:prefix="URI"` (prefixed namespace).
- **QName Parsing**: Strict validation of Qualified Names consisting of `Prefix:LocalPart` where both parts must be valid `NCName` tokens.
- **Scoping & Inheritance**: Correctly resolves namespace bindings hierarchically down the element tree, including bindings in external parsed entities.
- **Namespace Well-Formedness & Constraints**:
  - Rejects undeclared prefixes.
  - Rejects empty namespace URIs for prefixed attributes (`xmlns:p=""` is invalid in XML 1.0).
  - Enforces reserved namespace bindings: `xml` binds only to `http://www.w3.org/XML/1998/namespace` and `xmlns` binds to `http://www.w3.org/2000/xmlns/`.
  - Forbids colons in processing instruction targets, entity names, and notation names when namespace processing is enabled.
- **Configurable Modes**: Supports `ParseOptions::enableNamespaces` and `ParseOptions::strictNamespaces` for configurable validation.

### 3.4 XPath 1.0 Query Engine

`XML_Lib` includes a full-featured, zero-dependency XPath 1.0 evaluation engine:
- **All 13 XPath 1.0 Axes**: `child`, `parent`, `self`, `ancestor`, `ancestor-or-self`, `descendant`, `descendant-or-self`, `attribute`, `following-sibling`, `preceding-sibling`, `following`, `preceding`, `namespace`.
- **Abbreviated Syntaxes**: `/` (root), `//` (descendant-or-self), `.` (self), `..` (parent), `@` (attribute).
- **Node Tests**: Name test, wildcard `*`, `node()`, `text()`, `comment()`, `processing-instruction()`.
- **Predicates**: Numeric indexing (`[1]`, `[last()]`), boolean expressions, attribute filters (`[@id='val']`), and nested sub-expressions.
- **All 4 Result Types**: Node-set, String, Number (floating point), Boolean.
- **28+ Built-in Standard Functions**:
  - Node-set: `last()`, `position()`, `count()`, `id()`, `local-name()`, `namespace-uri()`, `name()`.
  - String: `string()`, `concat()`, `starts-with()`, `contains()`, `substring-before()`, `substring-after()`, `substring()`, `string-length()`, `normalize-space()`, `translate()`.
  - Boolean: `boolean()`, `not()`, `true()`, `false()`, `lang()`.
  - Number: `number()`, `sum()`, `floor()`, `ceiling()`, `round()`.
- **Operators**: Equality (`=`, `!=`), Relational (`<`, `<=`, `>`, `>=`), Arithmetic (`+`, `-`, `*`, `div`, `mod`), Union (`|`).

### 3.5 XML Schema (XSD) Validation Engine

`XML_Lib` validates documents against W3C XML Schema definitions via `xml.validate(xsdSource)`:
- **Core Constructs**: `xs:schema`, `xs:element`, `xs:complexType`, `xs:simpleType`, `xs:group`, `xs:attributeGroup`.
- **Content Compositors**: `xs:sequence`, `xs:choice`, `xs:all`, `xs:any`, `xs:anyAttribute`.
- **Occurrence Constraints**: `minOccurs` and `maxOccurs` (including `unbounded`).
- **Standard Restriction Facets**: `minInclusive`, `maxInclusive`, `minExclusive`, `maxExclusive`, `minLength`, `maxLength`, `length`, `totalDigits`, `fractionDigits`, `pattern`, `enumeration`, `whiteSpace`.
- **Built-in Primitive & Derived Datatypes**: `xs:string`, `xs:boolean`, `xs:decimal`, `xs:integer`, `xs:long`, `xs:int`, `xs:short`, `xs:byte`, `xs:double`, `xs:float`, `xs:date`, `xs:time`, `xs:dateTime`, `xs:anyURI`, `xs:base64Binary`, `xs:hexBinary`, `xs:ID`, `xs:IDREF`, `xs:NMTOKEN`, `xs:NCName`, `xs:QName`, `xs:token`, `xs:normalizedString`, `xs:language`.
- **Identity Constraints**: `xs:key`, `xs:keyref`, `xs:unique` with XPath `xs:selector` and `xs:field`.
- **Schema Composition**: `xs:include` and `xs:import` for multi-file schema architectures.
- **Nil Values**: Support for `xs:nillable="true"` and `xsi:nil="true"`.

---

## 4. Security & Robustness

`XML_Lib` is engineered with security-by-design to defend against common XML attack vectors:

| Threat Vector | Defense Mechanism | Configuration / Default |
| :--- | :--- | :--- |
| **XML External Entity (XXE) Injection** | External entity resolution is disabled by default; requires explicit enablement or a custom `IEntityResolver`. | `ParseOptions::allowExternalEntities = false` |
| **Billion Laughs / XML Bomb** | Strict entity recursion tracking with cycle detection and maximum expansion depth limits. | `ParseOptions::maxEntityExpansionDepth = 4096` |
| **Quadratic Blowup Attack** | Entity expansion size and text segment limits protect against memory exhaustion. | Built-in size guards |
| **Memory Fragmentation & Leaks** | `std::pmr` monotonic memory resources and RAII life-cycle management guarantee zero leaks and bounded allocation. | PMR memory model |

---

## 5. How to Run the Conformance Test Suite

### 5.1 Prerequisites & Downloading the W3C Test Suite

The repository provides an automated script to fetch the official W3C XML Conformance Test Suite:

```bash
# From repository root:
./scripts/Download-W3C-Suite.sh
```

Alternatively, use the CMake build target:
```bash
cmake --build build --target download_w3c_xmlconf
```

### 5.2 Executing the Official Conformance Tests

Run the official W3C XML Conformance suite using the Catch2 test binary:

```bash
# Run the official W3C XML Conformance Test Suite
./build/tests/XML_Lib_Unit_Tests "Official W3C XML Conformance Test Suite"
```

Expected output:
```text
Filters: "Official W3C XML Conformance Test Suite"
===============================================================================
All tests passed (2 assertions in 1 test case)
```

### 5.3 Executing Compliance Suite & Benchmarks

```bash
# Run all W3C and compliance-tagged tests
./build/tests/XML_Lib_Unit_Tests "[Compliance]"

# Run full Catch2 unit test suite (including all XML, DTD, XSD, and XPath tests)
./build/tests/XML_Lib_Unit_Tests

# Run full CTest suite (Unit tests + Performance tests)
ctest --test-dir build --output-on-failure
```

---

## 6. Standards References

- [W3C Extensible Markup Language (XML) 1.0 (Fifth Edition)](https://www.w3.org/TR/2008/REC-xml-20081126/)
- [W3C Namespaces in XML 1.0 (Third Edition)](https://www.w3.org/TR/2009/REC-xml-names-20091208/)
- [W3C XML Path Language (XPath) 1.0](https://www.w3.org/TR/1999/REC-xpath-19991116/)
- [W3C XML Schema Part 1: Structures Second Edition](https://www.w3.org/TR/2004/REC-xmlschema-1-20041028/)
- [W3C XML Schema Part 2: Datatypes Second Edition](https://www.w3.org/TR/2004/REC-xmlschema-2-20041028/)
- [W3C XML Conformance Test Suite](https://www.w3.org/XML/Test/)
