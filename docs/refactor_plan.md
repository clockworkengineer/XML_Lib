# XML_Lib Refactor Plan

**Date:** 2026-05-11  
**Version baseline:** 1.2.0  
**Standard:** Implements all 10 attributes of a high-quality software library (see `notes/attributes.md`)

---

## Executive Summary

This plan maps each of the 10 library quality attributes to concrete, actionable refactor tasks with specific files, code locations, and acceptance criteria. Tasks are ordered by priority within each attribute; overall priority is security > reliability > API usability > documentation.

---

## Attribute 1 — Intuitive API Design

**Current state:** The `XML` class public interface is clean, but users need internal headers to cast `Node` objects (`isA<T>`, `NRef<T>`). Source/destination types live in implementation headers not exposed at the public include level. The `Guide.md` documents an entirely different, wrong API.

### Tasks

#### 1.1 — Add convenience parse/stringify overloads to `XML`

**File:** `classes/include/XML.hpp`, `classes/source/XML.cpp`

Add these signatures to `XML` so users never need `BufferSource` / `FileSource` for common cases:

```cpp
// Parse from a plain string
void parse(const std::string_view &xmlString) const;
// Parse from a file path
void parse(const std::filesystem::path &filePath) const;
// Stringify to a string (returns the XML text)
[[nodiscard]] std::string stringify() const;
// Stringify to a file path
void stringify(const std::filesystem::path &filePath, Format format = Format::utf8) const;
```

The implementations delegate to the existing `BufferSource` / `FileSource` / `BufferDestination` / `FileDestination` internally so the public `ISource`/`IDestination` overloads remain for advanced use.

#### 1.2 — Promote `Node` cast helpers to the public header

**File:** `classes/include/XML.hpp` (or a new `classes/include/XML_Node.hpp` public header)

`isA<T>()` and `NRef<T>()` are widely needed but buried in an implementation header. Provide a thin forwarding public header:

```cpp
// classes/include/XML_Node.hpp  (new public header)
#pragma once
#include "XML.hpp"
#include "implementation/node/XML_Node_Reference.hpp"
namespace XML_Lib {
  // Re-exports isA<T> and NRef<T> so users include only public headers.
  using XML_Lib::isA;
  using XML_Lib::NRef;
}
```

Add `XML_Node.hpp` to `install(FILES ...)` if a CMake install target is added (see §9).

#### 1.3 — Fix `Self` / `Root` error message swap

**File:** `classes/include/implementation/node/XML_Node_Reference.hpp`

Lines for `Root` and `Self` in `checkNodeType<T>` have their error messages transposed:

```cpp
// BUG: says "(self)" when checking Root, "(root)" when checking Self
} else if constexpr (std::is_same_v<T, Self>) {
    if (!isA<Self>(xNode)) { XML_LIB_THROW(Node::Error("Node not a (root) element.")); }   // wrong
} else if constexpr (std::is_same_v<T, Root>) {
    if (!isA<Root>(xNode)) { XML_LIB_THROW(Node::Error("Node not a (self) element.")); }   // wrong
```

Fix:

```cpp
} else if constexpr (std::is_same_v<T, Self>) {
    if (!isA<Self>(xNode)) { XML_LIB_THROW(Node::Error("Node is not a self-closing element.")); }
} else if constexpr (std::is_same_v<T, Root>) {
    if (!isA<Root>(xNode)) { XML_LIB_THROW(Node::Error("Node is not the root element.")); }
```

#### 1.4 — Expose `XML_Sources.hpp` / `XML_Destinations.hpp` in the public include path

**File:** `CMakeLists.txt` — `target_include_directories`

Currently `BufferSource`, `FileSource`, `BufferDestination`, `FileDestination` require users to add implementation include paths manually. Move these four lightweight headers to `classes/include/` (or symlink/include-forward) so they are reachable from the public include root.

**Acceptance criteria:**
- A user can write a complete parse-and-stringify program using only `#include "XML.hpp"` and one additional `#include "XML_Sources.hpp"` with no `-I` flags for internal directories.
- `NRef<Element>(xml.root())` compiles from a file that includes only `XML_Node.hpp`.

---

## Attribute 2 — Comprehensive Documentation

**Current state:** `Guide.md` documents a completely different API (`XMLDocument`, `XMLElement`, `doc.LoadFile()`). `README.md` states C++20 but `CMakeLists.txt` requires C++23. `API.md` is accurate but incomplete. No Doxygen configuration. Examples exist but are not linked from docs.

### Tasks

#### 2.1 — Rewrite `Guide.md` against the real API

**File:** `docs/Guide.md`

Replace the entire file. The new guide must:
- Show `#include "XML.hpp"` (not fictional headers).
- Demonstrate `XML xml; xml.parse(BufferSource{...});` and the new §1.1 convenience overloads.
- Cover: parse from string, parse from file, access root element, iterate children, read attributes, use `NRef<Element>`, stringify, error handling, DTD/XSD/XPath (gated on feature flags).
- Reference `API.md` for the full reference.

#### 2.2 — Fix C++ standard discrepancy in `README.md`

**File:** `README.md`

Change "C++20" to "C++23" throughout. Update the compiler prerequisites section:

```
- A C++23-compatible compiler (GCC ≥ 13, Clang ≥ 16, MSVC ≥ 19.35)
```

Also fix the placeholder GitHub URL (`https://github.com/your_user/XML_Lib.git`).

#### 2.3 — Add Doxygen support

**Files:** new `docs/Doxyfile`, `CMakeLists.txt`

Add a `Doxyfile` configured for the `classes/include/` tree and add a `docs` CMake target:

```cmake
find_package(Doxygen OPTIONAL_COMPONENTS dot)
if(DOXYGEN_FOUND)
  add_custom_target(docs
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating API documentation")
endif()
```

Add `///` Doxygen doc comments to all public class members in:
- `classes/include/XML.hpp`
- `classes/include/XPath.hpp`
- `classes/include/DTD_Validator.hpp`
- `classes/include/XSD_Validator.hpp`
- `classes/include/interface/ISource.hpp`
- `classes/include/interface/IDestination.hpp`
- `classes/include/interface/IAction.hpp`
- `classes/include/interface/IParser.hpp`
- `classes/include/interface/IStringify.hpp`

#### 2.4 — Link examples from `README.md`

**File:** `README.md`

Add a "Examples" section listing each source file in `examples/source/` with a one-line description, hyperlinked to the file.

**Acceptance criteria:**
- `Guide.md` compiles when its code snippets are extracted into a `.cpp` file.
- Running `cmake --build . --target docs` produces HTML output in `docs/html/`.
- `README.md` references C++23 and a real repository URL.

---

## Attribute 3 — High Reliability

**Current state:** The `xml_lib_throw` free function calls `std::terminate()` in both exception-enabled and no-exception paths — the exception-enabled path should not call terminate (the `XML_LIB_THROW` macro correctly throws, but the free function body is dead code that still terminates). Error messages for `Root` and `Self` are swapped (see §1.3).

### Tasks

#### 3.1 — Fix `xml_lib_throw` terminate in exception-enabled path

**File:** `classes/include/implementation/common/XML_Error.hpp`

Current (exception-enabled path):

```cpp
template<typename E> [[noreturn]] inline void xml_lib_throw(const E &error) { (void)error; std::terminate(); }
#define XML_LIB_THROW(exception) throw exception
```

The free function is never called when exceptions are enabled (`XML_LIB_THROW` expands to `throw`), but its existence with `std::terminate()` body is misleading and dangerous. Remove the exception-enabled version so it cannot be called accidentally:

```cpp
// Exception-enabled path: xml_lib_throw is intentionally not defined.
// Use the XML_LIB_THROW macro.
#define XML_LIB_THROW(exception) throw exception
```

The no-exception path keeps `xml_lib_throw` calling `std::terminate()` with a clear comment that this is intentional for embedded targets.

#### 3.2 — Add `XML_LIB_THROW` usage audit

**All `.cpp` files under `classes/source/`**

Grep for bare `throw` statements that bypass `XML_LIB_THROW`:

```sh
grep -rn "\bthrow\b" classes/source/
```

Replace each bare `throw SomeError(...)` with `XML_LIB_THROW(SomeError(...))` so the no-exception embedded build path is consistent.

#### 3.3 — Add regression tests for `Self`/`Root` error messages

**File:** `tests/source/node/XML_Lib_Tests_Self.cpp`, `tests/source/node/XML_Lib_Tests_Root.cpp`

After fixing §1.3, add a `SECTION` that checks the error message text when `NRef<Root>` is called on a `Self` node and vice-versa, preventing regression.

**Acceptance criteria:**
- All existing tests pass.
- No bare `throw` in `classes/source/` (except where exceptions genuinely must not be redirected).
- Error message strings are correct and covered by tests.

---

## Attribute 4 — Performance and Efficiency

**Current state:** PMR arena allocator is in place. `std::vector<XMLAttribute>` inside `Element` does not use PMR, causing heap allocations per element. U16string ↔ UTF-8 conversions happen on every attribute/name read. `XML_Arena` fixed initial size is 64 KB, which may be too small for large documents triggering fallback to the system allocator.

### Tasks

#### 4.1 — Convert `XMLAttribute` storage in `Element` to PMR

**File:** `classes/include/implementation/variants/XML_Element.hpp`, `XML_Root.hpp`, `XML_Self.hpp`

Change `std::vector<XMLAttribute>` to `std::pmr::vector<XMLAttribute>`:

```cpp
mutable std::pmr::vector<XMLAttribute> attributes;
mutable std::pmr::vector<XMLAttribute> namespaces;
```

Pass the memory resource from the `Variant` base to these vectors. This avoids per-element heap allocations for attribute lists.

#### 4.2 — Make the arena initial size configurable

**File:** `classes/include/implementation/common/XML_Arena.hpp`

The arena is hard-coded to 64 KB. Allow it to be set at construction:

```cpp
// Already correct; ensure XML_Impl passes a sensible default and that
// CMake exposes XML_LIB_ARENA_SIZE_KB as an option (default 256).
```

**File:** `CMakeLists.txt`

```cmake
option(XML_LIB_ARENA_SIZE_KB "Initial arena size in KB" 256)
target_compile_definitions(${XML_LIBRARY_NAME} PRIVATE
    XML_LIB_ARENA_SIZE_KB=${XML_LIB_ARENA_SIZE_KB})
```

**File:** `classes/include/implementation/XML_Impl.hpp`

```cpp
XML_Arena arena{ XML_LIB_ARENA_SIZE_KB * 1024 };
```

#### 4.3 — Cache UTF-8 names in `Element` to avoid repeated conversion

**File:** `classes/include/implementation/variants/XML_Element.hpp`

`name()` returns a `const std::string &` stored as `elementName` (already UTF-8). Verify that `getContents()` does not re-encode on every call. If it does, add a memoised `contentCache` field behind a `mutable` flag.

#### 4.4 — Profile and document performance characteristics

**File:** `docs/Guide.md` (performance section)

After §4.1–4.3, run the existing performance tests and record baseline numbers. Add a "Performance" section to the guide describing arena sizing guidelines and the trade-off between `XML_LIB_ARENA_SIZE_KB` and memory usage.

**Acceptance criteria:**
- Attribute list allocation no longer appears in a heap profiler for typical documents.
- Configuring `XML_LIB_ARENA_SIZE_KB=1024` and parsing a 1 MB document does not trigger any fallback allocations (verified with a custom `std::pmr::memory_resource` that asserts on fallback).

---

## Attribute 5 — Maintainability

**Current state:** `XML_Core.hpp` is a monolithic include-all header. `ISource.hpp` contains utility method implementations (`isWS`, `ignoreWS`, `match`) that belong in the implementation layer. The `IACTION_EVENT` macro hides virtual methods from IDE tooling. `variants/` and `node/` folder naming is inconsistent — node variants live in `variants/`, but node accessors live in `node/`.

### Tasks

#### 5.1 — Split utility methods out of `ISource`

**File:** `classes/include/interface/ISource.hpp`

Move `isWS()`, `ignoreWS()`, and `match()` to a new inline helper `XML_SourceHelpers.hpp` in `implementation/common/`. `ISource` becomes a pure abstract interface with no inline method bodies.

This change requires adding `#include "XML_SourceHelpers.hpp"` wherever those helpers are called.

#### 5.2 — Replace `IACTION_EVENT` macro with explicit method pairs

**File:** `classes/include/interface/IAction.hpp`

Expand the macro into explicit `virtual void onX(Node &)` and `virtual void onX(const Node &)` declarations. The macro saves ~10 lines but defeats IDE go-to-definition, autocomplete, and grep. Replace with explicit declarations:

```cpp
virtual void onNode(Node &node) {}
virtual void onNode(const Node &node) {}
virtual void onCDATA(Node &node) {}
virtual void onCDATA(const Node &node) {}
// ... etc
```

#### 5.3 — Rename `variants/` folder to `nodes/`

**Files:** All `#include` paths referencing `implementation/variants/`, `CMakeLists.txt`, `tests/CMakeLists.txt`

The subdirectory holds node type definitions (`XML_Element.hpp`, `XML_Comment.hpp`, etc.) but is named `variants/` — the type is `Variant` but the content represents node kinds. Rename to `nodes/` and update all include paths and CMake include directories.

Alternatively, keep the folder name but rename the `node/` folder to `node_accessors/` to make the distinction clearer. Evaluate impact before committing.

#### 5.4 — Reduce `XML_Core.hpp` scope

**File:** `classes/include/implementation/XML_Core.hpp`

This header includes ~30 other headers unconditionally. Restructure so that each implementation `.cpp` includes only the headers it needs directly. `XML_Core.hpp` should remain as a convenience aggregate for files that genuinely need everything (e.g., `Default_Parser.cpp`), but mark it as `// Internal aggregate — do not include in new files`.

**Acceptance criteria:**
- Every public interface method (in `IAction`, `ISource`, etc.) is directly navigable in VS Code via Go-to-Definition.
- `clang-tidy` reports no `readability-macro-usage` warnings for the refactored headers.
- Compilation time of a clean build decreases measurably (target: ≥ 10 % reduction).

---

## Attribute 6 — Flexibility and Customization

**Current state:** All five extension interfaces (`IParser`, `IStringify`, `IAction`, `IEntityMapper`, `IValidator`) are injectable, which is good. However, there is no runtime control over parse limits, no SAX-style event streaming, and no way to customise entity resolution policy without replacing the whole `IEntityMapper`.

### Tasks

#### 6.1 — Add a `ParseOptions` struct

**File:** `classes/include/XML.hpp`

```cpp
struct ParseOptions {
    std::size_t maxEntityExpansionDepth  = 512;   // see §7.1
    std::size_t maxNestingDepth          = 1000;  // see §7.2
    std::size_t maxAttributeCount        = 10000;
    bool        allowExternalEntities    = false; // see §7.3
};
```

Pass `ParseOptions` through to `XML_Impl` and `IParser`. The `Default_Parser` reads these values during parse. Existing zero-argument `parse()` overloads use the default-constructed `ParseOptions`.

```cpp
void parse(ISource &source, const ParseOptions &options = {}) const;
void parse(ISource &&source, const ParseOptions &options = {}) const;
```

#### 6.2 — Add `IEntityResolver` hook

**File:** new `classes/include/interface/IEntityResolver.hpp`

```cpp
class IEntityResolver {
public:
    virtual ~IEntityResolver() = default;
    // Return the replacement text for an external entity, or std::nullopt to deny.
    [[nodiscard]] virtual std::optional<std::string>
        resolve(const std::string_view &systemId,
                const std::string_view &publicId) = 0;
};
```

Inject via `ParseOptions::entityResolver`. When `allowExternalEntities` is `false` (default), the built-in resolver denies all external entities and throws `SyntaxError`. When an `IEntityResolver` is supplied, it is called instead. This replaces the current all-or-nothing external entity behaviour.

**Acceptance criteria:**
- Parsing with default `ParseOptions` behaves identically to current behaviour (no observable change).
- Supplying a custom `IEntityResolver` allows controlled external entity loading.
- `ParseOptions::maxEntityExpansionDepth = 1` causes a `SyntaxError` on the first entity expansion attempt.

---

## Attribute 7 — Strong Security

**Current state:** The library has no limits on entity expansion depth (XML bomb / billion laughs), no nesting depth limit, and external DTD entity resolution is unrestricted (XXE). `toFile`/`fromFile` take raw `string_view` paths with no validation.

### Tasks

#### 7.1 — Enforce entity expansion depth limit (XML Bomb / Billion Laughs)

**File:** `classes/source/implementation/entity/XML_EntityMapper.cpp`, `XML_EntityMapperHelpers.cpp`

When resolving an entity reference that itself expands to other entity references, track a recursion depth counter. When it exceeds `ParseOptions::maxEntityExpansionDepth` (default 512), throw `SyntaxError("Entity expansion depth limit exceeded")`.

#### 7.2 — Enforce element nesting depth limit

**File:** `classes/source/implementation/xml/parser/Default_Parser.cpp` (or wherever elements are pushed to the parse stack)

Maintain a current depth counter incremented on element open and decremented on element close. When depth exceeds `ParseOptions::maxNestingDepth` (default 1000), throw `SyntaxError("Maximum element nesting depth exceeded")`.

#### 7.3 — Deny external entities by default (XXE prevention)

**File:** `classes/source/implementation/dtd/DTD_Validator_Impl_Parse.cpp` (entity resolution site)

Add a guard at the point where `SYSTEM` or `PUBLIC` external entities are about to be fetched:

```cpp
if (!parseOptions.allowExternalEntities && !parseOptions.entityResolver) {
    XML_LIB_THROW(SyntaxError("External entity resolution is disabled. "
                               "Set ParseOptions::allowExternalEntities = true or supply an IEntityResolver."));
}
```

Document this clearly in `README.md` and `docs/Guide.md` under a "Security" heading.

#### 7.4 — Validate file paths in `fromFile`/`toFile`

**File:** `classes/source/implementation/xml/file/XML_File.cpp`

Use `std::filesystem::path` to normalise the path and check that it does not escape a designated base directory when one is configured. At minimum, reject paths containing null bytes and paths that are clearly relative traversals (`../`). Throw `XML::Error` with a clear message on rejection.

Signature change in `XML.hpp`:

```cpp
[[nodiscard]] static std::string fromFile(const std::filesystem::path &filePath);
static void toFile(const std::filesystem::path &filePath, const std::string_view &xmlString, Format format = Format::utf8);
```

The `string_view` overloads can remain as deprecated thin wrappers.

#### 7.5 — Add attribute count limit

**File:** `classes/source/implementation/xml/parser/Default_Parser.cpp`

When parsing attributes on an element, check that the running count does not exceed `ParseOptions::maxAttributeCount` (default 10 000). Throw `SyntaxError("Maximum attribute count exceeded")`.

**Acceptance criteria:**
- A crafted "billion laughs" XML document throws `SyntaxError` before exhausting memory.
- A deeply nested document (10 001 levels) throws `SyntaxError` with the nesting message.
- `xml.parse(BufferSource{"<!DOCTYPE foo [<!ENTITY e SYSTEM \"file:///etc/passwd\">]><foo>&e;</foo>"})` throws `SyntaxError` by default.
- A `std::filesystem::path` with a null byte is rejected.

---

## Attribute 8 — High Testability

**Current state:** Good Catch2 coverage organised by feature. However, tests include internal implementation headers directly, creating tight coupling. No mock implementations of `IParser`, `IAction`, `ISource` are provided. Embedded (no-exception) mode is not exercised by CI.

### Tasks

#### 8.1 — Provide mock/stub helpers in `tests/include/`

**Files:** new `tests/include/mocks/Mock_ISource.hpp`, `Mock_IAction.hpp`, `Mock_IParser.hpp`

Lightweight Catch2-compatible stubs:

```cpp
// Mock_ISource.hpp
struct Mock_ISource : ISource {
    std::string data;
    std::size_t pos = 0;
    Char current() const override { return pos < data.size() ? static_cast<Char>(data[pos]) : Char(0); }
    void next() override { if (pos < data.size()) ++pos; }
    bool more() const override { return pos < data.size(); }
    // ... minimal implementations
};
```

These allow unit tests to exercise individual parse helpers without building a full document.

#### 8.2 — Remove private implementation headers from test include paths

**File:** `tests/CMakeLists.txt`

Tests currently add every implementation subdirectory to their include path. Refactor tests so that only:
- `classes/include/` (public headers)
- `tests/include/` (test utilities/mocks)
- `catch2` headers

…are on the public include path. Tests that need to inspect internals should use the public API only. For the few tests that legitimately test internal helpers (e.g., character validation), add a separate `XML_LIB_TEST_INTERNALS` cmake option that re-adds the internal include paths.

#### 8.3 — Add embedded (no-exception) build test target

**File:** `tests/CMakeLists.txt`

```cmake
if(XML_LIB_NO_EXCEPTIONS)
  add_executable(XML_Lib_Embedded_Tests ...)
  target_compile_definitions(XML_Lib_Embedded_Tests PRIVATE XML_LIB_NO_EXCEPTIONS)
  # Add subset of tests compatible with no-exception build
endif()
```

Add a `tests/source/embedded/` directory with tests that verify correct `std::terminate()`-on-error behaviour using process-exit testing (Catch2 `REQUIRE_THROWS` cannot be used; use `SKIP` with a note, or use death tests via a subprocess fixture).

#### 8.4 — Add `ParseOptions` security tests

**File:** new `tests/source/xml/XML_Lib_Tests_Security.cpp`

Cover every security limit from §7:
- Entity expansion depth exceeded.
- Nesting depth exceeded.
- External entity denied by default.
- External entity allowed via `allowExternalEntities = true`.
- Attribute count exceeded.

**Acceptance criteria:**
- `ctest` passes in both default and `XML_LIB_EMBEDDED` configurations.
- Code coverage (measured with `gcov`/`llvm-cov`) of `classes/source/` exceeds 85 %.

---

## Attribute 9 — Compatibility and Portability

**Current state:** `CMakeLists.txt` sets C++23 but README says C++20. The `${PLATFORM}` converter has only `linux` and `windows` paths; macOS is silently given the `linux` converter. `stdafx.h` at the root is a Windows precompiled header artefact with no role in the library. No CMake install target is defined.

### Tasks

#### 9.1 — Fix platform detection to include macOS

**File:** `CMakeLists.txt`

```cmake
if(APPLE)
  set(PLATFORM "macos")
elseif(MSVC)
  set(PLATFORM "windows")
else()
  set(PLATFORM "linux")
endif()
```

Create `classes/source/implementation/converter/macos/XML_Converter.cpp` (can initially be a symlink or copy of the linux version if identical, with a comment explaining why).

#### 9.2 — Add CMake install target

**File:** `CMakeLists.txt`

```cmake
include(GNUInstallDirs)
install(TARGETS ${XML_LIBRARY_NAME}
    EXPORT XML_LibTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY classes/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/XML_Lib
    FILES_MATCHING PATTERN "*.hpp"
    PATTERN "implementation" EXCLUDE)   # only public headers
install(EXPORT XML_LibTargets
    FILE XML_LibTargets.cmake
    NAMESPACE XML_Lib::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/XML_Lib)
```

Also generate a `XML_LibConfig.cmake` and version file via `CMakePackageConfigHelpers`.

#### 9.3 — Remove or reclassify `stdafx.h`

**File:** `stdafx.h` (root)

This file is empty or serves as a Windows PCH artifact. Either delete it or move it inside a `windows/` directory with a comment. Update `CMakeLists.txt` to use it as a precompiled header only on MSVC (`target_precompile_headers`).

#### 9.4 — Verify C++23 requirements and document minimum toolchain

**File:** `README.md`, `CMakeLists.txt`

Audit which C++23 features are actually used (structured bindings with auto, deduced `this`, etc.) versus which C++20 features could suffice. If the library only uses C++20 features + `std::pmr` (C++17), lower the standard requirement to C++20 and document the finding. If genuine C++23 features are used, document the minimum compiler versions clearly.

**Acceptance criteria:**
- Building on macOS with AppleClang 15 succeeds.
- `cmake --install build --prefix /tmp/xml_lib_install` produces a usable install tree.
- A downstream `CMakeLists.txt` with `find_package(XML_Lib)` and `target_link_libraries(myapp XML_Lib::XML_Lib)` works without additional `-I` flags.

---

## Attribute 10 — Low Dependency Footprint

**Current state:** Excellent — zero runtime dependencies, only Catch2 for tests. `std::pmr` is the only C++17 dependency. The `FetchContent` Catch2 download requires internet access at configure time.

### Tasks

#### 10.1 — Make Catch2 dependency optional / local-first

**File:** `tests/CMakeLists.txt`

Support a local Catch2 installation before falling back to `FetchContent`:

```cmake
find_package(Catch2 3 QUIET)
if(NOT Catch2_FOUND)
  message(STATUS "Catch2 not found locally; downloading via FetchContent")
  FetchContent_Declare(Catch2 ...)
  FetchContent_MakeAvailable(Catch2)
endif()
```

This allows offline builds and avoids pinning the Catch2 version in build systems that vendor their own.

#### 10.2 — Document dependency footprint in `README.md`

**File:** `README.md`

Add a "Dependencies" section:

```
## Dependencies
- **Runtime:** None. XML_Lib uses only the C++23 standard library.
- **Test (optional):** Catch2 v3 (fetched automatically if not found locally).
- **Documentation (optional):** Doxygen + Graphviz (for `cmake --build . --target docs`).
```

#### 10.3 — Add `std::pmr` availability guard for older toolchains

**File:** `CMakeLists.txt`

```cmake
include(CheckCXXSourceCompiles)
check_cxx_source_compiles("
#include <memory_resource>
int main() { auto *r = std::pmr::get_default_resource(); (void)r; }
" HAS_PMR)
if(NOT HAS_PMR)
  message(FATAL_ERROR "XML_Lib requires std::pmr (C++17). Your toolchain does not provide it.")
endif()
```

This gives a clear error rather than a cryptic compile failure on older GCC/Clang.

**Acceptance criteria:**
- `cmake -DXML_LIB_BUILD_TESTS=ON ..` succeeds without internet access when Catch2 ≥ 3 is installed system-wide.
- `ldd` on the built library shows no external shared-library dependencies (other than libc/libstdc++).

---

## Implementation Sequence

The following ordering minimises rework and unblocks later tasks:

| Phase | Tasks | Rationale |
|-------|-------|-----------|
| 1 (Security — immediate) | 7.1, 7.2, 7.3, 7.4, 7.5 | Security fixes should not wait; can be done independently. |
| 2 (Reliability) | 1.3, 3.1, 3.2, 3.3 | Fix correctness bugs before adding new API surface. |
| 3 (API usability) | 1.1, 1.2, 1.4, 6.1, 6.2 | `ParseOptions` (6.1) is needed by security tests (8.4). |
| 4 (Testability) | 8.1, 8.2, 8.3, 8.4 | Write tests once the new API is stable. |
| 5 (Portability) | 9.1, 9.2, 9.3, 9.4, 10.1, 10.3 | Install target and platform support. |
| 6 (Performance) | 4.1, 4.2, 4.3, 4.4 | Profile after correctness is confirmed. |
| 7 (Maintainability) | 5.1, 5.2, 5.3, 5.4 | Refactor once feature set is frozen. |
| 8 (Documentation) | 2.1, 2.2, 2.3, 2.4, 10.2 | Document the final state, not intermediate states. |

---

## File Change Summary

| File | Change type | Attribute(s) |
|------|-------------|--------------|
| `classes/include/XML.hpp` | Add convenience overloads; add `ParseOptions`; path→filesystem | 1, 6, 7 |
| `classes/source/XML.cpp` | Implement new overloads | 1, 6 |
| `classes/include/interface/IAction.hpp` | Replace `IACTION_EVENT` macro | 5 |
| `classes/include/interface/ISource.hpp` | Remove inline method bodies | 5 |
| `classes/include/interface/IParser.hpp` | Accept `ParseOptions` | 6 |
| `classes/include/interface/IEntityResolver.hpp` | **New file** | 6, 7 |
| `classes/include/implementation/node/XML_Node_Reference.hpp` | Fix error message swap | 1, 3 |
| `classes/include/implementation/common/XML_Error.hpp` | Remove terminate from exception-enabled path | 3 |
| `classes/include/implementation/common/XML_SourceHelpers.hpp` | **New file** (extracted helpers) | 5 |
| `classes/include/implementation/common/XML_Arena.hpp` | Size from compile-time constant | 4 |
| `classes/include/implementation/variants/XML_Element.hpp` | PMR attribute vectors | 4 |
| `classes/include/implementation/XML_Impl.hpp` | Accept `ParseOptions`; arena size constant | 4, 6 |
| `classes/include/XML_Node.hpp` | **New public header** (re-exports `isA`, `NRef`) | 1 |
| `classes/source/implementation/entity/XML_EntityMapper*.cpp` | Entity expansion depth limit | 7 |
| `classes/source/implementation/xml/parser/Default_Parser.cpp` | Nesting + attribute count limits | 7 |
| `classes/source/implementation/dtd/DTD_Validator_Impl_Parse.cpp` | External entity guard | 7 |
| `classes/source/implementation/xml/file/XML_File.cpp` | Filesystem path validation | 7 |
| `tests/CMakeLists.txt` | Prune internal includes; add embedded target | 8, 9 |
| `tests/include/mocks/Mock_ISource.hpp` | **New file** | 8 |
| `tests/include/mocks/Mock_IAction.hpp` | **New file** | 8 |
| `tests/include/mocks/Mock_IParser.hpp` | **New file** | 8 |
| `tests/source/xml/XML_Lib_Tests_Security.cpp` | **New file** | 7, 8 |
| `CMakeLists.txt` | macOS platform; install target; PMR check; arena KB option | 4, 9, 10 |
| `docs/Guide.md` | Complete rewrite | 2 |
| `docs/Doxyfile` | **New file** | 2 |
| `README.md` | Fix C++ standard, URL, add Dependencies/Security sections | 2, 7, 10 |
| `stdafx.h` | Remove or move to `windows/` | 9 |
