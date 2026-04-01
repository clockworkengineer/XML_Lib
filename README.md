# XML_Lib

## Overview
**XML_Lib** is a C++20-based library designed to simplify the parsing, manipulation, and generation of XML documents. It is intended for developers who need an efficient, lightweight, and feature-rich solution for working with XML in their projects. The library provides a clean interface and conforms to modern C++ coding practices to ensure high performance and reliability.

## Features
- **XML Parsing**: Fully compliant with XML 1.0 standard, including strict validation of elements, attributes, entities, whitespace, and declaration.
- **XML Serialization**: Serialize data structures to XML format.
- **XML Namespaces**: Full W3C XML Namespace support — `xmlns` declarations, prefix scoping, QName parsing (`getPrefix()`, `getLocalName()`, `getNamespaceURI()`), and well-formedness enforcement.
- **DTD Validation**: Internal and external DTD parsing and validation.
- **XSD Validation**: Phase 1 W3C XML Schema (XSD) validation via `XML::validate(xsdSource)` — element content models (sequence/choice/all), attribute constraints (required/optional/prohibited/fixed), all builtin simple types, and all standard restriction facets.
- **Memory Efficiency**: Optimized algorithms to minimize memory usage.
- **Error Handling**: Robust error and exception handling mechanisms, with detailed standard-compliant error messages.
- **Cross-platform Compatibility**: Designed to run seamlessly on Linux, Windows, and macOS environments.
- **C++20 Support**: Full compliance with the C++20 standard and features such as ranges and smart pointers.

## Installation

### Prerequisites
- A C++20-compatible compiler (such as GCC, Clang, or MSVC)
- [CMake](https://cmake.org/) (minimum version 3.15)
- Optional: Git for cloning the repository.

### Build Instructions
1. Clone the repository:
```shell script
git clone https://github.com/your_user/XML_Lib.git
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

### Standard Compliance
XML_Lib is fully compliant with XML 1.0, including:
- Strict validation of tag and attribute names
- Expanded encoding support (UTF-8, UTF-16, ISO-8859, etc.)
- Robust entity handling
- Whitespace normalization
- Detailed error reporting for all non-compliant cases
- **W3C XML Namespaces** — `xmlns`/`xmlns:prefix` declarations, prefix scoping, QName decomposition (`getPrefix()`, `getLocalName()`, `getNamespaceURI()`), and attribute prefix validation
- **XSD Schema Validation** (Phase 1) — `XML::validate(xsdSource)` validates element structure, attribute constraints, and simple type restrictions against a W3C XML Schema
Unit tests cover all standard and edge cases.

### Integration with your Project
If you're using CMake, you can integrate **XML_Lib** with your project by including the library in your `CMakeLists.txt`:
add_subdirectory(XML_Lib)
target_link_libraries(your_project_name PRIVATE XML_Lib)
```

## Documentation
Detailed documentation and API references can be found in the `docs/` directory or on the [online documentation website](https://your_documentation_url.com).

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
If you encounter issues or have questions, please feel free to:
- Submit an issue on the GitHub repository.
- Reach out to the maintainer via email: `support@your_email.com`.

## Acknowledgements
Special thanks to contributors and testers who helped refine and improve **XML_Lib**.