# Embedded Systems Refactor Plan for XML_Lib

## Purpose
This plan captures the current embedded-ready state of XML_Lib and defines concrete next actions to make the library truly embedded-friendly. It is tuned to the existing source tree, current CMake flags, and the runtime behavior of the XML parser and DOM layers.

## Current Embedded-Relevant Analysis
- `CMakeLists.txt` already provides a strong embedded foundation with:
  - `XML_LIB_EMBEDDED`
  - `XML_LIB_NO_EXCEPTIONS`
  - `XML_LIB_STRICT_NO_EXCEPTIONS`
  - `XML_LIB_NO_RTTI`
  - `XML_LIB_MINIMAL_FEATURES`
  - optional `XML_LIB_ENABLE_XPATH`, `XML_LIB_ENABLE_XSD`, `XML_LIB_ENABLE_DTD`, and `XML_LIB_ENABLE_STRINGIFY`
- The build system already forces minimal feature defaults and uses `MinSizeRel` when `XML_LIB_EMBEDDED` is enabled.
- The public API in `classes/include/XML.hpp` still exposes higher-level object model APIs and uses heap-heavy types such as `std::string`, `std::vector`, and `std::unique_ptr`.
- `classes/include/implementation/common/XML_Error.hpp` still defines `std::string`-based error payloads and a `terminate()` fallback when exceptions are disabled, which is a partial embedded fault policy.
- Internal parser, arena, and tree allocation code is partially gated by `XML_LIB_EMBEDDED`, but the public surface and build include layout remain broad.
- Embedded tests are present in `tests/source/embedded/`, and CMake now includes an `XML_Lib_Embedded_Unit_Tests` target plus an embedded size-report target.

## Embedded Refactor Goals
1. Create a compact embedded build profile that is independent from the desktop feature set.
2. Eliminate runtime heap dependence wherever safe and practical.
3. Provide a clean no-exceptions build path with explicit error propagation.
4. Make feature gates compile-time only and remove unused object code from embedded binaries.
5. Verify behavior and footprint with embedded-specific tests and size regression tooling.

## Concrete Refactor Plan

### 1. Harden the Embedded Build Profile
- Enforce the umbrella behavior in `CMakeLists.txt`:
  - `XML_LIB_EMBEDDED` forces `XML_LIB_NO_EXCEPTIONS=ON`, `XML_LIB_NO_RTTI=ON`, `XML_LIB_MINIMAL_FEATURES=ON`, and `XML_LIB_BUILD_SIZE_OPTIMIZED=ON`.
  - `XML_LIB_MINIMAL_FEATURES` forces `XPath`, `XSD`, and `DTD` off while allowing `XML_LIB_ENABLE_STRINGIFY` on.
- Add embedded-specific compiler and linker tuning:
  - `-fno-exceptions`, `-fno-rtti` when embedded flags are enabled.
  - `-ffunction-sections`, `-fdata-sections`, and linker `--gc-sections`.
  - `-Os` or a custom `Embedded` build type.
  - `-fvisibility=hidden` for library symbols.
- Document an embedded configuration example in `README.md` or `docs/`.

### 2. Narrow Public Header Exposure
- Keep `classes/include/` as the public API surface and make `classes/include/implementation/` private.
- Expose only the minimal public headers needed by embedded consumers:
  - `XML.hpp`
  - `XPath.hpp` (optional)
  - `XSD_Validator.hpp` (optional)
  - `DTD_Validator.hpp` (optional)
- Restrict implementation headers to `PRIVATE` include directories in the library target.
- Keep `XML_Config.hpp` internal and out of the public install layout.

### 3. Define a Minimal Embedded API Subset
- Create or document a reduced embedded API surface that includes:
  - core XML parse
  - stringify when enabled
  - traversal by callback/action
  - low-allocation buffer/file source primitives
- Keep higher-level interfaces like XPath, XSD, and DTD behind compile-time flags only.
- Consider an explicit embedded facade if the full `XML` class remains too broad.

### 4. Reduce Heap Allocation in Parsing
- Audit `classes/source/implementation/common/XML_Parse.cpp` and `classes/source/implementation/xml/parser/Default_Parser.cpp`.
- Replace temporary `std::string` allocations with `std::string_view` or buffer spans when the source buffer is alive.
- Reserve container capacity before building node lists and attribute vectors.
- Avoid repeated encoding conversions inside parser hot loops.
- Parse directly into node metadata where safe, instead of duplicating text content.

### 5. Replace Exception Control Flow with Embedded-Safe Status
- Replace the `terminate()` fallback in `XML_Error.hpp` with an explicit embedded error policy.
- Add a lightweight status/result type such as:
  - `struct XMLStatus { bool ok; ErrorCode code; const char *message; }`
- Provide embedded-friendly parse/validate APIs that return status instead of throwing.
- Ensure embedded builds do not require `<stdexcept>` or exception runtime support.

### 6. Expand Arena/Pool Allocation Support
- Build on existing arena work in `classes/include/implementation/XML_Impl.hpp` and parser headers.
- Ensure the arena services parser temporaries and DOM node allocation in embedded mode.
- Add scoped arena lifetime around `XML::parse()` so embedded trees are built without per-node `new/delete`.
- Use `std::pmr::monotonic_buffer_resource` where supported, and fall back to a simple custom arena if needed.
- Verify embedded destruction is fast and heap-free.

### 7. Make Optional Features Truly Optional
- Ensure `XML_LIB_ENABLE_XPATH`, `XML_LIB_ENABLE_XSD`, and `XML_LIB_ENABLE_DTD` exclude their source files from the library.
- Guard public API declarations behind `#if defined(...)` in headers.
- Use CMake conditions and compile definitions to make feature exclusion complete.
- Prevent disabled features from contributing to embedded object code.

### 8. Harden Embedded Regression and Footprint Testing
- Expand the embedded test suite in `tests/source/embedded/`.
- Cover:
  - minimal XML parse and stringify round-trips
  - buffer/file source parsing
  - proper embedded compile-time gating
  - size artifact existence and footprint expectations
- Enhance `size_report_embedded` to compare embedded vs full library sizes.
- Add a heap-allocation or low-memory regression test if feasible.

## Prioritized Implementation Roadmap
1. Harden `CMakeLists.txt` embedded defaults and add explicit embedded flags.
2. Narrow public header exposure and enforce private implementation includes.
3. Implement the minimal embedded API subset and verify `XML_LIB_EMBEDDED` build.
4. Refactor parser and tree allocations for lower dynamic memory usage.
5. Replace exceptions with embedded-safe status/error handling.
6. Add embedded regression tests and size-reporting support.

## Expected Outcomes
- A repeatable embedded build profile with enforced no-exceptions/no-RTTI settings.
- Smaller embedded binary footprint with unused feature code excluded.
- Lower runtime heap churn through reserved containers and span-based parsing.
- A safer public API surface for embedded consumers.
- Regression coverage that catches gating and footprint regressions early.

## Suggested Implementation Targets
- `docs/embedded_refactor_plan.md` (this file)
- `CMakeLists.txt` embedded profile enforcement and private include handling
- `tests/source/embedded/` coverage expansion
- `classes/include/implementation/common/XML_Error.hpp` embedded error policy
- `classes/source/implementation/common/XML_Parse.cpp` and parser memory reductions
- `classes/include/implementation/XML_Impl.hpp` arena allocation expansion
- `tests/CMakeLists.txt` embedded test target and size-report tightening
