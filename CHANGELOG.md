# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.3.0] - 2026-09-23

### Added
- **Component Factories (DIP)**: Added `XML_Factories.hpp` / `XML_Factories.cpp` providing decoupled factory functions (`createDefaultParser`, `createDefaultStringify`, `createDefaultEntityMapper`, `createDefaultValidatorRegistry`, `createDefaultXPathEngine`).
- **Pluggable Schema Validator Registry (OCP)**: Integrated `ValidatorRegistry` into `XML` facade (`registerValidator`, `validate(schemaType, schemaSource)`).
- **Extensible XPath Engine (OCP)**: Introduced `IXPathEngine` query engine interface and `XML::setXPathEngine` hook allowing custom query engines.
- **Dedicated Namespace Validator (SRP)**: Extracted `NamespaceValidator` component encapsulating W3C XML Namespaces 1.0 rules.
- **LSP Contract Purification**: Purified `IParser` and `IStringify` base contracts by removing dummy stub implementations; introduced `IValidatingParser` and `IIndentedStringify` role interfaces.

### Refactored
- **DRY Consolidation**: Consolidated file I/O operations (`XML_FileIO`), QName handling (`XML_QName`), character reference and quoted string decoders (`XML_ParseHelpers`), and node variant checks (`XML_NodeKindHelpers`).

## [1.2.0] - 2026-09-20

### Added
- **Move Semantics for `XML`**: `XML` is now fully move-constructible and move-assignable (`XML(XML&&) noexcept`, `operator=(XML&&) noexcept`).
- **Safe Custom Parser/Stringifier Overload**: `XML(std::unique_ptr<IStringify>, std::unique_ptr<IParser>)` for explicit ownership transfer.
- **W3C XML Schema (XSD) Validation**: Full Phase 2 validation engine (`XSD_Validator`, `XML::validate(xsdSource)`) supporting `xs:sequence`, `xs:choice`, `xs:all`, `xs:any`, `xs:anyAttribute`, identity constraints (`xs:key`, `xs:keyref`, `xs:unique`), schema composition (`xs:include`, `xs:import`), restriction facets, and built-in datatypes.
- **XPath 1.0 Query Engine**: Complete XPath 1.0 engine (`XPath` class and `xml.xpath(expression)` convenience method) implementing all 13 axes, 28+ core functions, predicates, abbreviated syntax (`//`, `@`, `.`, `..`), and union expressions.
- **PMR Arena Allocation**: Embedded polymorphic memory resources (`std::pmr`) for ultra-low overhead node and token allocations governed by `XML_LIB_ARENA_SIZE_KB`.
- **CMake Presets**: `CMakePresets.json` providing standardized presets for `release`, `debug`, `asan-ubsan`, and `embedded`.
- **Sanitizer Support**: Built-in `-DXML_LIB_ENABLE_ASAN=ON` and `-DXML_LIB_ENABLE_UBSAN=ON` options.
- **CPack Packaging**: Automated generator for `.tar.gz` and `.zip` distribution packages.
- **Cross-Platform CI**: GitHub Actions workflow covering Linux (GCC & Clang), macOS (AppleClang), and Windows (MSVC).

### Fixed
- **Library Export Contract**: Fixed missing `$<INSTALL_INTERFACE:...>` directories and headers, ensuring downstream consumers using `find_package(XML_Lib)` can cleanly compile and link.
- **Root CTest Execution**: Added root-level `enable_testing()` so running `ctest` from the project root correctly runs all test targets.
- **Example Targets**: Fixed `.cpp` file extension leaking into executable binary names and removed unconditional binary installation to `/usr/local/bin`.
- **Comment Residuals**: Cleaned up historical copy-paste references in core source comments.

## [1.1.0] - 2026-08-15

### Added
- **DTD Validation Subsystem**: Validation for internal and external DTD definitions, ELEMENT content models, and ATTRIBUTE declarations.
- **Security Hardening**: Billion Laughs (XML bomb) recursion limits and default XXE rejection via configurable `ParseOptions`.
- **Namespaces**: W3C XML Namespace resolution, prefix scoping, and QName parsing.

## [1.0.0] - 2026-06-01

### Added
- Initial production release of XML_Lib for C++20.
- XML 1.0 compliant parser with DOM tree representation (`Node`, `Element`, `Root`, `Comment`, `CDATA`, `PI`).
- Buffer and file-based streaming I/O (`BufferSource`, `FileSource`, `BufferDestination`, `FileDestination`).
- Multiple character encoding and BOM support (UTF-8, UTF-16BE, UTF-16LE, UTF-32BE, UTF-32LE).
- Visitor traversal API (`IAction`).
