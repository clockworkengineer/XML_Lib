# XML_Lib Hardening Plan

## Objective
Create a concrete, prioritized hardening plan for XML_Lib that improves input safety, resource exhaustion protection, parser robustness, and regression coverage.

## Summary
The library is a modern C++20 XML parser, validator, and serializer. Key hardening targets are:
- I/O and string handling boundaries
- XML parser recursion and entity expansion limits
- DTD/XSD/XPath validation safety
- compile-time security flags and sanitizer support
- stronger regression tests and fuzz coverage

## 1. Immediate Code Hardening Tasks

1.1 Validate and harden file-range operations
- `classes/include/implementation/io/XML_FileSource.hpp`
- Add explicit checks for `end >= start` and `end <= fileSize`
- Avoid `source.read(&rangeBuffer[0], size)` without verifying stream state
- Use `std::string::resize()` and `source.read(rangeBuffer.data(), size)`
- Validate `source.gcount()` after read and restore state safely

1.2 Safer C-string handling in output buffers
- `classes/include/implementation/io/XML_BufferDestination.hpp`
- Replace `strlen(bytes)` loops with size-safe APIs or overloads that accept explicit lengths
- Prevent reliance on null-termination for buffer append operations
- Add coverage for `add(const char * bytes)` when `bytes` contains embedded NULs or is empty

1.3 Strengthen source buffering and conversion
- `classes/include/implementation/io/XML_BufferSource.hpp`
- Add a maximum input size guard before UTF-16 conversion and normalization
- Harden BOM detection / encoding assumptions for malformed inputs
- Ensure `convertCRLFToLF()` cannot loop infinitely on invalid sequences

## 2. Parser and Validation Hardening

2.1 Review and enforce parser limits
- `classes/include/implementation/parser/Default_Parser.hpp`
- Verify and propagate `ParseOptions::maxNestingDepth`, `maxEntityExpansionDepth`, `maxAttributeCount`
- Expand checks to include:
  - maximum element count per document
  - maximum total attribute count for a document
  - maximum text/content node size
- Add explicit error handling for exceeded parser limits

2.2 External entity protection and XXE hardening
- `classes/include/XML.hpp`
- Confirm `ParseOptions::allowExternalEntities` defaults to `false`
- Ensure `IEntityResolver` only resolves external entities when explicitly configured
- Add test coverage covering disabled external entities and resolver override behavior

2.3 DTD/XSD/XPath attack surface
- Audit `classes/source/implementation/dtd/*`, `classes/source/implementation/xsd/*`, and `classes/source/implementation/xpath/*`
- Add limits for:
  - DTD entity recursion
  - XSD schema complexity and type expansion
  - XPath expression length and evaluation recursion
- Block or safely fail on unsupported external schema access or malformed schema constructs

2.4 Entity mapping and substitution
- `classes/include/implementation/entity/XML_EntityMapper.hpp`
- Audit `translate()` and entity lookup/definition handling for unbounded expansion
- Add safety checks for recursive entity definitions and repeated expansion

## 3. Testing and Regression Coverage

3.1 Add security-focused regression tests
- Create tests for invalid `getRange(start,end)` values
- Test `BufferDestination::add(const char *)` when passed empty strings and embedded NULs
- Add negative tests for parser limits: nesting depth, attribute count, entity expansion, text size
- Add tests for `allowExternalEntities == false` and custom `IEntityResolver`

3.2 Add fuzzing / malformed input corpus
- Introduce a new test dataset under `tests/files/` or `examples/files/` with:
  - huge documents
  - deep nesting
  - entity bombs (`Billion Laughs` style)
  - invalid UTF-8/UTF-16 sequences
  - malformed DTD/XSD and XPath payloads
- Build a fuzz harness or targeted fuzz test with Catch2 or libFuzzer

3.3 Enable sanitizers and static analysis in CI
- Add CMake options for `XML_LIB_ENABLE_ASAN`, `XML_LIB_ENABLE_UBSAN`, `XML_LIB_ENABLE_TSAN`
- Add a build target for `XML_LIB_BUILD_SECURITY_TESTS` or `XML_LIB_ENABLE_FUZZING`
- Enable stronger compiler warnings:
  - `-Wformat-security`, `-Wconversion`, `-Wsign-conversion`, `-Wshadow`, `-Wduplicated-branches`
- Add a CI job for sanitizer builds and security regression tests

## 4. Build-time and Configuration Hardening

4.1 Add hardened build options
- Define defaults in `CMakeLists.txt` for:
  - `XML_LIB_ENABLE_SECURITY_HARDENING`
  - `XML_LIB_DISABLE_DTD` / `XML_LIB_DISABLE_XSD` / `XML_LIB_DISABLE_XPATH` for minimal attack surface
  - `XML_LIB_MAX_XML_SIZE` / `XML_LIB_MAX_TOTAL_ATTRIBUTES`
- Expose `ParseOptions` values clearly in public API docs

4.2 Improve defensive API contracts
- Document that all public parse entry points are safe for untrusted input
- Use `noexcept` where appropriate for move/destructors
- Ensure bug reports mention `ParseOptions` maximums and XXE policy defaults

## 5. Audit Path

5.1 Immediate review targets
- `classes/include/implementation/io/XML_FileSource.hpp`
- `classes/include/implementation/io/XML_BufferDestination.hpp`
- `classes/include/implementation/io/XML_BufferSource.hpp`
- `classes/include/implementation/parser/Default_Parser.hpp`
- `classes/include/implementation/entity/XML_EntityMapper.hpp`
- `classes/include/XML.hpp`

5.2 Secondary review targets
- `classes/source/implementation/dtd/DTD_Validator_Impl_Parse.cpp`
- `classes/source/implementation/xsd/XSD_Validator_Impl_Parse.cpp`
- `classes/source/implementation/xpath/XPath_Lexer.cpp`
- `classes/include/implementation/common/XML_ParseHelpers.hpp`
- `classes/include/implementation/common/XML_Utility.hpp`

## 6. Milestones

1. Add boundary guards and safe I/O handling.
2. Harden parser limits and entity resolution policy.
3. Expand regression tests and add a fuzzing input corpus.
4. Add sanitizer/ASAN CI coverage and static analysis.
5. Document security configuration and release hardened defaults.

## Implementation status
- Steps 1 through 5 have been implemented and validated by build/test execution.
- Security build options, parser limits, I/O boundary checks, XPath/XSD/XEE hardening, and audit-path fixes are committed.

---

This plan is designed to convert the current codebase into a defensively hardened XML library with explicit resource limits, safe string handling, and regression coverage for malformed or malicious XML input.
