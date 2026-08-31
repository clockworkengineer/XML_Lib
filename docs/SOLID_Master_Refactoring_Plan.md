# Master SOLID Refactoring Plan & File-by-File Specification for XML_Lib

## 1. Executive Summary

This document provides a comprehensive, file-by-file architectural analysis and concrete refactoring specification for all C++ header and source files in `XML_Lib` (`classes/`, `examples/`, `tests/`). It outlines exact structural transformations required to bring every component into complete alignment with **SOLID** software design principles:

- **S**ingle Responsibility Principle (SRP)
- **O**pen/Closed Principle (OCP)
- **L**iskov Substitution Principle (LSP)
- **I**nterface Segregation Principle (ISP)
- **D**ependency Inversion Principle (DIP)

---

## 2. File-by-File Refactoring Specification

### 2.1 Interface Layer (`classes/include/interface/`)

#### [`ISource.hpp`](file:///home/robt/projects/XML_Lib/classes/include/interface/ISource.hpp)
- **Current State**: Refactored into `ICharStream`, `ILocationTracker`, `IRangeReader`, `IResettableStream`, composite `ISource`.
- **SOLID Status**: ISP compliant.
- **Action**: Preserve composite interface for backward compatibility; mandate narrow role interfaces in internal consumer signatures.

#### [`IDestination.hpp`](file:///home/robt/projects/XML_Lib/classes/include/interface/IDestination.hpp)
- **Current State**: Refactored into `ICharWriter`, `IStringWriter`, `IResettableDestination`, composite `IDestination`.
- **SOLID Status**: ISP compliant.
- **Action**: Maintain `using` overload resolution declarations.

#### [`IEntityRegistry.hpp`](file:///home/robt/projects/XML_Lib/classes/include/interface/IEntityRegistry.hpp) [NEW]
- **Current State**: Role interface for entity registration and lookup (`setInternal`, `getExternal`, `isPresent`).
- **SOLID Alignment**: ISP / SRP.

#### [`IEntityExpander.hpp`](file:///home/robt/projects/XML_Lib/classes/include/interface/IEntityExpander.hpp) [NEW]
- **Current State**: Role interface for entity expansion, string translation, and cycle detection (`translate`, `map`, `checkForRecursion`).
- **SOLID Alignment**: ISP / SRP.

#### [`ISecurityPolicyManager.hpp`](file:///home/robt/projects/XML_Lib/classes/include/interface/ISecurityPolicyManager.hpp) [NEW]
- **Current State**: Role interface for XXE policy management (`setExternalEntityPolicy`).
- **SOLID Alignment**: ISP / SRP.

#### [`IEntityMapper.hpp`](file:///home/robt/projects/XML_Lib/classes/include/interface/IEntityMapper.hpp)
- **Current State**: Refactored into pure composite interface extending `IEntityRegistry`, `IEntityExpander`, and `ISecurityPolicyManager`.
- **SOLID Alignment**: ISP compliant.

#### [`IParser.hpp`](file:///home/robt/projects/XML_Lib/classes/include/interface/IParser.hpp)
- **Current State**: Segregated pure `IParser` (`parse`) from `IValidatingParser` (`canValidate`, `validate`).
- **SOLID Alignment**: LSP / ISP compliant.

#### [`IStringify.hpp`](file:///home/robt/projects/XML_Lib/classes/include/interface/IStringify.hpp)
- **Current State**: Added `IIndentedStringify` role interface.
- **SOLID Alignment**: LSP compliant.

#### [`ISchemaParser.hpp`](file:///home/robt/projects/XML_Lib/classes/include/interface/ISchemaParser.hpp) [NEW]
- **Current State**: Template role interface for schema parsing.
- **SOLID Alignment**: SRP / OCP.

#### [`ISchemaValidator.hpp`](file:///home/robt/projects/XML_Lib/classes/include/interface/ISchemaValidator.hpp) [NEW]
- **Current State**: Role interface for schema document validation.
- **SOLID Alignment**: SRP / OCP.

#### [`IValidatorRegistry.hpp`](file:///home/robt/projects/XML_Lib/classes/include/interface/IValidatorRegistry.hpp) [NEW]
- **Current State**: Pluggable validator registry interface.
- **SOLID Alignment**: OCP / DIP.

#### [`IVisitorRoles.hpp`](file:///home/robt/projects/XML_Lib/classes/include/interface/IVisitorRoles.hpp) [NEW]
- **Current State**: Segregated node visitor role interfaces (`IElementVisitor`, `ICommentVisitor`, etc.).
- **SOLID Alignment**: ISP compliant.

#### [`IAction.hpp`](file:///home/robt/projects/XML_Lib/classes/include/interface/IAction.hpp)
- **Current State**: Composite visitor interface inheriting all role interfaces in `IVisitorRoles.hpp`.
- **SOLID Alignment**: ISP compliant.

---

### 2.2 Implementation Headers & Source Layer (`classes/include/implementation/` & `classes/source/implementation/`)

#### [`Default_Parser.hpp`](file:///home/robt/projects/XML_Lib/classes/include/implementation/parser/Default_Parser.hpp) & [`Default_Parser.cpp`](file:///home/robt/projects/XML_Lib/classes/source/implementation/xml/parser/Default_Parser.cpp)
- **Deficiency**: Constructor binds to `IEntityMapper&` rather than segregated `IEntityRegistry&` and `IEntityExpander&`. Monolithic parse methods mix prolog, element, attribute, and DTD parsing.
- **Refactoring**:
  1. Inject `IEntityRegistry&` and `IEntityExpander&` via constructor (DIP).
  2. Split monolithic parsing into `IXMLParseStage` pipeline (`PrologParseStage`, `ElementParseStage`, `AttributeParseStage`, `DTDParseStage`).

#### [`XML_EntityMapper.cpp`](file:///home/robt/projects/XML_Lib/classes/source/implementation/entity/XML_EntityMapper.cpp)
- **Deficiency**: Single class handles entity map storage, text regex translation, cycle graph validation, and XXE resolver lookup.
- **Refactoring**: Split into composite class delegating to `EntityStorage`, `EntityExpanderEngine`, `RecursionChecker`, and `XXESecurityPolicy`.

#### [`DTD_Validator_Impl.cpp`](file:///home/robt/projects/XML_Lib/classes/source/implementation/dtd/DTD_Validator_Impl.cpp) & associated split files
- **Deficiency**: `DTD_Validator_Impl` parses DTD text, constructs DTD AST, validates elements/attributes, and stringifies DTD text.
- **Refactoring**: Separate `DTD_SchemaParser` (implementing `ISchemaParser<DTD_Schema>`) from `DTD_SchemaValidator` (implementing `ISchemaValidator`).

#### [`XSD_Validator_Impl.cpp`](file:///home/robt/projects/XML_Lib/classes/source/implementation/xsd/XSD_Validator_Impl.cpp) & associated split files
- **Deficiency**: `XSD_Validator_Impl` combines XSD XML parsing, schema node mapping, type constraint evaluation, and validation execution.
- **Refactoring**: Separate `XSD_SchemaParser` from `XSD_SchemaValidator`.

#### [`Default_Stringify.hpp`](file:///home/robt/projects/XML_Lib/classes/include/implementation/stringify/Default_Stringify.hpp)
- **Deficiency**: Formatting uses direct type checks / switch statements for node kinds, violating OCP.
- **Refactoring**: Introduce `INodeSerializer` strategy map for dynamic node formatting.

#### [`XPath_Evaluator.cpp`](file:///home/robt/projects/XML_Lib/classes/source/implementation/xpath/XPath_Evaluator.cpp), [`XPath_AxisHelpers.cpp`](file:///home/robt/projects/XML_Lib/classes/source/implementation/xpath/XPath_AxisHelpers.cpp), [`XPath_EvalHelpers.cpp`](file:///home/robt/projects/XML_Lib/classes/source/implementation/xpath/XPath_EvalHelpers.cpp)
- **Deficiency**: Tightly coupled to concrete `Node` structure and uses enum `switch` statements for axis/function evaluation.
- **Refactoring**:
  1. Introduce `IXPathNodeAdapter` abstraction.
  2. Implement `IXPathFunction` strategy registry and `IXPathAxisHandler` handlers.

#### [`XML.cpp`](file:///home/robt/projects/XML_Lib/classes/source/XML.cpp) & [`XML_Impl.cpp`](file:///home/robt/projects/XML_Lib/classes/source/implementation/xml/XML_Impl.cpp)
- **Deficiency**: `XML` static methods (`fromFile`, `toFile`, `getFileFormat`) violate SRP; constructor raw pointers violate DIP.
- **Refactoring**: Move file methods to `XML_FileIO`; introduce smart-pointer constructor overloads and `XMLBuilder`.

---

## 3. Subsystem Refactoring Matrix

| Subsystem | Principles Addressed | Target File / Class Artifacts | Key Architectural Result |
| :--- | :--- | :--- | :--- |
| **I/O** | ISP, DIP, SRP | `ISource.hpp`, `IDestination.hpp`, `IFileStorage.hpp`, `SourceFactoryImpl` | Decoupled streams, clean role interfaces |
| **Entity / Security** | ISP, SRP, DIP | `IEntityRegistry`, `IEntityExpander`, `ISecurityPolicyManager`, `EntityService` | Modular storage, isolated XXE defense |
| **Parser** | LSP, ISP, DIP, OCP | `IParser`, `IValidatingParser`, `IXMLParseStage`, `PrologParseStage` | Clean parsing pipeline, injected mappers |
| **Validator** | SRP, OCP, DIP | `ISchemaParser`, `ISchemaValidator`, `IValidatorRegistry` | Extensible schema validation pipeline |
| **Serializer** | LSP, OCP | `IStringify`, `IIndentedStringify`, `INodeSerializer` | Strategy-driven node formatting |
| **Traversal** | ISP, OCP | `IVisitorRoles.hpp`, `IAction.hpp`, `NodeVisitorAdapter` | Narrow visitor interfaces, backward compatible |
| **XPath** | DIP, OCP | `IXPathNodeAdapter`, `IXPathFunction`, `IXPathAxisHandler` | Adaptable node model, extensible functions |
| **Facade / API** | SRP, DIP | `XML`, `XML_Impl`, `XML_FileIO`, `XMLBuilder` | High-level facade with smart-pointer injection |

---

## 4. Verification Plan

### Automated Test Suite Execution
1. **Full CMake Rebuild & Test**:
   ```bash
   cmake -B build -S .
   cmake --build build --config Debug
   ./build/tests/XML_Lib_Unit_Tests
   ```
2. **Targeted Subsystem Verification**:
   - `XML_Lib_Tests_ISource`: I/O stream segregation & LSP invariants.
   - `XML_Lib_Tests_Entity_Mapper`: Entity registry, expander, and security policy checks.
   - `XML_Lib_Tests_XML`: Parser & Serializer integration.
   - `XML_Lib_Tests_DTD_*` & `XML_Lib_Tests_XSD`: Pluggable schema validation engines.
   - `XML_Lib_Tests_XPath`: XPath node adapter & function strategy verification.
   - `XML_Lib_Tests_Security` & `XML_Lib_Tests_Compliance`: W3C XML conformance & XXE defense.
