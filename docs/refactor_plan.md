# XML_Lib Refactor Plan: Reduce Size and Increase Performance

## Summary
This refactor plan targets the library's core XML parsing, validation, entity mapping, XPath evaluation, build configuration, and public API boundaries. The goal is to reduce binary size, lower memory allocations, improve execution speed, and simplify compile-time dependencies.

## Key Findings
- `classes/include/XML_Core.hpp` currently exposes a large monolithic include surface that pulls many internal headers into public builds.
- Parser code such as `classes/source/implementation/common/XML_Parse.cpp` allocates many temporary `std::string` objects and converts characters repeatedly during parse loops.
- Entity mapping and translation in `classes/source/implementation/entity/XML_EntityMapper.cpp` use repeated string copies, filesystem checks and replacement loops that degrade performance.
- XSD parsing and validation in `classes/source/implementation/xsd/XSD_Validator_Impl_Parse.cpp` and `classes/source/implementation/xsd/XSD_Validator_Impl_Validate.cpp` perform repeated string tag/attribute lookups and allocate intermediate strings frequently.
- XPath evaluation in `classes/source/implementation/xpath/XPath_Evaluator.cpp` builds result strings for node tests, value conversion and node traversal repeatedly, causing high allocation overhead.
- CMake currently defines compile options but lacks explicit `Release` optimization presets, size-focused build variants, and hidden symbol visibility controls.

## Refactor Goals
1. Reduce generated library code size by eliminating unnecessary public headers and by minimizing code duplication.
2. Increase runtime performance for XML parse/serialize/validation/XPath operations.
3. Improve memory efficiency by reducing temporary allocations and by using better container and string handling patterns.
4. Harden the public ABI boundary so only stable public APIs are exposed.
5. Add build options for optimized builds and size-optimized builds.

## Concrete Refactor Tasks

### 1. Narrow the public include surface
- Move internal-only headers under `classes/include/implementation/` or a private include directory.
- Expose only essential public headers such as `XML.hpp`, `XPath.hpp`, `XSD_Validator.hpp`, `DTD_Validator.hpp`, and `XML_Interfaces.hpp`.
- Remove `classes/include/XML_Core.hpp` from public exports or make it private.
- In `CMakeLists.txt`, restrict `target_include_directories` so only public includes are public and implementation headers are private.
- Add explicit `target_compile_definitions(${XML_LIBRARY_NAME} PRIVATE XML_LIB_INTERNAL)` for internal compilation units if needed.

### 2. Optimize parser string handling
- In `classes/source/implementation/common/XML_Parse.cpp`:
  - Replace repeated `std::string` growth patterns with reserved buffers or `String`/`Char`-based accumulation.
  - Avoid `toUtf8` conversions inside tight loops; convert only once per token or use a single output buffer.
  - Parse name/token boundaries into `std::string_view` or internal character span where possible.
  - Reduce `std::string` temporaries in `parseCharacterReference`, `parseEntityReference`, `parseValue`, and `parseTagBody`.
- Replace full string concatenation for parse errors with preformatted messages and `std::string_view` usage.

### 3. Improve entity mapping and translation
- In `classes/source/implementation/entity/XML_EntityMapper.cpp`:
  - Use `std::unordered_map<std::string, XML_EntityMapping>` with reserved capacity before bulk insertion.
  - Avoid rebuilding `std::string` names in `getEntityMapping` and `isPresent` by using a stable string key or by storing an interned key table.
  - Replace repeated `std::string` scans in `translate()` with a single-pass lexer or a trie-like scan for entity lookups.
  - Cache external entity file contents once per entity instead of re-reading filesystem entries repeatedly.
  - Consider switching `std::set<std::string>` to `std::unordered_set<std::string>` for recursion detection.

### 4. Reduce XSD parsing overhead
- In `classes/source/implementation/xsd/XSD_Validator_Impl_Parse.cpp`:
  - Replace repeated `localTag(child)`/`attrValue(child, ...)` calls with a single tag identifier and a reference to attributes when possible.
  - Convert `resolveType`, `localTag`, and `attrValue` to return `std::string_view` or avoid copies when the underlying node already stores the name/value.
  - Reduce repeated allocations in `childElements()` by reserving capacity when the child count is known or by iterating directly over children.
  - Use `std::string_view` for attribute names and type tag comparisons if the internal representation supports stable views.
- Where inline types are created, prefer emplace-back with reserved capacity and reuse objects rather than repeated `make_unique` in hot paths.

### 5. Optimize XPath evaluation
- In `classes/source/implementation/xpath/XPath_Evaluator.cpp`:
  - Avoid `nodeName(node)` and `nodeLocalName(node)` string creation on every node test. Cache node names or compare against the stored `name()` directly using `std::string_view`.
  - Replace `nodeStringValue()` recursion with a streaming visitor or a precomputed text cache for element descendants.
  - Avoid repeated conversions in `resultToNumber` and `resultToString` by storing only one canonical representation where possible.
  - Preallocate result vectors and reuse them across recursive XPath evaluation calls instead of allocating new vectors per axis or predicate.
  - Simplify axis traversal and node filtering logic to avoid returning full candidate copy lists when only counts or first matches are needed.

### 6. Improve runtime memory locality for tree and AST structures
- Introduce a parse-time memory arena or object pool for `Node` and related AST/node model objects.
- Evaluate whether `std::pmr::monotonic_buffer_resource` can be used for parse-heavy allocations in `XML_Impl`, `XSD_Impl`, and `XPath_Impl`.
- Use `reserve()` for vector containers in tree nodes, XSD particles, and XPath node sets when the expected size is known.
- Consider compacting node metadata and reducing pointer overhead in the XML DOM representation.

### 7. Build system and compiler optimization
- Update `CMakeLists.txt` to add explicit options:
  - `option(XML_LIB_ENABLE_LTO "Enable link-time optimization" ON)`
  - `option(XML_LIB_BUILD_SIZE_OPTIMIZED "Build with size optimization" OFF)`
  - `option(XML_LIB_BUILD_TESTS "Build unit tests" ON)`
- Add default `CMAKE_BUILD_TYPE` recommendation to `Release` for production builds.
- Add safe visibility options such as `-fvisibility=hidden` on non-Windows platforms.
- Add `target_compile_options(${XML_LIBRARY_NAME} PRIVATE $<$<CONFIG:Release>:-O3;-flto>)` and `target_compile_options(${XML_LIBRARY_NAME} PRIVATE $<$<CONFIG:MinSizeRel>:-Os;-flto>)`.
- Remove `XML_Config.hpp` from source list if it is only generated and already included implicitly.

### 8. Add performance regression and sizing metrics
- Create targeted performance tests for:
  - XML parse latency on large documents
  - XPath query latency on deep and wide node sets
  - XSD validation throughput
  - Serialization/stringify runtime
- Add binary size checks or size report targets to compare before/after.
- Use `tests/source/performance/` or a similar folder for microbenchmarks and record baseline numbers.

## Priority Workflow
1. Refactor public headers and CMake include boundaries.
2. Optimize parser string handling and entity mapper internals.
3. Reduce XSD parse/validate copy overhead.
4. Optimize XPath evaluator and node-name handling.
5. Add build flags for Release/LTO/size optimizations.
6. Add benchmark/regression tests and validate against baseline.

## Recommended Deliverables
- `docs/refactor_plan.md` (this file)
- `classes/include/*.hpp` public/private split with reduced export paths
- Updated `CMakeLists.txt` with build options and private include directories
- Refactored parser/validator/XPath source modules with fewer string temporaries
- New performance regression test suite in `tests/source/performance`

## Expected Impact
- Smaller compiled library binary by reducing exported headers and internal symbol exposure.
- Faster XML parse/validation/XPath execution by eliminating repeated string allocations and redundant node traversal.
- Better memory efficiency through reserved containers, pooled allocations, and lower overall heap traffic.
- Cleaner public interface, easier maintenance, and better compile-time performance for consumers.
