# Master SOLID Refactoring Plan for XML_Lib

## 1. Executive Summary

This document provides a comprehensive analysis of the entire `XML_Lib` C++ codebase across all subsystems (`ISource`/`IDestination`, `IParser`, `IValidator`, `IStringify`, `IEntityMapper`, `IAction`, `XPath`, and the `XML`/`XML_Impl` facade) with respect to the **SOLID** principles of object-oriented software architecture:

- **S**ingle Responsibility Principle (SRP)
- **O**pen/Closed Principle (OCP)
- **L**iskov Substitution Principle (LSP)
- **I**nterface Segregation Principle (ISP)
- **D**ependency Inversion Principle (DIP)

It presents a concrete, step-by-step architectural roadmap to refactor every subsystem into complete alignment with SOLID principles, preserving existing performance guarantees and full backward compatibility where required.

---

## 2. Complete Subsystem SOLID Architectural Analysis

### 2.1 Input / Output Subsystem (`ISource`, `IDestination`, `XML_File`, Factories)

* **Current Deficiencies**:
  - **ISP Violation**: `IDestination` is a fat interface combining character appends (`add(Char)`), C-string appends (`add(const char*)`), string_view appends (`add(const string_view&)`), and state clearing (`clear()`).
  - **SRP Violation**: `XML_File` static class combines file existence checks, security validation (rejecting `..` and null bytes), file reading, encoding format detection (BOM parsing), and file writing into a single utility class.
  - **DIP Violation**: `XML_SourceFactory.hpp` is located in `classes/include/interface/` but directly includes implementation headers (`XML_BufferSource.hpp`, `XML_FileSource.hpp`).
* **Refactoring Strategy**:
  - Segregate `IDestination` into `ICharWriter` (single character output), `IStringWriter` (block/string output), and `IResettableDestination` (clear/reset).
  - Create pure interface `IDestinationFactory` in `interface/` and move implementation to `implementation/io/`.
  - Extract file I/O and path validation into a dedicated `IFileStorage` service abstraction.

---

### 2.2 Entity & Security Subsystem (`IEntityMapper`, `IEntityResolver`, `XML_EntityMapper`)

* **Current Deficiencies**:
  - **ISP Violation**: `IEntityMapper` declares 14 virtual methods spanning entity registration (`setInternal`, `setExternal`, `setNotation`), entity lookup (`getInternal`, `getExternal`, `getNotation`), existence checks (`isPresent`), XXE security policy management (`setExternalEntityPolicy`), text translation (`translate`, `map`), and cycle detection (`checkRecursiveEntity`, `checkForRecursion`). Consumers needing simple entity resolution are forced to depend on the entire 14-method interface.
  - **SRP Violation**: `XML_EntityMapper` acts as a storage repository, an entity substitution string parser/translator, an XXE security enforcement module, and a graph cycle detector.
  - **OCP Violation**: Support for new entity resolution strategies (e.g. OASIS XML Catalogs or remote HTTP resolvers) requires modifying `XML_EntityMapper`.
* **Refactoring Strategy**:
  - Segregate `IEntityMapper` into four focused interfaces:
    1. `IEntityRegistry`: Entity registration and lookup (`setInternal`, `getInternal`, `isPresent`).
    2. `IEntityExpander`: Entity reference substitution and string translation (`translate`, `map`).
    3. `IEntityRecursionChecker`: Recursion detection and graph validation (`checkForRecursion`).
    4. `ISecurityPolicyManager`: XXE policy enforcement and external entity resolution control.
  - Composition: Create `EntityService` which implements these segregated interfaces using modular sub-components (`EntityStorage`, `EntityExpander`, `RecursionChecker`, `SecurityPolicy`).

---

### 2.3 Parser Subsystem (`IParser`, `Default_Parser`, `XML_ParseHelpers`, `XML_Parse`)

* **Current Deficiencies**:
  - **LSP & ISP Violation**: `IParser` combines `parse(ISource&, const ParseOptions&)` with `canValidate()` and `validate(Node&)`. Parsers that do not support validation return `false` for `canValidate()` and throw errors if `validate()` is called, breaking Liskov Substitution.
  - **DIP Violation**: `Default_Parser` directly instantiates concrete dependencies (e.g. `XML_EntityMapper`) inside its constructor rather than accepting abstract `IEntityRegistry` / `IEntityExpander` interfaces.
  - **SRP & OCP Violation**: `Default_Parser` and procedurally grouped helpers in `XML_ParseHelpers.cpp` and `XML_Parse.cpp` handle prolog parsing, element/attribute parsing, entity expansion, character validation, and DTD validation within long procedural functions with tight coupling.
* **Refactoring Strategy**:
  - Segregate `IParser` (pure document parsing returning `Node`) from `IValidatingParser` / `IDocumentValidator`.
  - Inject `IEntityRegistry`, `IEntityExpander`, and `ISecurityPolicyManager` via constructor into `Default_Parser`.
  - Decompose `Default_Parser` into modular parsing stage handlers (`PrologParser`, `ElementParser`, `AttributeParser`, `DTDSectionParser`) implementing an `IXMLParseStage` interface.

---

### 2.4 Validator Subsystem (`IValidator`, `DTD_Validator`, `XSD_Validator`, `DTD_Impl`, `XSD_Impl`)

* **Current Deficiencies**:
  - **SRP Violation**: `DTD_Validator_Impl` and `XSD_Validator_Impl` combine DTD/XSD schema XML text parsing, schema internal data structure representation, constraint model creation, and validation execution into massive PImpl classes.
  - **OCP & DIP Violation**: `XML` and `XML_Impl` hardcode DTD and XSD validation through conditional compilation `#if defined(...)` macros and direct instantiation of `XSD_Validator` / `DTD_Validator` classes rather than using a pluggable validation registry or pipeline.
* **Refactoring Strategy**:
  - Separate Schema Parsing (`ISchemaParser<SchemaT>`) from Document Validation (`ISchemaValidator<SchemaT>`).
  - Introduce `IValidatorRegistry` allowing validators for DTD, XSD, RNG (RELAX NG), or Schematron to be registered dynamically without modifying `XML_Impl`.

---

### 2.5 Serializer Subsystem (`IStringify`, `Default_Stringify`)

* **Current Deficiencies**:
  - **LSP Violation**: `IStringify` provides default base implementations for `getIndent()` and `setIndent()`. Non-indented stringifiers inherit stateful indent methods that do nothing or return `0`.
  - **OCP Violation**: `Default_Stringify` formats nodes using direct node type checking and switch statements. Adding a custom node type or custom formatting rule requires editing `Default_Stringify`.
* **Refactoring Strategy**:
  - Extract `IIndentedStringify` for formatters that support indentation settings.
  - Introduce `INodeSerializer` strategy map in `Default_Stringify`, enabling open extension for new node types and custom serialization rules.

---

### 2.6 Node & Traversal Subsystem (`Node`, `XML_Variant`, `IAction`, `traverse`)

* **Current Deficiencies**:
  - **ISP Violation**: `IAction` defines 24 virtual methods (12 node types x 2 overloads for const and non-const). Clients wanting to process only elements or comments must depend on and inherit 24 methods.
  - **OCP Violation**: Tree traversal in `XML_Impl` relies on hardcoded node dispatching functions (`traverseNodes`). Adding new node types requires updating `IAction`, `traverseNodes`, and all visitor implementations.
* **Refactoring Strategy**:
  - Segregate `IAction` into narrow node-specific visitor interfaces (`IElementVisitor`, `ICommentVisitor`, `ITextVisitor`, `IDTDVisitor`, etc.).
  - Provide a composite adapter `NodeVisitorAdapter` implementing all interfaces with no-op defaults for backward compatibility.
  - Introduce `ITreeIterator` abstraction for configurable tree traversal strategies (pre-order, post-order, filtered).

---

### 2.7 XPath Subsystem (`XPath`, `XPath_Parser`, `XPath_Lexer`, `XPath_Evaluator`, `XPath_AST`)

* **Current Deficiencies**:
  - **DIP & SRP Violation**: `XPath_Evaluator` operates directly on concrete `Node` struct pointers and `XML_Variant` data structures, coupling XPath execution tightly to the DOM storage implementation.
  - **OCP Violation**: Axis evaluation (`XPath_AxisHelpers.cpp`) and function evaluation (`XPath_EvalHelpers.cpp`) use large `switch` statements over enum values. Adding a standard XPath 1.0 function or custom extension function requires modifying helper switch statements.
* **Refactoring Strategy**:
  - Introduce `IXPathNodeAdapter` abstraction so the XPath engine can query any tree structure (DOM, SAX event stream, custom tree).
  - Refactor XPath functions into an `IXPathFunction` strategy registry and axes into `IXPathAxisHandler` handlers.

---

### 2.8 Facade & Public API (`XML`, `XML_Impl`)

* **Current Deficiencies**:
  - **SRP Violation**: `XML` facade contains static file utility methods (`fromFile`, `toFile`, `getFileFormat`), document lifecycle management, DTD/XSD validation dispatching, serialization dispatching, and traversal dispatching.
  - **DIP Violation**: `XML` constructor accepts raw pointers `XML(IStringify *stringify = nullptr, IParser *parser = nullptr)` and takes ownership using `std::unique_ptr::reset()`, which risks memory leaks or confusion regarding ownership semantics.
* **Refactoring Strategy**:
  - Move static file utility methods to a dedicated `XML_FileIO` utility or `IFileStorage` service.
  - Provide overload for `XML` constructor using smart pointers (`std::shared_ptr<IParser>`, `std::shared_ptr<IStringify>`) or a fluent `XMLBuilder` pattern.

---

## 3. Subsystem Refactoring Matrix & Target Interfaces

| Subsystem | Principles Addressed | Key Target Interfaces & Classes | Primary Architectural Gain |
| :--- | :--- | :--- | :--- |
| **Input / Output** | ISP, DIP, SRP | `ICharStream`, `ILocationTracker`, `IRangeReader`, `IResettableStream`, `ICharWriter`, `IStringWriter`, `ISourceFactory`, `IFileStorage` | Decoupled streams, clean headers, extensible source sources & destinations |
| **Entity / Security** | ISP, SRP, DIP | `IEntityRegistry`, `IEntityExpander`, `IEntityRecursionChecker`, `ISecurityPolicyManager` | Modular entity storage, isolated XXE defense, zero fat interface pollution |
| **Parser** | LSP, ISP, DIP, OCP | `IParser`, `IValidatingParser`, `IXMLParseStage`, `PrologParser`, `ElementParser` | Segregated parsing vs validation, injected dependencies, open parse stages |
| **Validator** | SRP, OCP, DIP | `ISchemaParser`, `ISchemaValidator`, `IValidatorRegistry` | Extensible validator pipeline, separated schema parsing from validation |
| **Serializer** | LSP, OCP | `IStringify`, `IIndentedStringify`, `INodeSerializer` | Clean interface contracts, open node formatting strategy map |
| **Node / Traversal** | ISP, OCP | `IElementVisitor`, `ICommentVisitor`, `NodeVisitorAdapter`, `ITreeIterator` | Narrow visitor interfaces, customizable tree iteration |
| **XPath** | DIP, OCP | `IXPathNodeAdapter`, `IXPathFunction`, `IXPathAxisHandler` | Decoupled XPath engine, extensible function & axis registry |
| **Facade / API** | SRP, DIP | `XML`, `XML_Impl`, `XMLBuilder`, `XML_FileIO` | Focused facade responsibility, safe smart-pointer dependency injection |

---

## 4. Phased Implementation Roadmap

### Phase 1: Source & Destination I/O Subsystem (Completed / In Progress)
1. Complete `ISource` interface segregation (`ICharStream`, `ILocationTracker`, `IRangeReader`, `IResettableStream`).
2. Move `XML_SourceFactory` implementation out of public interface headers into `SourceFactoryImpl`.
3. Separate `BufferSource` and `FileSource` declarations and definitions into `.hpp` / `.cpp`.
4. Segregate `IDestination` into `ICharWriter` and `IStringWriter`.

### Phase 2: Entity & Security Subsystem Refactoring
1. Define `IEntityRegistry`, `IEntityExpander`, `IEntityRecursionChecker`, and `ISecurityPolicyManager` in `classes/include/interface/`.
2. Refactor `XML_EntityMapper` to implement these segregated interfaces via sub-components.
3. Update `Default_Parser` to accept `IEntityRegistry` and `IEntityExpander` via dependency injection.

### Phase 3: Parser & Serializer Subsystem Refactoring
1. Split `IParser` by removing `canValidate()` and `validate()` into `IValidatingParser`.
2. Refactor `Default_Parser` into modular stage parsers (`PrologParser`, `ElementParser`, etc.).
3. Split `IStringify` and `IIndentedStringify`. Implement `INodeSerializer` strategy dispatch in `Default_Stringify`.

### Phase 4: Validator Subsystem Refactoring
1. Separate `DTD_Validator` and `XSD_Validator` schema parsers from document validators.
2. Implement `IValidatorRegistry` in `XML_Impl` to allow pluggable validators.

### Phase 5: Visitor & Traversal Subsystem Refactoring
1. Add narrow visitor role interfaces (`IElementVisitor`, etc.) and `NodeVisitorAdapter`.
2. Refactor `XML::traverse()` to use `NodeVisitorAdapter`.

### Phase 6: XPath & Facade Subsystem Refactoring
1. Decouple `XPath_Evaluator` via `IXPathNodeAdapter`.
2. Move static file methods from `XML` facade to `XML_FileIO`.
3. Support smart pointer constructors and `XMLBuilder`.

---

## 5. Verification Strategy

1. **Unit Tests (Catch2)**:
   - Run unit tests for each subsystem (`./tests/XML_Lib_Tests`).
   - Validate ISource, IParser, IValidator, IStringify, IEntityMapper, IAction, XPath, and XML API.
2. **W3C XML & Conformance Testing**:
   - Run compliance suite (`XML_Lib_Tests_Compliance.cpp`).
3. **Security Fuzzing Corpus**:
   - Run security tests (`XML_Lib_Tests_Security.cpp`, `XML_Lib_Tests_Fuzz_Corpus.cpp`) to verify XXE defense and entity expansion recursion limits remain fully intact.
4. **Performance & Memory Benchmark**:
   - Run performance suite (`XML_Lib_Tests_Performance.cpp`) to ensure no regression in parsing speed or memory consumption.
