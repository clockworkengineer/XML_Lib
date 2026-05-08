# Embedded Systems Refactor Plan for XML_Lib

## Purpose
Provide a concrete, actionable refactor plan to enhance XML_Lib for embedded systems development. The plan focuses on reducing binary size, eliminating heap-dependent runtime behavior, lowering dynamic memory use, and exposing a minimal buildable library surface for constrained targets.

## Current Embedded-Relevant Findings
- `CMakeLists.txt` already exposes embedded-friendly build options: `XML_LIB_EMBEDDED`, `XML_LIB_NO_EXCEPTIONS`, `XML_LIB_NO_RTTI`, and `XML_LIB_MINIMAL_FEATURES`.
- The public API and implementation currently rely heavily on `std::string`, `std::vector`, `std::unique_ptr`, and exceptions.
- `XML.hpp` uses `std::string_view` in some APIs, but many parse/serialize operations still require full `std::string` allocations.
- The library ships with optional components (`XPath`, `XSD`, `DTD`, `stringify`) that should be configurable independently for embedded builds.
- Internal headers are exposed via wide include paths in CMake, increasing build surface and compile-time dependency risk for embedded users.

## Embedded Refactor Goals
1. Minimize heap allocation and dynamic memory dependence.
2. Separate embedded-safe subset from the full feature set.
3. Eliminate exception-based control flow in embedded builds.
4. Reduce public header exposure and improve ABI/compile-time robustness.
5. Add build tooling to verify footprint, size, and feature gating.

## Concrete Refactor Tasks

### 1. Harden Embedded Build Configuration
- Keep `XML_LIB_EMBEDDED` as the umbrella option.
- Ensure `XML_LIB_EMBEDDED` forces:
  - `XML_LIB_NO_EXCEPTIONS=ON`
  - `XML_LIB_NO_RTTI=ON`
  - `XML_LIB_MINIMAL_FEATURES=ON`
  - `XML_LIB_BUILD_SIZE_OPTIMIZED=ON`
- Add or refine optional flags:
  - `XML_LIB_ENABLE_XPATH`
  - `XML_LIB_ENABLE_XSD`
  - `XML_LIB_ENABLE_DTD`
  - `XML_LIB_ENABLE_STRINGIFY`
  - `XML_LIB_ENABLE_PERFORMANCE_COUNTERS`
- Add explicit build-type guidance:
  - default `CMAKE_BUILD_TYPE=Release` for normal builds
  - `MinSizeRel` or a custom `Embedded` config for size-focused builds
- Add embedded-specific compiler options in `CMakeLists.txt`:
  - `-fno-exceptions`, `-fno-rtti` when requested
  - `-ffunction-sections`, `-fdata-sections`, and linker `--gc-sections`
  - `-Os` for size-optimized embedded builds
  - `-fvisibility=hidden` where supported

### 2. Reduce Public API Surface and Build Includes
- Expose only the stable public headers in `classes/include/`:
  - `XML.hpp`
  - `XPath.hpp`
  - `XSD_Validator.hpp`
  - `DTD_Validator.hpp`
  - any minimal support headers needed by embedded users
- Move all implementation and internal headers into private include paths such as:
  - `classes/include/implementation/`
  - `classes/include/interface/`
- Restrict `target_include_directories()` so implementation headers are `PRIVATE`.
- Keep `XML_Config.hpp` generated and consumed internally; do not expose it as part of the public install layout.

### 3. Introduce an Embedded Minimal Feature Subset
- Define a small embedded profile that compiles with only core XML parsing and optional stringify support.
- Disable higher-level features by default in the embedded profile:
  - XPath engine
  - XSD validation
  - DTD validation (optional)
- Provide explicit lightweight entry points and types for embedded users.
- Keep `XML::parse()`, `XML::stringify()`, `XML::traverse()`, and low-allocation `BufferSource` / `FileSource` as the primary API surface for embedded.

### 4. Remove or Reduce Heap-Allocating Parse Hot Paths
- Audit parser internals in `classes/source/implementation/common/XML_Parse.cpp` and related modules.
- Replace per-token `std::string` copies with `std::string_view` or internal buffer span views wherever the source buffer remains valid.
- Prefer temporary fixed-size stacks or arena buffers for token text, names, and attribute values.
- Reserve vector capacity before building element/attribute lists, especially in tree construction.
- Avoid repeated conversions from source encodings to UTF-8 inside tight loops.
- Where possible, parse directly from the source buffer into node metadata rather than copying full string values.

### 5. Eliminate Exceptions from Embedded Code Paths
- Audit error handling across the codebase.
- Introduce an embedded-safe error policy using return codes or an error callback when `XML_LIB_NO_EXCEPTIONS=ON`.
- Keep exception-based APIs for desktop/non-embedded builds, but ensure embedded builds do not require `std::exception`.
- Provide an internal `Status`/`Result` type for embedded validation and parse errors.

### 6. Add Pool/Arena Memory Management for Node Trees
- Evaluate a single allocation arena for the DOM/node tree and parser temporary structures.
- Consider `std::pmr::monotonic_buffer_resource` or a custom arena type guarded by `XML_LIB_EMBEDDED`.
- Allocate node objects, name/value storage, and parser metadata from the arena to reduce fragmentation.
- Add cleanup semantics that avoid calling `delete` per node in embedded configuration.

### 7. Make Optional Components Truly Optional
- Ensure `XML_LIB_ENABLE_XPATH`, `XML_LIB_ENABLE_XSD`, and `XML_LIB_ENABLE_DTD` remove corresponding source files from the build and exclude APIs from the embedded public surface.
- Guard feature implementations with `#if` around compile-time flags rather than runtime switches.
- Add targeted `CMakeLists.txt` conditions and `target_compile_definitions` for feature gating.

### 8. Add Embedded-Focused Tests and Size Regression Checks
- Create a small embedded test suite under `tests/source/embedded/`.
- Cover:
  - minimal XML parse and stringify round-trips
  - file-buffer parse with no heap growth
  - option flags producing a working `XML_LIB_EMBEDDED` build
  - size and symbol visibility checks
- Add a `size_report` target to compare embedded vs full library sizes.
- Add a minimal runtime regression test for heap activity if possible.

## Prioritized Implementation Roadmap
1. Refine `CMakeLists.txt` embedded and feature-gating options.
2. Narrow public header exposure and private include directories.
3. Implement minimal embedded profile and verify `XML_LIB_EMBEDDED` build.
4. Refactor parser and tree allocations for lower dynamic memory usage.
5. Replace exceptions with embedded-safe error handling where required.
6. Add embedded regression tests and size-reporting support.

## Expected Outcomes
- A buildable embedded profile with no exceptions, no RTTI, and size-optimized compiler options.
- Smaller binary size due to feature gating, hidden symbols, and reduced object code.
- Lower runtime heap churn through reserved containers and span-based parsing.
- A clean public API surface for embedded consumers.
- Better maintainability via explicit internal/public header separation.

## Suggested Next Deliverables
- `docs/embedded_refactor_plan.md` (this file)
- Updated `CMakeLists.txt` with embedded profile options and private include handling
- A new `tests/source/embedded/` mini-suite
- A memory arena implementation behind `XML_LIB_EMBEDDED`
- A minimal `XML_LIB_EMBEDDED` build target or CMake configuration example
