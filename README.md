# XML_Lib

[![CI](https://github.com/clockworkengineer/XML_Lib/actions/workflows/ci.yml/badge.svg)](https://github.com/clockworkengineer/XML_Lib/actions/workflows/ci.yml)
[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE.txt)
[![Buy Me a Coffee](https://img.shields.io/badge/Donate-Buy%20Me%20A%20Coffee-orange.svg)](https://www.buymeacoffee.com/clockworkengineer)

**XML_Lib** is an enterprise-grade, high-performance C++20 library for parsing, querying, validating, manipulating, and serialising XML documents. Engineered with a strict **SOLID** architecture, modern C++ idioms, and zero external runtime dependencies, it provides an intuitive, robust API designed for both modern desktop/server applications and resource-constrained embedded environments.

---

## Key Highlights

- **Complete XML 1.0 & Namespaces**: Full conformance to W3C XML 1.0 (Fifth Edition) and XML Namespaces 1.0 specifications, including comments, CDATA, processing instructions, and qualified names (QNames).
- **Dual Validation Engines (DTD & XSD)**:
  - **DTD Validation**: Supports internal and external DTD subsets, element content models (`EMPTY`, `ANY`, mixed, sequence/choice), and attribute declarations (`ID`, `IDREF`, `NMTOKEN`, `ENTITY`, `NOTATION`).
  - **W3C XML Schema (XSD) Validation**: Validates documents against XSD schemas (`xs:sequence`, `xs:choice`, `xs:all`, `xs:any`, `xs:anyAttribute`, identity constraints `xs:key`/`xs:keyref`/`xs:unique`, schema composition via `xs:include`/`xs:import`, built-in types, and facets).
- **XPath 1.0 Query Engine**: Evaluates complex XPath expressions across the document tree with support for all 13 axes, 28+ built-in functions, predicates, and abbreviated syntax (`//`, `@`, `.`, `..`).
- **High-Performance Memory Model**: Uses `std::pmr` (Polymorphic Memory Resources) monotonic buffer arenas to minimize heap fragmentation and ensure high-throughput parsing.
- **Security by Design**: Hardened against Billion Laughs / quadratic blowup (XML bomb) recursion and external entity injection (XXE) attacks with strictly configurable resource limits (`ParseOptions`).
- **Embedded & Minimal Footprints**: Optional embedded configuration supporting `-fno-exceptions`, `-fno-rtti`, and dead-code stripping (`--gc-sections`).
- **Intuitive Modern C++ API**: Move semantics (Rule of Five), `std::string_view` zero-copy inputs, filesystem paths, and type-safe node downcasting (`isA<T>`, `NRef<T>`).

---

## 100% SOLID Architecture

XML_Lib is decoupled across dedicated role interfaces and services following the SOLID design principles:

| Subsystem | SOLID Principles | Key Abstractions & Interfaces | Architectural Benefits |
| :--- | :--- | :--- | :--- |
| **Input Stream I/O** | **ISP, DIP, SRP** | `ISource`, `BufferSource`, `FileSource` | Isolated stream normalization, BOM detection, UTF conversion |
| **Output Destination I/O**| **ISP, SRP** | `IDestination`, `BufferDestination`, `FileDestination` | Role-segregated serialization targets with zero name lookup clashes |
| **Entity & Security** | **ISP, SRP, DIP** | `IEntityRegistry`, `IEntityExpander`, `ISecurityPolicyManager` | Modular entity expansion, recursion cycle protection, and XXE defense |
| **Parser & Serializer** | **LSP, OCP, DIP** | `IParser`, `IStringify`, `INodeSerializer`, `IXMLParseStage` | Pluggable parsing stages and strategy-based formatting maps |
| **Validator Pipeline** | **OCP, DIP, SRP** | `ISchemaParser`, `ISchemaValidator`, `IValidatorRegistry` | Pluggable validation pipeline for DTD and XSD schemas |
| **Visitor & Traversal** | **ISP, OCP** | `IVisitorRoles`, `NodeVisitorAdapter`, `IAction` | Granular role visitor callbacks (`IElementVisitor`, `ICommentVisitor`) |
| **XPath & File Service**| **SRP, DIP, OCP** | `IXPathNodeAdapter`, `XPath`, `XML_FileIO` | Decoupled XPath node navigation and isolated static file I/O |

For comprehensive architectural documentation, see the [SOLID Architecture Guide](docs/SOLID_Architecture_Guide.md).

---

## Quick Start

### 1. Parsing and Accessing Elements

```cpp
#include <XML_Lib/XML.hpp>
#include <XML_Lib/XML_Node.hpp>
#include <iostream>

using namespace XML_Lib;

int main() {
    XML xml;
    xml.parse(R"(
        <?xml version="1.0" encoding="UTF-8"?>
        <catalog>
            <book id="bk101">
                <author>Gambardella, Matthew</author>
                <title>XML Developer's Guide</title>
                <price>44.95</price>
            </book>
        </catalog>
    )");

    auto &root = NRef<Root>(xml.root());
    std::cout << "Root element: " << root.name() << "\n";

    for (const auto &child : root.getChildren()) {
        if (isA<Element>(child)) {
            const auto &book = NRef<Element>(child);
            std::cout << "Book ID: " << book["id"].getParsed() << "\n";
        }
    }
    return 0;
}
```

### 2. Evaluating XPath Queries

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

    // Evaluate XPath expression
    auto matches = xml.xpath("//item[@category='electronics' and @in_stock='true']");
    for (const auto *node : matches) {
        std::cout << "Match: " << node->getContents() << "\n";
    }

    // Direct XPath evaluator
    XPath evaluator(xml.root());
    std::cout << "Electronics count: "
              << evaluator.evaluateNumber("count(//item[@category='electronics'])") << "\n";
    return 0;
}
```

### 3. Validating Against an XSD Schema

```cpp
#include <XML_Lib/XML.hpp>
#include <iostream>

using namespace XML_Lib;

int main() {
    XML xml{"<note><to>Tove</to><from>Jani</from><body>Don't forget!</body></note>"};

    const std::string_view schema = R"(
        <xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema">
            <xs:element name="note">
                <xs:complexType>
                    <xs:sequence>
                        <xs:element name="to" type="xs:string"/>
                        <xs:element name="from" type="xs:string"/>
                        <xs:element name="body" type="xs:string"/>
                    </xs:sequence>
                </xs:complexType>
            </xs:element>
        </xs:schema>
    )";

    try {
        xml.validate(schema);
        std::cout << "Document is valid against schema!\n";
    } catch (const std::exception &ex) {
        std::cerr << "Validation failed: " << ex.what() << "\n";
    }
    return 0;
}
```

### 4. Move Semantics

```cpp
XML createDocument() {
    XML doc{"<response status=\"ok\"/>"};
    return doc; // Efficiently moved (no copying)
}

int main() {
    XML doc = createDocument(); // Move-constructed
    XML target;
    target = std::move(doc);    // Move-assigned
}
```

---

## Integration Guide

### Using CMake `find_package`

Once installed, consuming XML_Lib in your `CMakeLists.txt` is seamless:

```cmake
cmake_minimum_required(VERSION 3.20)
project(MyApplication CXX)

set(CMAKE_CXX_STANDARD 20)
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
    GIT_TAG        v1.2.0
)
FetchContent_MakeAvailable(XML_Lib)

target_link_libraries(my_app PRIVATE XML_Lib::XML_Lib)
```

---

## Building and Testing

### Requirements
- C++20 compliant compiler:
  - GCC ≥ 11
  - Clang ≥ 14
  - AppleClang ≥ 14
  - MSVC ≥ 19.29 (Visual Studio 2019 16.10+)
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
- [API Reference](docs/API.md) — Complete class and method API documentation.
- [SOLID Architecture Guide](docs/SOLID_Architecture_Guide.md) — Architectural overview and design principles.
- [Standards Compliance Report](docs/XML_Lib_Standards_Report.md) — Detailed breakdown of XML 1.0, DTD, XSD, and XPath 1.0 compliance.
- [Changelog](CHANGELOG.md) — Detailed release and version history.
- [Contributing Guidelines](CONTRIBUTING.md) — Guidelines for submitting issues and pull requests.

---

## License

XML_Lib is licensed under the [MIT License](LICENSE.txt).
