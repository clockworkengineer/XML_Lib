# XML_Lib Concrete Refactor Plan

**Date:** 2026-05-19
**Source:** library code + `notes/attributes.md`

This refactor plan maps the 10 high-quality library attributes from `notes/attributes.md` to concrete implementation tasks in the XML_Lib codebase. Each attribute includes current-state observations, actionable tasks, affected files, and acceptance criteria.

---

## Executive Summary

The library already contains strong XML parsing, XSD/DTD support, and XPath capabilities. The refactor plan focuses on closing gaps in API usability, reliability, security, portability, documentation, and maintainability.

Priority order:
1. Security
2. Reliability
3. API usability
4. Testability
5. Portability
6. Performance
7. Documentation
8. Maintainability

---

## Attribute 1 — Intuitive API Design

**Current state:** Public API is partially clean, but convenience overloads, public header coverage, and user-facing documentation are inconsistent. Internal APIs like `NRef<T>` are reachable only through implementation headers.

### Tasks

1. Add public overloads to `XML` for string/file parse and stringify.
   - Files: `classes/include/XML.hpp`, `classes/source/XML.cpp`
   - New signatures:
     - `void parse(const std::string_view &xmlString) const;`
     - `void parse(const std::filesystem::path &filePath) const;`
     - `[[nodiscard]] std::string stringify() const;`
     - `void stringify(const std::filesystem::path &filePath, Format format = Format::utf8) const;`
   - Implementation delegates to existing source/destination classes.

2. Publish node cast helpers in a public header.
   - New file: `classes/include/XML_Node.hpp`
   - Re-export `isA<T>()` and `NRef<T>()` to eliminate the need for implementation includes.

3. Expose source and destination helpers in public include paths.
   - Files: `classes/include/` and `CMakeLists.txt`
   - Ensure `XML_Sources.hpp` and `XML_Destinations.hpp` are includable from the public API.

4. Fix swapped `Self` / `Root` error messages.
   - File: `classes/include/implementation/node/XML_Node_Reference.hpp`
   - Acceptance criteria: error text matches the node type.

### Acceptance criteria
- Users can write a basic parse/visit/stringify example with only public headers.
- `NRef<Element>(xml.root())` compiles using a public header only.
- Error messages for root/self casts are correct.

---

## Attribute 2 — Comprehensive Documentation

**Current state:** `Guide.md` documents a different API; `README.md` claims C++20 while the CMake tree targets C++23. `API.md` covers public signatures but lacks usage narratives and dependency guidance.

### Tasks

1. Rewrite `docs/Guide.md` to match the real API.
   - Include parse/stringify examples, attribute access, XPath examples, DTD/XSD validation, and error handling.

2. Fix README inconsistencies.
   - File: `README.md`
   - Update C++ standard text, repository references, and example section links.

3. Add Doxygen config and docs build support.
   - New file: `docs/Doxyfile`
   - CMake target: `docs`
   - Add public API documentation comments for key headers.

4. Add a dependency and security section to README.
   - Document runtime and optional build/test dependencies.

### Acceptance criteria
- `cmake --build . --target docs` produces HTML documentation.
- `README.md` correctly states the compiler requirements.
- Example code compiles when copied into a `.cpp` file.

---

## Attribute 3 — High Reliability

**Current state:** Error handling has a misleading no-op path in the exception-enabled branch. Bare `throw` usage may bypass embedded/no-exception handling. Some internal error messages are inconsistent.

### Tasks

1. Remove misleading `std::terminate()` path in exception-enabled `xml_lib_throw`.
   - File: `classes/include/implementation/common/XML_Error.hpp`

2. Audit and normalize exception usage.
   - Replace bare `throw` with `XML_LIB_THROW(...)` in library code where appropriate.

3. Add regression tests for node type cast error messages.
   - Files: `tests/source/node/...`

### Acceptance criteria
- No bare `throw` statements remain in public-facing library code where `XML_LIB_THROW` should be used.
- Existing tests still pass.
- Error messages are consistent.

---

## Attribute 4 — Performance and Efficiency

**Current state:** The library uses an arena allocator, but element attribute storage and namespace storage are on the heap. The arena has a small hard-coded default size and UTF-8 conversion may be repeated.

### Tasks

1. Convert attribute and namespace storage in node types to PMR containers.
   - File: `classes/include/implementation/variants/XML_Element.hpp`
   - Change `std::vector<XMLAttribute>` to `std::pmr::vector<XMLAttribute>`.

2. Make the arena initial size configurable.
   - Files: `classes/include/implementation/common/XML_Arena.hpp`, `classes/include/implementation/XML_Impl.hpp`, `CMakeLists.txt`
   - Add a CMake option `XML_LIB_ARENA_SIZE_KB` defaulting to 256.

3. Cache UTF-8 names or attribute text when repeated conversions occur.
   - File: `classes/include/implementation/variants/XML_Element.hpp`

4. Document arena sizing tradeoffs in `docs/Guide.md`.

### Acceptance criteria
- Attribute storage uses the library's PMR memory resource.
- Parsing a large document with `XML_LIB_ARENA_SIZE_KB=1024` avoids fallback allocations.
- Performance observations are recorded in the guide.

---

## Attribute 5 — Maintainability

**Current state:** Utility methods are embedded in interface headers; macros hide visibility; the header structure is inconsistent and creates coupling.

### Tasks

1. Extract utility methods out of `ISource` into implementation helpers.
   - New file: `classes/include/implementation/common/XML_SourceHelpers.hpp`

2. Replace `IACTION_EVENT` macro with explicit virtual methods.
   - File: `classes/include/interface/IAction.hpp`

3. Reduce the scope of `XML_Core.hpp`.
   - File: `classes/include/implementation/XML_Core.hpp`
   - Make it an internal aggregate only.

4. Consider renaming `implementation/variants/` to `implementation/nodes/` or adjust folder naming for consistency.

### Acceptance criteria
- Implementation helpers are no longer mixed into public interface headers.
- IDE navigation works cleanly through action interface methods.
- The internal header graph is simpler.

---

## Attribute 6 — Flexibility and Customization

**Current state:** The parser has no runtime parse-limit configuration and external entity resolution is all-or-nothing.

### Tasks

1. Add a `ParseOptions` struct to configure parser behavior.
   - File: `classes/include/XML.hpp`
   - Fields: `maxEntityExpansionDepth`, `maxNestingDepth`, `maxAttributeCount`, `allowExternalEntities`, `entityResolver`.

2. Pass `ParseOptions` through `XML_Impl` and parser interfaces.
   - File: `classes/include/interface/IParser.hpp`

3. Add an `IEntityResolver` hook.
   - New file: `classes/include/interface/IEntityResolver.hpp`

4. Keep existing behavior unchanged for default options.

### Acceptance criteria
- Default `ParseOptions` preserves current parsing semantics.
- Custom options enable secure and flexible parsing.
- External entity access is controllable.

---

## Attribute 7 — Strong Security

**Current state:** No limits on entity expansion, nesting depth, attribute count, or external entity resolution. File path handling is permissive.

### Tasks

1. Enforce an entity expansion depth limit.
   - File: parser/entity resolution implementation.
   - Default limit: 512.

2. Enforce maximum element nesting depth.
   - File: `classes/source/implementation/xml/parser/Default_Parser.cpp`
   - Default limit: 1000.

3. Deny external entities by default.
   - File: DTD parser/entity resolution code.
   - Provide `ParseOptions::allowExternalEntities` and optional `IEntityResolver`.

4. Validate file paths passed to `fromFile` / `toFile`.
   - File: `classes/source/implementation/xml/file/XML_File.cpp`
   - Reject null bytes and clearly unsafe paths.

5. Enforce maximum attribute count.
   - File: `classes/source/implementation/xml/parser/Default_Parser.cpp`
   - Default limit: 10 000.

### Acceptance criteria
- Crafted security attack inputs fail safely with `SyntaxError`.
- External entity loading is blocked by default.
- Path validation errors are explicit.

---

## Attribute 8 — High Testability

**Current state:** Tests are present, but they depend on internal implementation headers and no embedded/no-exception test target exists.

### Tasks

1. Add mock helpers for interfaces used in tests.
   - Files: `tests/include/mocks/Mock_ISource.hpp`, `Mock_IAction.hpp`, `Mock_IParser.hpp`

2. Prune internal include dependencies from tests.
   - File: `tests/CMakeLists.txt`
   - Keep only public headers plus local test utilities.

3. Add an embedded/no-exception test target.
   - File: `tests/CMakeLists.txt`
   - Verify `std::terminate()` behavior.

4. Add security-oriented regression tests.
   - New file: `tests/source/xml/XML_Lib_Tests_Security.cpp`

### Acceptance criteria
- Tests can be compiled against public headers only.
- Security limits are covered.
- CI can run both normal and embedded builds.

---

## Attribute 9 — Compatibility and Portability

**Current state:** Platform detection and sample build metadata are incomplete. No install target exists. Windows precompiled-header artifacts remain in the tree.

### Tasks

1. Fix platform selection to explicitly support macOS.
   - File: `CMakeLists.txt`

2. Add a proper CMake install target and package config.
   - File: `CMakeLists.txt`

3. Remove or relocate `stdafx.h`.
   - File: `stdafx.h` or move into a `windows/` directory.

4. Validate or lower the compiler standard requirement.
   - File: `CMakeLists.txt`, `README.md`

### Acceptance criteria
- macOS builds succeed.
- `cmake --install` creates a usable install tree.
- A downstream project can `find_package(XML_Lib)`.

---

## Attribute 10 — Low Dependency Footprint

**Current state:** Runtime dependency footprint is already very low, but the test build currently fetches Catch2 during configure.

### Tasks

1. Prefer local Catch2 installations before using `FetchContent`.
   - File: `tests/CMakeLists.txt`

2. Add dependency documentation to `README.md`.

3. Add a `std::pmr` availability guard in CMake.
   - File: `CMakeLists.txt`

### Acceptance criteria
- Build works offline when Catch2 is available locally.
- The runtime library has no third-party dependencies beyond the standard library.
- Toolchain compatibility is explicit.

---

## Recommended Implementation Sequence

1. Security: entity depth, nesting depth, external entity denial, attribute count, path validation.
2. Reliability: exception path cleanup, error message fixes.
3. API usability: public overloads, public node header, source/destination exposure.
4. Flexibility: `ParseOptions`, `IEntityResolver`.
5. Testability: mocks, public-only test includes, embedded tests.
6. Portability: macOS support, install target, std::pmr guard.
7. Performance: PMR containers, configurable arena, caching.
8. Documentation: guide rewrite, README updates, Doxygen docs.
9. Maintainability: header cleanup, macro removal, folder consistency.

---

## File Change Summary

| File | Change | Attributes |
|---|---|---|
| `classes/include/XML.hpp` | Add convenience overloads, `ParseOptions`, filesystem API | 1, 6, 7 |
| `classes/source/XML.cpp` | Implement overloads | 1, 6 |
| `classes/include/XML_Node.hpp` | New public node helper header | 1 |
| `classes/include/implementation/node/XML_Node_Reference.hpp` | Fix error text | 1, 3 |
| `classes/include/implementation/common/XML_Error.hpp` | Clean exception path | 3 |
| `classes/include/interface/IParser.hpp` | Accept `ParseOptions` | 6 |
| `classes/include/interface/IEntityResolver.hpp` | New optional entity resolver | 6, 7 |
| `classes/include/implementation/variants/XML_Element.hpp` | PMR vector storage | 4 |
| `classes/include/implementation/common/XML_Arena.hpp` | Configurable arena size | 4 |
| `classes/source/implementation/xml/parser/Default_Parser.cpp` | Attribute count and nesting limits | 7 |
| `classes/source/implementation/xml/file/XML_File.cpp` | Path validation | 7 |
| `tests/CMakeLists.txt` | Public-only test includes; embedded target | 8, 9 |
| `tests/include/mocks/` | New test mocks | 8 |
| `CMakeLists.txt` | Platform support; install target; PMR guard | 9, 10 |
| `README.md` | Document C++ standard, dependencies, security | 2, 7, 10 |
| `docs/Guide.md` | Rewrite user guide | 2 |
| `docs/Doxyfile` | Add docs generation | 2 |

---

## Notes

- The existing `docs/refactor_plan.md` already contains a related plan, but this file provides a fresh, concrete roadmap based on the current code and the 10 attributes from `notes/attributes.md`.
- Implementation tasks are intentionally scoped so they can be executed incrementally and validated with targeted tests.
