# XML_Lib SOLID Architecture & Design Patterns Guide

## Overview

**XML_Lib** is architected to adhere strictly to modern **SOLID** object-oriented design principles. This guide explains the core architectural patterns, subsystem role interfaces, dependency injection mechanisms, and extension points for developers extending or contributing to `XML_Lib`.

---

## SOLID Design Principles in XML_Lib

### 1. Single Responsibility Principle (SRP)
Each class and module has a single, well-defined responsibility:
- **`XML_FileIO`**: Handles disk file reading, writing, path validation (`..` traversal defense), and BOM encoding detection.
- **`MMapSource`**: Manages kernel page-cache memory-mapped file access (POSIX and Windows) without intermediate heap copies.
- **`EntityStorage`**: Manages entity mapping tables and lookup.
- **`XXESecurityPolicy`**: Controls external entity resolution rules and disk file caching.
- **`OASIS_Catalog`**: Implements OASIS XML Catalogs 1.1 resolution for offline entity and schema mapping.
- **`EntityRecursionChecker`**: Detects circular entity definitions.
- **`EntityExpanderEngine`**: Manages entity reference macro substitution and string translation.
- **`StreamNormalizer`**: Handles CRLF-to-LF line ending normalization and UTF byte swapping.
- **`XMLReader`**: High-performance $O(1)$-memory forward-only pull parsing cursor.
- **`XMLWriter`**: High-performance forward-only push serialization direct to destination streams.

### 2. Open/Closed Principle (OCP)
The library is open for extension without modifying existing core classes:
- **Node Serialization (`INodeSerializer`)**: [`Default_Stringify`](file:///home/robt/projects/XML_Lib/classes/include/implementation/stringify/INodeSerializer.hpp) uses a strategy table mapping variant types to serializers. Registering a new serializer strategy requires zero modifications to `Default_Stringify`.
- **Pluggable Validator Registry (`IValidatorRegistry`)**: Custom schema validators (DTD, XSD, RELAX NG, Schematron) are registered dynamically via [`ValidatorRegistry`](file:///home/robt/projects/XML_Lib/classes/include/implementation/ValidatorRegistry.hpp).
- **Pluggable Entity Resolvers (`IEntityResolver`)**: Custom entity resolution mechanisms (such as database-backed or encrypted resolvers) can be injected via `ParseOptions::entityResolver` without changing parser internals.
- **XPath Navigation (`IXPathNodeAdapter`)**: XPath evaluation is decoupled from the DOM layout via strategy adapters.

### 3. Liskov Substitution Principle (LSP)
Subclasses and interface implementations strictly fulfill behavioral contracts:
- **Validating Parser Segregation**: `IParser` provides pure document parsing. Only parsers capable of schema validation implement `IValidatingParser`, eliminating runtime throw fallbacks.
- **Indentation Control**: Stateless stringifiers inherit `IStringify`; formatters supporting indentation settings implement `IIndentedStringify`.
- **Entity Resolvers**: Any implementation of `IEntityResolver` (including `OASIS_Catalog`) can seamlessly substitute for standard entity resolution.

### 4. Interface Segregation Principle (ISP)
Clients depend only on narrow role interfaces:
- **Input Stream Roles**: `ICharStream` (navigation), `ILocationTracker` (position tracking), `IRangeReader` (substring slices), `IResettableStream` (rewinding).
- **Output Destination Roles**: `ICharWriter` (single char append), `IStringWriter` (string/block append), `IResettableDestination` (clearing).
- **Entity Roles**: `IEntityRegistry` (lookup), `IEntityExpander` (translation), `ISecurityPolicyManager` (XXE policy), `IEntityResolver` (external resolution).
- **Visitor Roles**: Narrow visitors in [`IVisitorRoles.hpp`](file:///home/robt/projects/XML_Lib/classes/include/interface/IVisitorRoles.hpp) (`IElementVisitor`, `ICommentVisitor`, `IContentVisitor`, etc.) dispatched via [`NodeVisitorAdapter`](file:///home/robt/projects/XML_Lib/classes/include/implementation/NodeVisitorAdapter.hpp).

### 5. Dependency Inversion Principle (DIP)
High-level modules depend on abstractions, not concrete details:
- `Default_Parser` depends on `IEntityRegistry` and `IEntityExpander` role interfaces.
- `XML` facade relies on abstract `IParser`, `IStringify`, and `IValidator` interfaces.
- Factory creation is decoupled through `XML_Factories` and `SourceFactoryImpl`.

---

## Subsystem Architecture Diagram

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
|Default_Parser|                      |Default_String|                |ValidatorReg. |
+--------------+                      +--------------+                +--------------+
       |                                      |                               |
       |                              +-------v------+                +-------v------+
       |                              |INodeSerialize|                |XSD_Schema    |
       |                              +--------------+                +--------------+
       |
+------v----------------------------------------------------------------------------+
|                          Entity & Security Subsystem                              |
+-----------------------------------------------------------------------------------+
|  IEntityRegistry  |  IEntityExpander  |  ISecurityPolicyManager  |  IEntityResolver|
+-----------------------------------------------------------------------------------+
                                                                              |
                                                                      +-------v------+
                                                                      |OASIS_Catalog |
                                                                      +--------------+

+-----------------------------------------------------------------------------------+
|                             Streaming Subsystem                                   |
+-----------------------------------------------------------------------------------+
|            XMLReader (Pull Cursor)          |        XMLWriter (Push Serializer)  |
+-----------------------------------------------------------------------------------+
```

---

## Extension Cookbooks

### Cookbook 1: Writing a Narrow Role Visitor

To inspect only elements without implementing all 24 methods of `IAction`:

```cpp
#include <XML_Lib/XML.hpp>
#include <XML_Lib/interface/IVisitorRoles.hpp>
#include <iostream>

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
#include <XML_Lib/implementation/stringify/Default_Stringify.hpp>
#include <XML_Lib/XML_Node.hpp>

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
stringifier.registerSerializer<Comment>(std::make_unique<CustomCommentSerializer>());
```

### Cookbook 3: Implementing a Custom Entity Resolver

To implement a secure, in-memory or encrypted entity resolver:

```cpp
#include <XML_Lib/interface/IEntityResolver.hpp>
#include <XML_Lib/XML.hpp>
#include <optional>
#include <string>

using namespace XML_Lib;

class EncryptedEntityResolver final : public IEntityResolver
{
public:
  std::optional<std::string> resolve(
    const std::string_view &systemId,
    [[maybe_unused]] const std::string_view &publicId) override
  {
    if (systemId == "urn:secure:key") {
      return "<key>SECRET_VALUE</key>";
    }
    return std::nullopt; // Fallback or reject
  }
};

EncryptedEntityResolver resolver;
ParseOptions options;
options.entityResolver = &resolver;

XML xml;
xml.parse(source, options);
```

### Cookbook 4: Custom Streaming Transformation Pipeline

To stream, filter, and transform an XML feed without loading a DOM tree:

```cpp
#include <XML_Lib/XMLReader.hpp>
#include <XML_Lib/XMLWriter.hpp>

using namespace XML_Lib;

void anonymizeUserFeed(const std::string &inPath, const std::string &outPath)
{
  auto reader = XMLReader::fromFile(inPath);
  auto writer = XMLWriter::toFile(outPath);
  writer.setIndent(true, 2);

  writer.writeStartDocument();
  writer.writeStartElement("users");

  while (reader.read()) {
    if (reader.nodeType() == XMLReader::NodeType::ElementStart && reader.name() == "user") {
      writer.writeStartElement("user");
      writer.writeAttribute("id", reader.findAttribute("id").value_or("0"));

      int depth = reader.depth();
      while (reader.read() && reader.depth() > depth) {
        if (reader.nodeType() == XMLReader::NodeType::ElementStart) {
          if (reader.name() == "email") {
            writer.writeElement("email", "REDACTED");
            reader.skip();
          } else if (reader.name() == "name") {
            writer.writeElement("name", reader.readElementText());
          }
        }
      }
      writer.writeEndElement(); // </user>
    }
  }

  writer.writeEndElement(); // </users>
  writer.writeEndDocument();
}
```

---

## Summary

By maintaining clean role segregation and pluggable strategy maps, `XML_Lib` ensures maximum performance, long-term maintainability, zero-allocation memory options, and full extensibility across all XML processing workflows.
