# XML_Lib

[![CI](https://github.com/clockworkengineer/XML_Lib/actions/workflows/ci.yml/badge.svg)](https://github.com/clockworkengineer/XML_Lib/actions/workflows/ci.yml)
[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)](https://en.cppreference.com/w/cpp/23)
[![Version: 1.4.0](https://img.shields.io/badge/Version-1.4.0-orange.svg)](CHANGELOG.md)
[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE.txt)
[![Buy Me a Coffee](https://img.shields.io/badge/Donate-Buy%20Me%20A%20Coffee-orange.svg)](https://www.buymeacoffee.com/clockworkengineer)

**XML_Lib** is an enterprise-grade, high-performance C++23 library for parsing, streaming, querying, validating, manipulating, and serialising XML documents. Engineered with a strict **SOLID** architecture, modern C++ idioms, and zero external runtime dependencies, it provides an intuitive, robust API designed for modern desktop/server applications, large-scale data ingestion pipelines, and resource-constrained embedded environments.

---

## Key Highlights

- **Complete XML 1.0 & Namespaces**: Full conformance to W3C XML 1.0 (Fifth Edition) and XML Namespaces 1.0 specifications, including comments, CDATA, processing instructions, and qualified names (QNames).
- **$O(1)$-Memory Streaming Pull & Push Parsers**:
  - **`XMLReader`**: High-speed forward-only streaming pull-parser cursor enabling continuous processing of multi-gigabyte XML streams with constant, negligible RAM usage (< 10 MB).
  - **`XMLWriter`**: Streaming push serializer emitting directly to destinations or files with automatic well-formed tag stack verification, formatting, and character escaping.
- **Dual Validation Engines with Pre-Compilation**:
  - **DTD Validation**: Supports internal and external DTD subsets, element content models (`EMPTY`, `ANY`, mixed, sequence/choice), and attribute declarations (`ID`, `IDREF`, `NMTOKEN`, `ENTITY`, `NOTATION`).
  - **W3C XML Schema (XSD) Validation**: Full XSD validation support (`xs:sequence`, `xs:choice`, `xs:all`, `xs:any`, `xs:anyAttribute`, identity constraints `xs:key`/`xs:keyref`/`xs:unique`, schema composition via `xs:include`/`xs:import`, built-in types, and facets).
  - **`XSD_Schema`**: Thread-safe schema pre-compilation allowing immutable schemas to be parsed once and reused across concurrent worker threads.
- **XPath 1.0 Engine & Query Caching**:
  - Evaluates complex XPath expressions across the document tree with support for all 13 axes, 28+ built-in functions, predicates, and abbreviated syntax (`//`, `@`, `.`, `..`).
  - **`XPathExpression`**: Pre-compiles and caches XPath ASTs to eliminate re-lexing and re-parsing overhead in hot query loops.
- **Zero-Copy Memory-Mapped File I/O (`MMapSource`)**:
  - Leverages OS kernel page caching (`mmap` on POSIX, `CreateFileMappingA` on Windows) for near-instantaneous ingestion of massive XML files without userspace buffer copies.
- **OASIS XML Catalogs 1.1 Resolution (`OASIS_Catalog`)**:
  - Implements OASIS XML Catalogs 1.1 standard (`system`, `public`, `rewriteSystem`, `rewriteURI`, and memory overrides) for secure, air-gapped schema and external entity resolution.
- **Modern C++23 & C++20 Idiomatic API**:
  - **Non-throwing APIs**: C++23 `std::expected<std::unique_ptr<XML>, XML_Error>` via `XML::parseExpected()`, `XMLReader::readExpected()`, and `XML::validateExpected()`.
  - **Monadic Lookups**: C++23 `std::optional` accessors (`node.findChild()`, `element.findAttribute()`, `reader.findAttribute()`) supporting `.and_then()`, `.transform()`, and `.value_or()`.
  - **Multidimensional Subscripting**: C++23 multidimensional `operator[]` on `Node` (`root["database", "credentials"]`, `root["book", 0]`).
  - **Ranges & Views**: Standard C++20 range views (`root.elements()`, `root.elements("book")`, `elementAttributes()`).
  - **Type Constraints**: Concept-based templates via `XML_Concepts.hpp` (`XMLNodeLike`, `XMLSourceLike`, etc.).
- **High-Performance Memory Model**: Uses `std::pmr` (Polymorphic Memory Resources) monotonic buffer arenas to eliminate heap allocation bottlenecks.
- **Security by Design**: Hardened against Billion Laughs / quadratic blowup (XML bomb) recursion and external entity injection (XXE) attacks with strictly configurable resource limits (`ParseOptions`).
- **Embedded & Minimal Footprints**: Optional embedded configuration supporting `-fno-exceptions`, `-fno-rtti`, and dead-code stripping (`--gc-sections`).

---

## 100% SOLID Architecture

XML_Lib is decoupled across dedicated role interfaces and services following the SOLID design principles:

| Subsystem | SOLID Principles | Key Abstractions & Interfaces | Architectural Benefits |
| :--- | :--- | :--- | :--- |
| **Input Stream I/O** | **ISP, DIP, SRP** | `ISource`, `BufferSource`, `FileSource`, `MMapSource` | Isolated stream normalization, zero-copy kernel mmap, BOM detection |
| **Output Destination I/O**| **ISP, SRP** | `IDestination`, `BufferDestination`, `FileDestination` | Role-segregated serialization targets with zero name lookup clashes |
| **Streaming Pipeline** | **SRP, ISP, OCP** | `XMLReader`, `XMLWriter` | $O(1)$-memory forward-only pull parsing and streaming generation |
| **Entity & Security** | **ISP, SRP, DIP** | `IEntityRegistry`, `IEntityExpander`, `ISecurityPolicyManager`, `IEntityResolver`, `OASIS_Catalog` | Modular entity expansion, XXE defense, OASIS Catalog 1.1 offline resolution |
| **Parser & Serializer** | **LSP, OCP, DIP** | `IParser`, `IStringify`, `INodeSerializer`, `IXMLParseStage` | Pluggable parsing stages and strategy-based formatting maps |
| **Validator Pipeline** | **OCP, DIP, SRP** | `ISchemaParser`, `ISchemaValidator`, `IValidatorRegistry`, `XSD_Schema` | Pluggable validation pipeline with thread-safe pre-compiled schemas |
| **Visitor & Traversal** | **ISP, OCP** | `IVisitorRoles`, `NodeVisitorAdapter`, `IAction` | Granular role visitor callbacks (`IElementVisitor`, `ICommentVisitor`) |
| **XPath & Query Cache**| **SRP, DIP, OCP** | `IXPathNodeAdapter`, `XPath`, `XPathExpression`, `XML_FileIO` | Decoupled XPath navigation and AST query caching |

For comprehensive architectural documentation, see the [SOLID Architecture Guide](docs/SOLID_Architecture_Guide.md).

---

## Quick Start

### 1. Modern DOM Parsing, Ranges, and Monadic Lookups

```cpp
#include <XML_Lib/XML.hpp>
#include <XML_Lib/XML_Node.hpp>
#include <XML_Lib/XML_Ranges.hpp>
#include <iostream>

using namespace XML_Lib;

int main() {
    XML xml{R"(
        <?xml version="1.0" encoding="UTF-8"?>
        <catalog>
            <book id="bk101">
                <author>Gambardella, Matthew</author>
                <title>XML Developer's Guide</title>
                <price>44.95</price>
            </book>
            <book id="bk102">
                <author>Ralls, Kim</author>
                <title>Midnight Rain</title>
                <price>5.95</price>
            </book>
        </catalog>
    )"};

    auto &root = NRef<Root>(xml.root());

    // C++20 Range view: iterate over only <book> child elements
    for (const auto &book : root.elements("book")) {
        const auto &bookElem = NRef<Element>(book);
        
        // C++23 Monadic optional lookup
        auto id = bookElem.findAttribute("id")
                          .transform([](const auto &attr) { return attr.get().getParsed(); })
                          .value_or("unknown");

        std::cout << "Book ID: " << id << "\n";
    }

    // C++23 Multidimensional indexing
    std::cout << "First book title: " << root["book", 0]["title"].getContents() << "\n";

    return 0;
}
```

### 2. $O(1)$-Memory Streaming Pull Parsing (`XMLReader`)

```cpp
#include <XML_Lib/XMLReader.hpp>
#include <iostream>

using namespace XML_Lib;

int main() {
    // Process large files without loading the entire DOM into memory
    auto reader = XMLReader::fromFile("large_dataset.xml");

    while (reader.read()) {
        if (reader.nodeType() == XMLReader::NodeType::ElementStart && reader.name() == "item") {
            auto category = reader.findAttribute("category").value_or("none");
            std::cout << "Found item in category: " << category << "\n";
        }
    }
    return 0;
}
```

### 3. Streaming XML Push Generation (`XMLWriter`)

```cpp
#include <XML_Lib/XMLWriter.hpp>
#include <iostream>

using namespace XML_Lib;

int main() {
    XMLWriter writer;
    writer.setIndent(true, 2);

    writer.writeStartDocument("1.0", "UTF-8");
    writer.writeStartElement("response");
    writer.writeAttribute("status", "success");

    writer.writeElement("message", "Processing completed successfully.");
    writer.writeComment("Generated automatically");

    writer.writeEndElement(); // </response>
    writer.writeEndDocument();

    std::cout << writer.result() << "\n";
    return 0;
}
```

### 4. Non-Throwing Parsing with `std::expected` (C++23)

```cpp
#include <XML_Lib/XML.hpp>
#include <iostream>

using namespace XML_Lib;

int main() {
    // Parse without exceptions
    auto result = XML::parseExpected("<root><data>42</data></root>");
    
    if (result) {
        std::unique_ptr<XML> xml = std::move(*result);
        std::cout << "Parsed root: " << xml->root().getContents() << "\n";
    } else {
        const XML_Error &err = result.error();
        std::cerr << "Parse error at line " << err.line 
                  << ", col " << err.column << ": " << err.message << "\n";
    }
    return 0;
}
```

### 5. Evaluating XPath Queries with Pre-Compiled ASTs

```cpp
#include <XML_Lib/XML.hpp>
#include <XML_Lib/XPath.hpp>
#include <iostream>

using namespace XML_Lib;

int main() {
    XML xml{R"(
        <inventory>
            <item category="electronics" in_stock="true">Laptop</item>
            <item category="furniture" in_stock="false">Chair</item>
            <item category="electronics" in_stock="false">Camera</item>
        </inventory>
    )"};

    // Pre-compile XPath expression once; evaluate repeatedly across documents
    XPathExpression query{"//item[@category='electronics' and @in_stock='true']"};
    
    auto matches = xml.xpath(query);
    for (const auto *node : matches) {
        std::cout << "Matching item: " << node->getContents() << "\n";
    }
    return 0;
}
```

### 6. Pre-Compiled XSD Schema Validation

```cpp
#include <XML_Lib/XML.hpp>
#include <XML_Lib/XSD_Schema.hpp>
#include <iostream>

using namespace XML_Lib;

int main() {
    // Pre-compile schema once; reuse across worker threads
    XSD_Schema schema{R"(
        <xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema">
            <xs:element name="note">
                <xs:complexType>
                    <xs:sequence>
                        <xs:element name="to" type="xs:string"/>
                        <xs:element name="from" type="xs:string"/>
                    </xs:sequence>
                </xs:complexType>
            </xs:element>
        </xs:schema>
    )"};

    XML doc{"<note><to>Alice</to><from>Bob</from></note>"};
    
    auto validationResult = doc.validateExpected(schema);
    if (validationResult) {
        std::cout << "Document is valid!\n";
    } else {
        std::cerr << "Validation failed: " << validationResult.error() << "\n";
    }
    return 0;
}
```

---

## Integration Guide

### Using CMake `find_package`

Once installed, consuming XML_Lib in your `CMakeLists.txt` is seamless:

```cmake
cmake_minimum_required(VERSION 3.20)
project(MyApplication CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(XML_Lib REQUIRED)

add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE XML_Lib::XML_Lib)
```

### Using CMake `FetchContent`

```cmake
include(FetchContent)
FetchContent_Declare(
    XML_Lib
    GIT_REPOSITORY https://github.com/clockworkengineer/XML_Lib.git
    GIT_TAG        v1.4.0
)
FetchContent_MakeAvailable(XML_Lib)

target_link_libraries(my_app PRIVATE XML_Lib::XML_Lib)
```

### Using vcpkg

Add `xml-lib` to your project's `vcpkg.json`:

```json
{
  "dependencies": [
    "xml-lib"
  ]
}
```

Or install directly via CLI:
```bash
vcpkg install xml-lib
```

### Using Conan 2.x

Add `xml-lib/1.4.0` to your `conanfile.py` or `conanfile.txt`:

```ini
[requires]
xml-lib/1.4.0

[generators]
CMakeDeps
CMakeToolchain
```

---

## Building and Testing

### Requirements
- C++23 compliant compiler:
  - GCC ≥ 13
  - Clang ≥ 17
  - AppleClang ≥ 15
  - MSVC ≥ 19.38 (Visual Studio 2022 17.8+)
- CMake ≥ 3.20

### Using CMake Presets

```bash
# Configure and build Release
cmake --preset release
cmake --build --preset release

# Run unit tests
ctest --preset unit-tests

# Debug build with ASan + UBSan
cmake --preset asan-ubsan
cmake --build --preset asan-ubsan
ctest --preset asan-tests
```

### Manual Build Commands

```bash
# Configure
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -DXML_LIB_BUILD_TESTS=ON -DXML_LIB_BUILD_EXAMPLES=ON

# Build
cmake --build build -j$(nproc)

# Run full test suite
ctest --test-dir build --output-on-failure

# Run W3C compliance test harness
./build/tests/XML_Lib_Unit_Tests -c "[Compliance]"

# Run performance benchmarks
./build/tests/XML_Lib_Performance_Tests
```

### Official W3C XML Conformance Test Suite

`XML_Lib` integrates the official [W3C XML Conformance Test Suite](https://www.w3.org/XML/Test/) (`xmlconf`) directly into its Catch2 test runner:
- If the test suite files are not present on disk, the test case gracefully skips without failing.
- If present, the test harness parses the `xmlconf.xml` catalog, executes tests across valid and non-well-formed fixtures, and strictly fails if any official test produces an unexpected outcome.

To download the official test suite and run it:
```bash
# Download W3C test suite files into tests/files/xmlconf
./scripts/Download-W3C-Suite.sh
# Or via CMake:
cmake --build build --target download_w3c_xmlconf

# Run official conformance tests
./build/tests/XML_Lib_Unit_Tests "Official W3C XML Conformance Test Suite"
```

### Installation

```bash
# Install to default system prefix (/usr/local)
sudo cmake --install build

# Or install to custom directory
cmake --install build --prefix /opt/xml_lib
```

---

## Documentation

- [User Guide](docs/Guide.md) — Comprehensive guide covering parsing, tree navigation, validation, and serialization.
- [API Reference](docs/API.md) — Exhaustive class and method reference for all components.
- [Streaming Processing Guide](docs/Streaming_Guide.md) — Dedicated guide for constant-memory ($O(1)$) pull parsing and push generation.
- [Modern C++ Guide](docs/Modern_Cpp_Guide.md) — In-depth guide to C++23 `std::expected`, monadic operations, ranges, and concepts.
- [Security & Catalog Resolution](docs/Entity_Catalog_Resolution.md) — Hardening against XXE and using OASIS XML Catalogs 1.1 for offline resolution.
- [Performance Tuning Guide](docs/Performance_Tuning_Guide.md) — PMR arenas, memory-mapped I/O (`MMapSource`), and pre-compilation optimization.
- [SOLID Architecture Guide](docs/SOLID_Architecture_Guide.md) — Architectural overview, design principles, and extension cookbooks.
- [W3C Conformance Documentation](docs/Conformance.md) — Official W3C XML Conformance Test Suite verification, pass rates, and standards breakdown.
- [Standards Compliance Report](docs/XML_Lib_Standards_Report.md) — Detailed breakdown of XML 1.0, DTD, XSD, XPath 1.0, and OASIS Catalogs 1.1.
- [Changelog](CHANGELOG.md) — Release and version history.
- [Contributing Guidelines](CONTRIBUTING.md) — Guidelines for submitting issues and pull requests.

---

## License

XML_Lib is licensed under the [MIT License](LICENSE.txt).
