# Refactoring Plan: Sources Subsystem (Total SOLID Architecture)

## 1. Executive Summary

This document presents a comprehensive analysis of the XML source input subsystem (`ISource`, `BufferSource`, `FileSource`, `SourceFactory`, `XML_SourceHelpers`) in `XML_Lib`. It details current SOLID principle violations and outlines a concrete, phased refactoring plan to bring the source subsystem into complete alignment with SOLID software design principles.

---

## 2. SOLID Architectural Analysis & Current Deficiencies

### 2.1 Single Responsibility Principle (SRP)
* **Overloaded Concrete Sources (`BufferSource` & `FileSource`)**:
  - `BufferSource` currently manages raw memory access, BOM detection, UTF-16 byte order swapping, UTF-8 to UTF-16 encoding conversion, CRLF-to-LF line ending normalization, position tracking, and range slice extraction.
  - `FileSource` combines file system `std::ifstream` lifecycle management, disk seeking, CRLF normalization on-the-fly during character iteration, position tracking, and raw disk range reading.
* **Inline Implementation Bloat**: Both `BufferSource` and `FileSource` have complete implementations defined directly inside `.hpp` headers (`classes/include/implementation/io/`), mixing interface declaration with low-level execution logic.

### 2.2 Open/Closed Principle (OCP)
* **Hardcoded Input Normalization**: Encoding detection/conversion and line-ending translations are embedded directly within concrete source classes. Supporting new input sources (e.g., `std::istream`, memory-mapped files, network streams, socket readers) or custom filters (e.g., decryption, decompression) currently requires duplicating or altering normalization logic.
* **Non-Extensible Factory Design**: `SourceFactory` provides static creation methods, preventing clients from extending or mocking factory creation without modifying `SourceFactory` code.

### 2.3 Liskov Substitution Principle (LSP)
* **Inconsistent `getRange()` Behavioral Contracts**:
  - `BufferSource::getRange(start, end)` expects character offsets within the normalized UTF-16 buffer and returns a decoded UTF-8 `std::string`.
  - `FileSource::getRange(start, end)` treats `start` and `end` as raw byte offsets in `std::ifstream` and returns raw file bytes without UTF-8 encoding guarantee or normalization.
  - Calling `getRange()` through an `ISource` abstraction yields divergent results depending on the underlying runtime type.
* **Boundary & State Guarantee Variances**: `BufferSource::backup()` clamps position to index `0` when underflowing, whereas `FileSource::backup()` seeks relative to current position with stream error state clear logic, leading to subtle behavioral mismatches.

### 2.4 Interface Segregation Principle (ISP)
* **Monolithic Parameter Passing in Parsing Subsystems**:
  - Helper functions in `XML_SourceHelpers.hpp` (`isWS`, `ignoreWS`, `match`) and parser modules (`XML_ParseHelpers.cpp`, `XML_Parse.cpp`, `DTD_Impl`) take monolithic `ISource &` parameters, even though many only require `ICharStream &` (for character navigation) or `ILocationTracker &` (for location reporting).
  - Callers are tightly bound to the complete `ISource` interface rather than narrow, task-specific role interfaces (`ICharStream`, `ILocationTracker`, `IRangeReader`, `IResettableStream`).

### 2.5 Dependency Inversion Principle (DIP)
* **Interface Layer Pollution**:
  - `classes/include/interface/XML_SourceFactory.hpp` is located in the public `interface/` directory, yet directly `#include`s low-level implementation headers: `"implementation/io/XML_BufferSource.hpp"` and `"implementation/io/XML_FileSource.hpp"`.
  - High-level interface headers depend directly on concrete implementation details, breaking Dependency Inversion.

---

## 3. Target SOLID Architecture

```
                                  +-------------------+
                                  |    ICharStream    |
                                  +-------------------+
                                  | +current(): Char  |
                                  | +next(): void     |
                                  | +more(): bool     |
                                  | +backup(long): void|
                                  +---------+---------+
                                            |
         +----------------------------------+----------------------------------+
         |                                  |                                  |
+--------v----------+              +--------v----------+              +--------v----------+
| ILocationTracker  |              |   IRangeReader    |              | IResettableStream |
+-------------------+              +-------------------+              +-------------------+
| +position(): long |              | +getRange(): str  |              | +reset(): void    |
| +getPosition()    |              +--------+----------+              +--------+----------+
| +getSystemId()    |                       |                                  |
+--------+----------+                       |                                  |
         |                                  |                                  |
         +----------------------------------+----------------------------------+
                                            |
                                  +---------v---------+
                                  |      ISource      |  <--- Pure Abstract Interface
                                  +-------------------+
                                            |
                  +-------------------------+-------------------------+
                  |                                                   |
        +---------v---------+                               +---------v---------+
        |   BufferSource    |                               |    FileSource     |
        +-------------------+                               +-------------------+
        | - buffer: String  |                               | - source: ifstream|
        | - tracker: Tracker|                               | - tracker: Tracker|
        +-------------------+                               +-------------------+
                  |                                                   |
                  +-------------------------+-------------------------+
                                            |
                                  +---------v---------+
                                  | LineColumnTracker |
                                  +-------------------+
                                  | +advance(ch, pos) |
                                  | +rewindTo(pos)    |
                                  +-------------------+

                                  +-------------------+
                                  |  ISourceFactory   |  <--- Interface in interface/
                                  +-------------------+
                                  | +createBufferSource|
                                  | +createFileSource |
                                  +---------+---------+
                                            |
                                  +---------v---------+
                                  | SourceFactoryImpl |  <--- Implementation in implementation/
                                  +-------------------+
```

---

## 4. Step-by-Step Refactoring Plan

### Phase 1: Dependency Inversion Fix (DIP & Interface Hygiene)
1. **Decouple `XML_SourceFactory.hpp`**:
   - Refactor `XML_SourceFactory.hpp` into a pure abstract interface class `ISourceFactory` in `classes/include/interface/XML_SourceFactory.hpp`. Remove includes of `XML_BufferSource.hpp` and `XML_FileSource.hpp`.
   - Create concrete implementation `SourceFactoryImpl` in `classes/include/implementation/io/XML_SourceFactoryImpl.hpp` and `classes/source/implementation/io/XML_SourceFactoryImpl.cpp`.
   - Provide a default factory accessor or static creation methods on `SourceFactoryImpl` while keeping the interface abstract.

### Phase 2: Concrete Source Declaration & Implementation Separation (SRP & Maintainability)
1. **Separate Headers and CPP Implementation Files**:
   - Split `XML_BufferSource.hpp` into clean declaration in `classes/include/implementation/io/XML_BufferSource.hpp` and implementation in `classes/source/implementation/io/XML_BufferSource.cpp`.
   - Split `XML_FileSource.hpp` into clean declaration in `classes/include/implementation/io/XML_FileSource.hpp` and implementation in `classes/source/implementation/io/XML_FileSource.cpp`.
2. **Isolate Stream Normalization**:
   - Ensure CRLF normalization and encoding adjustments are handled cleanly via `StreamNormalizer` without duplicating code in `FileSource` or `BufferSource`.

### Phase 3: Liskov Substitution & Contract Alignment (LSP)
1. **Standardize `getRange()` Semantics**:
   - Ensure both `BufferSource::getRange` and `FileSource::getRange` accept consistent index semantics (character/byte positions) and return normalized UTF-8 string output.
   - Verify boundary conditions (`start < 0`, `end > size`, `end < start`) throw identical `ISource::Error` exception types with descriptive messages.
2. **Standardize Boundary Behaviors (`backup`, `reset`, EOF)**:
   - Align position tracking and rewind semantics across `FileSource` and `BufferSource` using `LineColumnTracker`.

### Phase 4: Interface Segregation in Consumers (ISP)
1. **Refactor Helper Signatures**:
   - Update `XML_SourceHelpers.hpp` functions (`isWS`, `ignoreWS`, `match`) to accept `ICharStream &` instead of `ISource &`.
   - Update `getPosition` helper to accept `ILocationTracker &`.
2. **Refactor Parser / Lexer Functions**:
   - Audit `XML_ParseHelpers`, `XML_Parse`, and `DTD_Impl` functions and narrow parameters to `ICharStream &` or `ILocationTracker &` where full `ISource` access is not required.

### Phase 5: Build System, Verification & Tests
1. **Update `CMakeLists.txt`**:
   - Add new source files (`XML_BufferSource.cpp`, `XML_FileSource.cpp`, `XML_SourceFactoryImpl.cpp`) to the CMake target definitions.
2. **Expand Unit Test Suite**:
   - Update and execute Catch2 unit tests in `tests/source/io/XML_Lib_Tests_ISource.cpp` to verify all 4 segregated interfaces, factory creation, range extraction invariants, and LSP compliance.

---

## 5. Verification Plan

### Automated Build & Test Execution
1. **Clean Rebuild**:
   ```bash
   cmake -B build -S .
   cmake --build build --config Debug
   ```
2. **Execute Full Test Suite**:
   ```bash
   cd build && ctest --output-on-failure
   ```
3. **Execute Targeted ISource Tests**:
   ```bash
   ./tests/XML_Lib_Tests "[XML][ISource]"
   ./tests/XML_Lib_Tests "[XML][FileSource]"
   ./tests/XML_Lib_Tests "[XML][BufferSource]"
   ```
