# Refactoring Plan: Sources Architecture (SOLID Principles)

## 1. Executive Summary

This document provides a detailed architectural analysis of the XML source input subsystem (`ISource`, `BufferSource`, `FileSource`, `XML_SourceHelpers.hpp`) in `XML_Lib`. It details current violations of the SOLID design principles and establishes a concrete, step-by-step refactoring plan to achieve total SOLID compliance.

---

## 2. SOLID Violations Analysis

### 2.1 Single Responsibility Principle (SRP)
* **`ISource` interface bloat**: Combines stream navigation, coordinate tracking (`lineNo`, `columnNo`), substring slicing (`getRange`), stream resetting, and error exception definitions.
* **`BufferSource` overloaded responsibilities**:
  1. Byte-order detection & UTF-16 swapping magic.
  2. UTF-8 to UTF-16 encoding conversion.
  3. CRLF normalization (`convertCRLFToLF`).
  4. Memory bounds & buffer index management.
  5. Position tracking and range slicing.
* **`FileSource` overloaded responsibilities**:
  1. Standard file handle (`std::ifstream`) opening & lifecycle management.
  2. Dynamic line ending (`\r\n` vs `\n`) translation during streaming.
  3. Disk seeking/tellg operations.
  4. Raw disk range extraction.
  5. Position tracking.

### 2.2 Open/Closed Principle (OCP)
* Encoding transformations and line-ending normalizations are hardcoded inside `BufferSource` and `FileSource`. Adding support for new encodings, custom streams (e.g., `std::istream`, memory mapped files, network streams), or stream filters requires modifying existing source headers.
* High-level components (`Default_Parser`, `XSD_Validator_Impl`) perform explicit type checking via `dynamic_cast<FileSource*>` to retrieve system ID / file path metadata. Adding new source types breaks these callers unless modified.

### 2.3 Liskov Substitution Principle (LSP)
* **Inconsistent `getRange()` semantics**: `BufferSource::getRange` converts UTF-16 slices to UTF-8 using `toUtf8(...)`, whereas `FileSource::getRange` returns raw bytes read directly from `std::ifstream` without decoding. Passing different `ISource` implementations produces inconsistent behavior for range extraction.
* **Corrupted position on `backup()`**: Calling `backup(length)` rewinds character cursor / disk position in both `BufferSource` and `FileSource`, but leaves `lineNo` and `columnNo` unchanged. Subsequent `getPosition()` queries yield invalid line/column numbers.
* **Subtype specific querying**: Callers downcasting `ISource*` to `FileSource*` rely on subclass-specific methods (`getFileName()`) not present on `ISource`.

### 2.4 Interface Segregation Principle (ISP)
* `ISource` forces all clients to depend on a monolithic interface.
* Parsers requiring basic character iteration (`current()`, `next()`, `more()`) are forced to depend on methods for range extraction (`getRange`), absolute seek position (`position`), and resetting (`reset`).

### 2.5 Dependency Inversion Principle (DIP)
* `ISource` contains mutable member variables (`lineNo`, `columnNo`), violating pure abstract interface design.
* Client modules directly instantiate concrete `BufferSource` or `FileSource` instances rather than relying on abstract factories or dependency injection.

---

## 3. Target SOLID Architecture

```
                      +-------------------+
                      |   ICharStream     |
                      +-------------------+
                      | +current(): Char  |
                      | +next(): void     |
                      | +more(): bool     |
                      | +backup(long): void|
                      +---------+---------+
                                |
                                |
                      +---------v---------+
                      |     ISource       | <--- Pure Interface (Zero member variables)
                      +-------------------+
                      | +getSystemId()    |
                      +---------+---------+
                                |
          +---------------------+---------------------+
          |                                           |
+---------v---------+                       +---------v---------+
|   BufferSource    |                       |    FileSource     |
+-------------------+                       +-------------------+
| -buffer: String   |                       | -source: ifstream |
| -tracker: Tracker |                       | -tracker: Tracker |
+-------------------+                       +-------------------+
          |                                           |
          +---------------------+---------------------+
                                |
                       +--------v---------+
                       | LineColumnTracker|
                       +------------------+
                       | +advance(ch)     |
                       | +retreat(len)    |
                       +------------------+
```

### 3.1 Interface Segregation & Abstraction
1. **`ICharStream`**: Core character navigation contract (`current`, `next`, `more`, `backup`).
2. **`ILocationTracker`**: Position query contract (`position`, `getPosition`, `getSystemId`).
3. **`IRangeReader`**: Range extraction contract (`getRange`).
4. **`IResettableStream`**: Stream reset contract (`reset`).
5. **`ISource`**: Pure composite interface inheriting from the segregated contracts. No member variables.

### 3.2 Extracted Helper Components (Single Responsibility)
1. **`LineColumnTracker`**: Dedicated component handling line/column numbers, including accurate stack-based or rewind-aware tracking for `backup()`.
2. **`StreamNormalizer`**: Standalone helper for BOM detection, UTF-16 byte order swapping, and CRLF to LF normalization.

### 3.3 System Identification & Elimination of RTTI
* Add `virtual std::string getSystemId() const = 0` to `ISource`.
* `BufferSource::getSystemId()` returns `"<buffer>"` or a specified buffer label.
* `FileSource::getSystemId()` returns `filename`.
* Replace all `dynamic_cast<FileSource*>` in `Default_Parser.cpp` and `XSD_Validator_Impl.cpp` with virtual call `source.getSystemId()`.

### 3.4 Dependency Inversion via Source Factory
* Introduce `SourceFactory` with creation helpers:
  * `createBufferSource(...) -> std::unique_ptr<ISource>`
  * `createFileSource(...) -> std::unique_ptr<ISource>`

---

## 4. Implementation Steps

| Step | Scope | Description |
| :--- | :--- | :--- |
| **Step 1** | `LineColumnTracker` | Create `XML_LineColumnTracker.hpp` to manage line/column state and support position unwinding on `backup()`. |
| **Step 2** | `StreamNormalizer` | Create `XML_StreamNormalizer.hpp` to isolate BOM stripping, byte-swapping, and CRLF normalization. |
| **Step 3** | `ISource` Header | Split `ISource.hpp` into segregated interfaces (`ICharStream`, `ILocationTracker`, `IRangeReader`, `IResettableStream`), add `getSystemId()`, remove member fields. |
| **Step 4** | Concrete Sources | Update `BufferSource` and `FileSource` to utilize `LineColumnTracker` and `StreamNormalizer`, implement `getSystemId()`, and standardize `getRange()` encoding semantics. |
| **Step 5** | Call Sites | Replace RTTI `dynamic_cast<FileSource *>` in `Default_Parser.cpp` and `XSD_Validator_Impl.cpp` with `source.getSystemId()`. |
| **Step 6** | `SourceFactory` | Add `XML_SourceFactory.hpp` to enable clean dependency injection. |
| **Step 7** | Verification | Execute full Catch2 unit test suite and add dedicated tests for new interfaces and tracker. |

---

## 5. Verification Plan

* **Build Verification**:
  ```bash
  cmake -B build -S .
  cmake --build build
  ```
* **Test Verification**:
  ```bash
  cd build && ctest --output-on-failure
  ./tests/XML_Lib_Tests "[XML][ISource]"
  ./tests/XML_Lib_Tests "[XML][FileSource]"
  ./tests/XML_Lib_Tests "[XML][BufferSource]"
  ```
