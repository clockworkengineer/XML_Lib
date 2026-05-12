# XML_Lib

## Overview
**XML_Lib** is a C++20 library designed to simplify the parsing, manipulation, and generation of XML documents. It is intended for developers who need an efficient, lightweight, and feature-rich solution for working with XML in their projects. The library provides a clean interface and conforms to modern C++ coding practices to ensure high performance and reliability.

## Features
- **XML Parsing**: Fully compliant with XML 1.0 standard, including strict validation of elements, attributes, entities, whitespace, and declaration.
- **XML Serialization**: Serialize data structures to XML format.
- **XML Namespaces**: Full W3C XML Namespace support — `xmlns` declarations, prefix scoping, QName parsing (`getPrefix()`, `getLocalName()`, `getNamespaceURI()`), and well-formedness enforcement.
- **DTD Validation**: Internal and external DTD parsing and validation.
- **XSD Validation**: Phase 1 W3C XML Schema (XSD) validation via `XML::validate(xsdSource)` — element content models (sequence/choice/all), attribute constraints (required/optional/prohibited/fixed), all builtin simple types, and all standard restriction facets.
- **XPath Queries**: Full XPath 1.0 evaluation via the `XPath` class and `xml.xpath(expr)` shorthand — all 13 axes, 28+ built-in functions, predicates, all result types (node-set, string, number, boolean), and abbreviated syntax (`//`, `.`, `..`, `@`).
- **Memory Efficiency**: Optimized algorithms to minimize memory usage.
- **Error Handling**: Robust error and exception handling mechanisms, with detailed standard-compliant error messages.
- **Cross-platform Compatibility**: Designed to run seamlessly on Linux, Windows, and macOS environments.
- **C++20 Standard**: Uses C++20 features (`std::ranges`, `std::string_view`, `starts_with`/`ends_with`) and C++17 `std::pmr` (Polymorphic Memory Resources) for efficient node allocation. No C++23 features are required.

## Installation

### Prerequisites
- A C++20-compatible compiler:
  - GCC ≥ 10
  - Clang ≥ 12
  - AppleClang ≥ 13 (Xcode 13+)
  - MSVC ≥ 19.29 (Visual Studio 2019 16.10+)
- [CMake](https://cmake.org/) (minimum version 3.20)
- Optional: Git for cloning the repository.

### Build Instructions
1. Clone the repository:
```shell script
git clone https://github.com/clockworkengineer/XML_Lib.git
   cd XML_Lib
```

2. Create a build directory and change into it:
```shell script
mkdir build
   cd build
```

3. Generate the build files using CMake:
```shell script
cmake ..
```

4. Compile the library:
```shell script
cmake --build .
```

5. (Optional) Run unit tests:
```shell script
ctest
```

## Usage
To use **XML_Lib** in your project:
1. Include the appropriate header files in your source code.
2. Link your project against the generated library.

### Example
```cpp
#include "XML.hpp"
using namespace XML_Lib;

int main() {
   XML xml;
   BufferSource source{"<?xml version=\"1.0\" encoding=\"UTF-8\"?><root><child>value</child></root>"};
   xml.parse(source);
   auto &root = NRef<Element>(xml.root());
   std::cout << "Root name: " << root.name() << std::endl;
   std::cout << "Child contents: " << root[0].getContents() << std::endl;
}
```

### XPath Example
```cpp
#include "XML.hpp"
#include "XPath.hpp"
using namespace XML_Lib;

int main() {
   XML xml;
   BufferSource source{R"(<?xml version="1.0"?>
   <bookstore>
     <book category="web"><title>Learning XML</title><price>39.95</price></book>
     <book category="cooking"><title>Everyday Italian</title><price>30.00</price></book>
   </bookstore>)"};
   xml.parse(source);

   // Select nodes
   auto books = xml.xpath("//book");
   std::cout << books.size() << " books found\n";  // 2

   // Filter by attribute
   auto webBooks = xml.xpath("//book[@category='web']");

   // Built-in functions
   XPath xp(xml.root());
   std::cout << xp.evaluateString("string(//title[1])") << "\n"; // Learning XML
   std::cout << xp.evaluateNumber("sum(//price)") << "\n";       // 69.95
   std::cout << xp.evaluateBool("count(//book) > 1") << "\n";   // 1 (true)
}
```

### Standard Compliance
XML_Lib is fully compliant with XML 1.0, including:
- Strict validation of tag and attribute names
- Expanded encoding support (UTF-8, UTF-16, ISO-8859, etc.)
- Robust entity handling
- Whitespace normalization
- Detailed error reporting for all non-compliant cases
- **W3C XML Namespaces** — `xmlns`/`xmlns:prefix` declarations, prefix scoping, QName decomposition (`getPrefix()`, `getLocalName()`, `getNamespaceURI()`), and attribute prefix validation
- **XSD Schema Validation** (Phase 1) — `XML::validate(xsdSource)` validates element structure, attribute constraints, and simple type restrictions against a W3C XML Schema
- **XPath 1.0** — `xml.xpath(expr)` / `XPath::evaluate(expr)` supports all 13 axes, 28+ functions, predicates, and all result types

Unit tests cover all standard and edge cases.

### Integration with your Project
If you're using CMake, you can integrate **XML_Lib** with your project by including the library in your `CMakeLists.txt`:
add_subdirectory(XML_Lib)
target_link_libraries(your_project_name PRIVATE XML_Lib)
```

## Documentation
Detailed documentation and API references can be found in the `docs/` directory:
- [Guide.md](docs/Guide.md) — step-by-step user guide
- [API.md](docs/API.md) — full API reference

## Examples

Complete, runnable example programs are in [`examples/source/`](examples/source/):

| File | Description |
|---|---|
| [XML_Analyze_File.cpp](examples/source/XML_Analyze_File.cpp) | Parse every XML file in a directory and produce an analysis report of each document tree. |
| [XML_Create_At_Runtime.cpp](examples/source/XML_Create_At_Runtime.cpp) | Build an XML document programmatically at runtime using the `XML` class, index operators, and initialiser lists. |
| [XML_DTD_Validation_Example.cpp](examples/source/XML_DTD_Validation_Example.cpp) | Parse and validate XML with internal and external DTDs; shows error reporting for DTD violations. |
| [XML_Deep_Nesting_Example.cpp](examples/source/XML_Deep_Nesting_Example.cpp) | Parse and traverse deeply nested XML documents. |
| [XML_Encoding_Example.cpp](examples/source/XML_Encoding_Example.cpp) | Parse XML with different encodings (UTF-8, UTF-16, UTF-32) and BOM handling. |
| [XML_Entity_Reference_Example.cpp](examples/source/XML_Entity_Reference_Example.cpp) | Parse XML with predefined and custom entity references, including error handling for undefined entities. |
| [XML_Large_Attributes_Example.cpp](examples/source/XML_Large_Attributes_Example.cpp) | Parse XML with a large number of attributes per element. |
| [XML_Malformed_Example.cpp](examples/source/XML_Malformed_Example.cpp) | Handle and report malformed XML input gracefully. |
| [XML_Mixed_Content_Example.cpp](examples/source/XML_Mixed_Content_Example.cpp) | Parse and validate XML with mixed content (elements containing both text and child elements). |
| [XML_Parse_File.cpp](examples/source/XML_Parse_File.cpp) | Parse each XML file in a directory, stringify it back to text, and re-parse the buffer; includes timing. |
| [XML_Whitespace_Example.cpp](examples/source/XML_Whitespace_Example.cpp) | Demonstrate whitespace handling and formatting edge cases. |
| [XML_XPath_Basic.cpp](examples/source/XML_XPath_Basic.cpp) | XPath 1.0 navigation: absolute paths, `//` descendant shorthand, `.`/`..`, wildcards, and attribute axes. |
| [XML_XPath_Functions.cpp](examples/source/XML_XPath_Functions.cpp) | XPath 1.0 built-in functions: `count`, `last`, `position`, `string`, `concat`, `contains`, `substring`, and more. |
| [XML_XPath_Predicates.cpp](examples/source/XML_XPath_Predicates.cpp) | XPath 1.0 predicate filtering: positional predicates, attribute value tests, and child element value tests. |
| [XML_XSD_Attributes_Example.cpp](examples/source/XML_XSD_Attributes_Example.cpp) | XSD attribute validation: required, optional, prohibited, fixed-value, and `anyAttribute` wildcard declarations. |
| [XML_XSD_Basic_Validation.cpp](examples/source/XML_XSD_Basic_Validation.cpp) | Basic XSD schema validation against an inline schema; shows how to catch and report schema violations. |
| [XML_XSD_File_Validation.cpp](examples/source/XML_XSD_File_Validation.cpp) | Load both XML and XSD from files and validate the document against the schema. |
| [XML_XSD_Type_Restrictions.cpp](examples/source/XML_XSD_Type_Restrictions.cpp) | XSD simple type restriction facets: `minInclusive`/`maxInclusive`, `pattern`, `enumeration`, `minLength`/`maxLength`. |
| [XML__Fibonacci.cpp](examples/source/XML__Fibonacci.cpp) | Append the next Fibonacci number to an XML array file on each run; creates the file with `[0,1]` if it does not exist. |

To build the examples, configure CMake with `-DXML_LIB_BUILD_EXAMPLES=ON`:

```shell script
cmake -S . -B build -DXML_LIB_BUILD_EXAMPLES=ON
cmake --build build
```

## Contributing
Contributions are welcome! If you would like to contribute:
1. Fork the repository.
2. Create a new branch for your feature or bugfix: `git checkout -b feature/your_feature_name`.
3. Commit your changes.
4. Push your branch and create a pull request with a detailed explanation of your changes.

## License
This project is licensed under the [MIT License](./LICENSE). Feel free to use, modify, and distribute the library as per the terms of the license.

---

## Support
If you encounter issues or have questions, please submit an issue on the [GitHub repository](https://github.com/clockworkengineer/XML_Lib).

## Acknowledgements
Special thanks to contributors and testers who helped refine and improve **XML_Lib**.