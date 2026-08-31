# XML_Lib SOLID Architecture & Design Patterns Guide

## Overview

**XML_Lib** is architected to adhere strictly to modern **SOLID** object-oriented design principles. This guide explains the core architectural patterns, subsystem role interfaces, dependency injection mechanisms, and extension points for developers extending or contributing to `XML_Lib`.

---

## SOLID Design Principles in XML_Lib

### 1. Single Responsibility Principle (SRP)
Each class and module has a single, well-defined responsibility:
- **`XML_FileIO`**: Handles disk file reading, writing, path validation (`..` traversal defense), and BOM encoding detection.
- **`EntityStorage`**: Manages entity mapping tables and lookup.
- **`XXESecurityPolicy`**: Controls external entity resolution rules and disk file caching.
- **`EntityRecursionChecker`**: Detects circular entity definitions.
- **`EntityExpanderEngine`**: Manages entity reference macro substitution and string translation.
- **`StreamNormalizer`**: Handles CRLF-to-LF line ending normalization and UTF byte swapping.

### 2. Open/Closed Principle (OCP)
The library is open for extension without modifying existing core classes:
- **Node Serialization (`INodeSerializer`)**: [`Default_Stringify`](file:///home/robt/projects/XML_Lib/classes/include/implementation/stringify/INodeSerializer.hpp) uses an `unordered_map<Variant::Type, unique_ptr<INodeSerializer>>` strategy table. Registering a new serializer strategy requires zero modifications to `Default_Stringify`.
- **Pluggable Validator Registry (`IValidatorRegistry`)**: Custom schema validators (DTD, XSD, RELAX NG, Schematron) are registered dynamically via [`ValidatorRegistry`](file:///home/robt/projects/XML_Lib/classes/include/implementation/ValidatorRegistry.hpp).
- **XPath Navigation (`IXPathNodeAdapter`)**: XPath evaluation is decoupled from the DOM layout via strategy adapters.

### 3. Liskov Substitution Principle (LSP)
Subclasses and interface implementations strictly fulfill behavioral contracts:
- **Validating Parser Segregation**: `IParser` provides pure document parsing. Only parsers capable of schema validation implement `IValidatingParser`, eliminating runtime throw fallbacks.
- **Indentation Control**: Stateless stringifiers inherit `IStringify`; formatters supporting indentation settings implement `IIndentedStringify`.

### 4. Interface Segregation Principle (ISP)
Clients depend only on narrow role interfaces:
- **Input Stream Roles**: `ICharStream` (navigation), `ILocationTracker` (position tracking), `IRangeReader` (substring slices), `IResettableStream` (rewinding).
- **Output Destination Roles**: `ICharWriter` (single char append), `IStringWriter` (string/block append), `IResettableDestination` (clearing).
- **Entity Roles**: `IEntityRegistry` (lookup), `IEntityExpander` (translation), `ISecurityPolicyManager` (XXE policy).
- **Visitor Roles**: Narrow visitors in [`IVisitorRoles.hpp`](file:///home/robt/projects/XML_Lib/classes/include/interface/IVisitorRoles.hpp) (`IElementVisitor`, `ICommentVisitor`, `IContentVisitor`, etc.) dispatched via [`NodeVisitorAdapter`](file:///home/robt/projects/XML_Lib/classes/include/implementation/NodeVisitorAdapter.hpp).

### 5. Dependency Inversion Principle (DIP)
High-level modules depend on abstractions, not concrete details:
- `Default_Parser` depends on `IEntityRegistry` and `IEntityExpander` role interfaces.
- `XML` facade relies on abstract `IParser` and `IStringify` interfaces.
- Factory creation is decoupled through `SourceFactoryImpl`.

---

## Subsystem Architecture Diagrams

```
+-----------------------------------------------------------------------------------+
|                                  XML Facade                                       |
+-----------------------------------------------------------------------------------+
       |                                      |                               |
+------v-------+                      +-------v------+                +-------v------+
|    IParser   |                      |  IStringify  |                |  IValidator  |
+--------------+                      +--------------+                +--------------+
       |                                      |                               |
+------v-------+                      +-------v------+                +-------v------+
|Default_Parser|                      |Default_Stringify              |ValidatorReg. |
+--------------+                      +--------------+                +--------------+
       |                                      |                               |
       |                              +-------v------+                +-------v------+
       |                              |INodeSerializer                |ISchemaValid. |
       |                              +--------------+                +--------------+
       |
+------v----------------------------------------------------------------------------+
|                          Entity & Security Subsystem                              |
+-----------------------------------------------------------------------------------+
|  IEntityRegistry  |  IEntityExpander  |  ISecurityPolicyManager  |  RecursionChecker |
+-----------------------------------------------------------------------------------+
```

---

## Extension Cookbooks

### Cookbook 1: Writing a Narrow Role Visitor

To inspect only elements without implementing all 24 methods of `IAction`:

```cpp
#include "XML.hpp"
#include "interface/IVisitorRoles.hpp"

using namespace XML_Lib;

class ElementCounter final : public IElementVisitor
{
public:
  void onElement(const Node &node) override { ++count; }
  size_t count{ 0 };
};

XML xml{"<root><a><b/></a></root>"};
ElementCounter counter;
xml.traverse(counter); // count == 2
```

### Cookbook 2: Registering a Custom Node Serializer

To add a custom formatting strategy for `Comment` nodes:

```cpp
#include "implementation/stringify/Default_Stringify.hpp"

using namespace XML_Lib;

class CustomCommentSerializer final : public INodeSerializer
{
public:
  void serialize(
    const Node &xNode,
    IDestination &destination,
    [[maybe_unused]] unsigned long indent,
    [[maybe_unused]] const std::function<void(const Node &, IDestination &, unsigned long)> &recurse) const override
  {
    const auto &comment = NRef<Comment>(xNode);
    destination.add("/* " + comment.value() + " */");
  }
};

Default_Stringify stringifier;
stringifier.registerSerializer(Variant::Type::comment, std::make_unique<CustomCommentSerializer>());
```

---

## Summary

By maintaining clean role segregation and pluggable strategy maps, `XML_Lib` ensures maximum performance, long-term maintainability, zero-allocation memory options, and full extensibility across all XML processing workflows.
