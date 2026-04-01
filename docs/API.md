# XML_Lib API Reference

This document provides a reference for the public API of the XML_Lib library.

## Overview
XML_Lib is a C++20 library for parsing, creating, and manipulating XML files. All classes live in the `XML_Lib` namespace.

## Main Classes

### `XML`
The top-level class. Provides parse, stringify, traverse, and validate entry points.

```cpp
XML xml;
xml.parse(ISource &source);         // Parse XML from a source (BufferSource, FileSource)
xml.stringify(IDestination &dest);  // Serialise back to XML
xml.validate();                     // Validate against DTD (if present)
xml.traverse(IAction &action);      // Walk the Node tree
Node &xml.prolog();                 // Root prolog Node
Node &xml.root();                   // Root element Node
Node &xml.declaration();            // XML declaration Node
Node &xml.dtd();                    // DTD Node (throws if none)
static std::string XML::version();
```

### `Node`
Owning wrapper around a `Variant`. The Node tree represents the entire document.

```cpp
bool node.isEmpty() const;
bool node.isNameable() const;
bool node.isIndexable() const;
std::string node.getContents() const;
const Node &node[int index] const;
const Node &node[std::string_view name] const;
std::vector<Node> &node.getChildren();
```

Use `isA<T>(node)` / `NRef<T>(node)` to test and cast a `Node` to any `Variant` subtype.

### `Element` / `Root` / `Self`
Represent element nodes. `Root` is the document root element, `Self` a self-closing element.

```cpp
const std::string &element.name() const;          // Full qualified name (e.g. "h:table")
std::string element.getPrefix() const;             // Namespace prefix (e.g. "h"), "" if none
std::string element.getLocalName() const;          // Local part (e.g. "table")
std::string element.getNamespaceURI() const;       // Resolved URI from in-scope namespaces

// Attributes
bool element.hasAttribute(std::string_view name) const;
void element.addAttribute(std::string_view name, const XMLValue &value) const;
const std::vector<XMLAttribute> &element.getAttributes() const;
const XMLAttribute &element[std::string_view attrName] const;

// Namespaces (accumulated in-scope from root to this element)
bool element.hasNameSpace(std::string_view prefix) const;
void element.addNameSpace(std::string_view prefix, const XMLValue &value) const;
const XMLAttribute &element.getNameSpace(std::string_view prefix) const;
const std::vector<XMLAttribute> &element.getNameSpaces() const;
```

### `XMLAttribute`
Represents a name/value attribute.

```cpp
const std::string &attr.getName() const;
const std::string &attr.getUnparsed() const;
const std::string &attr.getParsed() const;
char attr.getQuote() const;

static bool XMLAttribute::contains(const std::vector<XMLAttribute> &, std::string_view name);
static XMLAttribute &XMLAttribute::find(std::vector<XMLAttribute> &, std::string_view name);
```

### `XMLValue`
Holds the raw (unparsed) and resolved (parsed) text of an attribute or entity value.

```cpp
XMLValue(std::string_view unparsed, std::string_view parsed = "", char quote = '"');
const std::string &value.getUnparsed() const;
const std::string &value.getParsed() const;
bool value.isReference() const;
bool value.isEntityReference() const;
bool value.isCharacterReference() const;
```

### `ISource` / `IDestination`
Source and destination abstractions for I/O.

```cpp
BufferSource source{std::string xmlText};   // Parse from string
FileSource   source{std::string filePath};  // Parse from file
BufferDestination dest;                     // Stringify to string (dest.toString())
FileDestination   dest{path, format};       // Stringify to file
```

## Namespace Support
XML_Lib implements the [W3C XML Namespaces](https://www.w3.org/TR/xml-names/) recommendation:

- `xmlns="uri"` declares a **default namespace**; accessible via `getNameSpace(":")`.
- `xmlns:prefix="uri"` declares a **prefixed namespace**; accessible via `getNameSpace("prefix")`.
- Namespace declarations scope to child elements — `getNameSpaces()` on any element returns all in-scope declarations (from root down to the element).
- Elements and attributes with undeclared prefixes cause a `SyntaxError` to be thrown.
- Duplicate namespace declarations on the same element throw a `SyntaxError`.
- `getPrefix()`, `getLocalName()`, `getNamespaceURI()` provide QName decomposition.

### Namespace Example
```cpp
XML xml;
BufferSource source{
  "<root xmlns:h=\"http://www.w3.org/TR/html4/\">"
  "<h:table h:border=\"1\"><h:tr><h:td>Data</h:td></h:tr></h:table>"
  "</root>"
};
xml.parse(source);
auto &root = NRef<Element>(xml.root());
auto &table = root[0];
std::cout << table.name();             // "h:table"
std::cout << table.getPrefix();        // "h"
std::cout << table.getLocalName();     // "table"
std::cout << table.getNamespaceURI();  // "http://www.w3.org/TR/html4/"
std::cout << table.getNameSpace("h").getParsed();  // "http://www.w3.org/TR/html4/"
```

## Error Handling
All errors throw exceptions derived from `std::runtime_error`:
- `SyntaxError` — malformed XML or namespace violations.
- `Node::Error` — invalid node access.
- `XMLAttribute::Error` — attribute not found.
- `FileSource::Error` / `BufferSource::Error` — I/O errors.

## Example Usage
See the [Guide](Guide.md) and the `examples/` directory for practical usage scenarios.

---
*For detailed implementation notes, refer to the source in `classes/include/` and `classes/source/`.*

