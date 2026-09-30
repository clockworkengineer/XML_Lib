# XML_Lib API Reference

This document provides a comprehensive technical reference for the public API of the XML_Lib library.

All classes, types, and functions reside in the `XML_Lib` namespace.

---

## Standard & Architecture

- **Language Standard**: ISO/IEC 14882:2023 (C++23)
- **Design Philosophy**: 100% SOLID architecture, zero external runtime dependencies, $O(1)$-memory streaming options, and RAII resource management.

---

## Table of Contents

1. [Main DOM Class (`XML`)](#1-main-dom-class-xml)
2. [Parse Options (`ParseOptions`)](#2-parse-options-parseoptions)
3. [Non-Throwing Error Detail (`XML_Error`)](#3-non-throwing-error-detail-xml_error)
4. [Streaming Pull Parser (`XMLReader`)](#4-streaming-pull-parser-xmlreader)
5. [Streaming Push Serializer (`XMLWriter`)](#5-streaming-push-serializer-xmlwriter)
6. [Zero-Copy Memory-Mapped Stream (`MMapSource`)](#6-zero-copy-memory-mapped-stream-mmapsource)
7. [OASIS XML Catalogs 1.1 (`OASIS_Catalog`)](#7-oasis-xml-catalogs-11-oasis_catalog)
8. [Pre-Compiled XSD Schema (`XSD_Schema`)](#8-pre-compiled-xsd-schema-xsd_schema)
9. [Pre-Compiled XPath AST (`XPathExpression`) & Evaluator (`XPath`)](#9-pre-compiled-xpath-ast-xpathexpression--evaluator-xpath)
10. [DOM Tree Classes (`Node`, `Element`, `XMLAttribute`, `XMLValue`)](#10-dom-tree-classes-node-element-xmlattribute-xmlvalue)
11. [C++20 Ranges & Views (`XML_Ranges.hpp`)](#11-c20-ranges--views-xml_rangeshpp)
12. [C++20 Concepts (`XML_Concepts.hpp`)](#12-c20-concepts-xml_conceptshpp)
13. [Serialization & Formatting (`IStringify`, `StringifyOptions`)](#13-serialization--formatting-istringify-stringifyoptions)
14. [SOLID Role Interfaces](#14-solid-role-interfaces)
15. [Exceptions and Error Types](#15-exceptions-and-error-types)

---

## 1. Main DOM Class (`XML`)

Defined in `<XML_Lib/XML.hpp>`. The top-level facade representing an in-memory XML document tree. Uses the PImpl idiom to provide value-like semantics and complete encapsulation.

### Types & Enums
```cpp
enum class Format : uint8_t { 
    utf8 = 0, 
    utf8BOM, 
    utf16BE, 
    utf16LE, 
    utf32BE, 
    utf32LE 
};
```

### Constructors & Assignment
```cpp
explicit XML(IStringify *stringify = nullptr, IParser *parser = nullptr);
explicit XML(std::unique_ptr<IStringify> stringify, std::unique_ptr<IParser> parser = nullptr);
explicit XML(const std::string_view &xmlString);

XML(XML &&other) noexcept;
XML &operator=(XML &&other) noexcept;

XML(const XML &) = delete;
XML &operator=(const XML &) = delete;

XML &operator=(const std::string_view &xmlString);
~XML() noexcept;
```

### Parsing Methods (Throwing)
```cpp
void parse(ISource &source, const ParseOptions &options = {}) const;
void parse(ISource &&source, const ParseOptions &options = {}) const;
void parse(const char *xmlString, const ParseOptions &options = {}) const;
void parse(const std::string_view &xmlString, const ParseOptions &options = {}) const;
void parse(const std::filesystem::path &filePath, const ParseOptions &options = {}) const;
```

### Parsing Methods (Non-Throwing C++23 `std::expected`)
```cpp
[[nodiscard]] static std::expected<std::unique_ptr<XML>, XML_Error> 
parseExpected(ISource &source, const ParseOptions &options = {}) noexcept;

[[nodiscard]] static std::expected<std::unique_ptr<XML>, XML_Error> 
parseExpected(ISource &&source, const ParseOptions &options = {}) noexcept;

[[nodiscard]] static std::expected<std::unique_ptr<XML>, XML_Error> 
parseExpected(std::string_view xmlString, const ParseOptions &options = {}) noexcept;

[[nodiscard]] static std::expected<std::unique_ptr<XML>, XML_Error> 
parseExpected(const std::string &xmlString, const ParseOptions &options = {}) noexcept;

[[nodiscard]] static std::expected<std::unique_ptr<XML>, XML_Error> 
parseExpected(const std::filesystem::path &filePath, const ParseOptions &options = {}) noexcept;
```

### Document Tree Accessors
```cpp
[[nodiscard]] Node &prolog() const;       // Everything before root (declaration, comments, PIs)
[[nodiscard]] Node &declaration() const;  // XML declaration (<?xml ...?>)
[[nodiscard]] Node &root() const;         // Document root element Node
[[nodiscard]] Node &dtd() const;          // DTD Node (throws if none present)
```

### Validation Methods
```cpp
// DTD Validation (requires XML_LIB_ENABLE_DTD)
void validate() const;
[[nodiscard]] std::expected<void, std::string> validateExpected() const noexcept;

// XSD Validation (requires XML_LIB_ENABLE_XSD)
void validate(const std::string_view &xsdSource) const;
void validate(const XSD_Schema &schema) const;
[[nodiscard]] std::expected<void, std::string> validateExpected(const std::string_view &xsdSource) const noexcept;
[[nodiscard]] std::expected<void, std::string> validateExpected(const XSD_Schema &schema) const noexcept;

// Custom Validator (OCP)
void registerValidator(const std::string_view &schemaType, std::unique_ptr<IValidator> validator) const;
void validate(const std::string_view &schemaType, const std::string_view &schemaSource) const;
```

### XPath Query Methods
```cpp
// Evaluate string expression
[[nodiscard]] std::vector<const Node *> xpath(std::string_view expression) const;

// Evaluate pre-compiled expression
[[nodiscard]] std::vector<const Node *> xpath(const XPathExpression &expression) const;

// Custom query engine injection (OCP)
void setXPathEngine(std::unique_ptr<IXPathEngine> engine) const;
```

### Serialization & Traversal
```cpp
void stringify(IDestination &destination) const;
void stringify(IDestination &&destination) const;
[[nodiscard]] std::string stringify() const;
void stringify(const std::filesystem::path &filePath, Format format = Format::utf8) const;

void traverse(IAction &action);
void traverse(IAction &action) const;
```

### Static Utility Services
```cpp
[[nodiscard]] static std::string version();
[[nodiscard]] static std::string fromFile(const std::filesystem::path &filePath);
static void toFile(const std::filesystem::path &filePath, const std::string_view &xmlString, Format format = Format::utf8);
[[nodiscard]] static Format getFileFormat(const std::string_view &fileName);
```

---

## 2. Parse Options (`ParseOptions`)

Defined in `<XML_Lib/XML.hpp>`. Configures security constraints, resource limits, and XML specification conformance dialects:

```cpp
struct ParseOptions {
    std::size_t maxXmlSize              = XML_LIB_MAX_XML_SIZE;   // Max input size in bytes (default: 100 MiB)
    std::size_t maxEntityExpansionDepth = 512;                      // Recursion limit (Billion Laughs defense)
    std::size_t maxNestingDepth         = 1000;                     // Max element nesting depth
    std::size_t maxElementCount         = 1000000;                  // Max elements across document
    std::size_t maxAttributeCount       = 10000;                    // Max attributes per element
    std::size_t maxTotalAttributeCount  = XML_LIB_MAX_TOTAL_ATTRIBUTES; // Max total attributes (default: 1,000,000)
    std::size_t maxTextNodeSize         = 1024 * 1024;              // Max text node size (1 MiB)
    bool        allowExternalEntities   = false;                    // When false, rejects external DTDs/entities (XXE defense)
    IEntityResolver *entityResolver     = nullptr;                  // Custom entity resolver (overrides allowExternalEntities)
    bool        strictNamespaces        = false;                    // Enforce XML Namespaces 1.0 (forbids xmlns:prefix="")
    bool        enableNamespaces        = true;                     // Enable/disable namespace validation
    bool        allowFuture1xVersions   = false;                    // Accept XML 1.x version headers (XML 1.0 5th Ed)
    bool        firstEntityDeclarationBinding = false;              // First entity declaration wins (XML 1.0 §4.2)
};
```

---

## 3. Non-Throwing Error Detail (`XML_Error`)

Defined in `<XML_Lib/XML_Expected.hpp>`:

```cpp
struct XML_Error {
    std::string message;
    long line{ 0 };
    long column{ 0 };
};
```

Returned as the unexpected type in `std::expected<T, XML_Error>`.

---

## 4. Streaming Pull Parser (`XMLReader`)

Defined in `<XML_Lib/XMLReader.hpp>`. A high-performance, forward-only streaming cursor parser operating with $O(1)$ memory complexity (< 10 MB RAM) across arbitrarily large files.

### Token Types
```cpp
enum class NodeType {
    None,                   // Initial state
    Declaration,            // <?xml ... ?>
    ElementStart,           // <tag attr="val">
    ElementEnd,             // </tag> or closing of <tag/>
    Text,                   // Character data
    CDATA,                  // <![CDATA[...]]>
    Comment,                // <!-- ... -->
    ProcessingInstruction,  // <?target data?>
    DTD,                    // <!DOCTYPE ...>
    Whitespace,             // Ignorable inter-element whitespace
    EndDocument             // End of document stream
};
```

### Constructors & Factories
```cpp
explicit XMLReader(ISource &source);
explicit XMLReader(std::string_view xmlString);
static XMLReader fromFile(const std::filesystem::path &filePath);

XMLReader(XMLReader &&) noexcept;
XMLReader &operator=(XMLReader &&) noexcept;
~XMLReader();
```

### Cursor Navigation
```cpp
bool read();                                          // Advance cursor; returns false at EOF
std::expected<bool, XML_Error> readExpected() noexcept; // Advance without exceptions
bool readToNextElement();                              // Advance to next ElementStart
std::string readElementText();                        // Read all text inside element to ElementEnd
void skip();                                          // Skip current element's entire subtree
```

### Node Inspection
```cpp
[[nodiscard]] NodeType nodeType() const noexcept;
[[nodiscard]] static constexpr std::string_view nodeTypeToString(NodeType type) noexcept;
[[nodiscard]] std::string_view name() const noexcept;
[[nodiscard]] std::string_view value() const noexcept;
[[nodiscard]] bool isEmptyElement() const noexcept;
[[nodiscard]] int depth() const noexcept;
[[nodiscard]] std::pair<long, long> getPosition() const;
void setSkipWhitespace(bool skip) noexcept;
```

### Attribute Inspection
```cpp
[[nodiscard]] std::size_t attributeCount() const noexcept;
[[nodiscard]] bool hasAttribute(std::string_view attrName) const noexcept;
[[nodiscard]] std::string_view getAttribute(std::string_view attrName) const noexcept;
[[nodiscard]] std::string_view getAttribute(std::size_t index) const;
[[nodiscard]] std::string_view getAttributeName(std::size_t index) const;
[[nodiscard]] std::optional<std::string_view> findAttribute(std::string_view attrName) const noexcept;
[[nodiscard]] const std::vector<std::pair<std::string, std::string>> &attributes() const noexcept;
```

---

## 5. Streaming Push Serializer (`XMLWriter`)

Defined in `<XML_Lib/XMLWriter.hpp>`. High-performance streaming serializer emitting directly to an `IDestination` or file with well-formedness tag verification and automatic escaping.

### Constructors & Factories
```cpp
explicit XMLWriter(IDestination &destination);
XMLWriter();                                          // In-memory buffer constructor
static XMLWriter toFile(const std::filesystem::path &filePath);

XMLWriter(XMLWriter &&) noexcept;
XMLWriter &operator=(XMLWriter &&) noexcept;
~XMLWriter();
```

### Configuration
```cpp
void setIndent(bool enable, int spaces = 2) noexcept;
void setOmitXmlDeclaration(bool omit) noexcept;
```

### Document & Tag Lifecycle
```cpp
void writeStartDocument(std::string_view version = "1.0", 
                        std::string_view encoding = "UTF-8", 
                        bool standalone = false);
void writeEndDocument();

void writeStartElement(std::string_view name);
void writeEndElement();
void writeEmptyElement(std::string_view name);
void writeElement(std::string_view name, std::string_view textContent);
```

### Attributes & Content
```cpp
void writeAttribute(std::string_view name, std::string_view value);
void writeCharacters(std::string_view text);          // Auto-escapes &, <, >, ", '
void writeComment(std::string_view comment);          // <!-- comment -->
void writeCDATA(std::string_view cdata);              // <![CDATA[ cdata ]]>
void writeProcessingInstruction(std::string_view target, std::string_view data = "");
void writeRaw(std::string_view rawMarkup);            // Raw unescaped output
```

### Flushing & Buffer Retrieval
```cpp
void flush();
[[nodiscard]] std::string result() const;              // Valid when constructed with default buffer
```

---

## 6. Zero-Copy Memory-Mapped Stream (`MMapSource`)

Defined in `<XML_Lib/XML_MMapSource.hpp>` and included via `<XML_Lib/XML_Sources.hpp>`. Maps files directly into userspace address space via the OS page cache without intermediate heap allocation.

```cpp
class MMapSource final : public ISource {
public:
    static constexpr std::size_t kMaxSourceBytes = XML_LIB_MAX_XML_SIZE;

    explicit MMapSource(const std::string_view &filePath, 
                        std::size_t maxSourceBytes = kMaxSourceBytes);

    // Implements all ISource role interfaces:
    // ICharStream, ILocationTracker, IRangeReader, IResettableStream
};
```

---

## 7. OASIS XML Catalogs 1.1 (`OASIS_Catalog`)

Defined in `<XML_Lib/OASIS_Catalog.hpp>`. Implements `IEntityResolver` according to the OASIS XML Catalogs V1.1 standard for offline entity and schema resolution.

### Constructors & Factories
```cpp
OASIS_Catalog() = default;
explicit OASIS_Catalog(std::string_view catalogXml, const std::filesystem::path &basePath = {});
static OASIS_Catalog fromFile(const std::filesystem::path &catalogFilePath);
```

### Catalog Loading & Mapping Configuration
```cpp
void loadCatalog(std::string_view catalogXml, const std::filesystem::path &basePath = {});
void loadCatalogFile(const std::filesystem::path &catalogFilePath);

void addSystemMapping(std::string_view systemId, std::string_view uriOrPath);
void addPublicMapping(std::string_view publicId, std::string_view uriOrPath);
void addRewriteSystem(std::string_view systemIdStartString, std::string_view rewritePrefix);
void addRewriteURI(std::string_view uriStartString, std::string_view rewritePrefix);
void addMemoryContent(std::string_view identifier, std::string_view content);
```

### Resolution (Contract Implementation)
```cpp
[[nodiscard]] std::optional<std::string> 
resolve(const std::string_view &systemId, const std::string_view &publicId) override;

[[nodiscard]] std::optional<std::string> resolveSystem(const std::string_view &systemId) const;
[[nodiscard]] std::optional<std::string> resolvePublic(const std::string_view &publicId, const std::string_view &systemId = {}) const;
[[nodiscard]] std::optional<std::string> resolveURI(const std::string_view &uri) const;
```

---

## 8. Pre-Compiled XSD Schema (`XSD_Schema`)

Defined in `<XML_Lib/XSD_Schema.hpp>` (under `XML_LIB_ENABLE_XSD`). Pre-parses and compiles an XSD schema definition into an immutable, thread-safe structure.

```cpp
class XSD_Schema {
public:
    explicit XSD_Schema(ISource &source);
    explicit XSD_Schema(std::string_view schemaSource);
    static XSD_Schema fromFile(const std::filesystem::path &filePath);

    XSD_Schema(const XSD_Schema &) = default;
    XSD_Schema &operator=(const XSD_Schema &) = default;
    XSD_Schema(XSD_Schema &&) noexcept = default;
    XSD_Schema &operator=(XSD_Schema &&) noexcept = default;

    void validate(const Node &xNode) const; // Throws XSD_Validator::Error
    [[nodiscard]] const std::shared_ptr<const XSD_SchemaDefinition> &definition() const noexcept;
};
```

---

## 9. Pre-Compiled XPath AST (`XPathExpression`) & Evaluator (`XPath`)

Defined in `<XML_Lib/XPath.hpp>` (under `XML_LIB_ENABLE_XPATH`).

### Pre-Compiled Expression (`XPathExpression`)
```cpp
class XPathExpression {
public:
    explicit XPathExpression(std::string_view expression);
    
    XPathExpression(const XPathExpression &);
    XPathExpression &operator=(const XPathExpression &);
    XPathExpression(XPathExpression &&) noexcept;
    XPathExpression &operator=(XPathExpression &&) noexcept;

    [[nodiscard]] std::string_view expression() const noexcept;
    [[nodiscard]] std::vector<const Node *> evaluate(const Node &contextNode) const;
    [[nodiscard]] std::string evaluateString(const Node &contextNode) const;
    [[nodiscard]] bool evaluateBool(const Node &contextNode) const;
    [[nodiscard]] double evaluateNumber(const Node &contextNode) const;
};
```

### Context Evaluator (`XPath`)
```cpp
class XPath {
public:
    explicit XPath(const Node &root);

    // Direct string evaluation
    [[nodiscard]] std::vector<const Node *> evaluate(std::string_view expression) const;
    [[nodiscard]] std::string evaluateString(std::string_view expression) const;
    [[nodiscard]] bool evaluateBool(std::string_view expression) const;
    [[nodiscard]] double evaluateNumber(std::string_view expression) const;

    // Pre-compiled evaluation
    [[nodiscard]] std::vector<const Node *> evaluate(const XPathExpression &compiled) const;
    [[nodiscard]] std::string evaluateString(const XPathExpression &compiled) const;
    [[nodiscard]] bool evaluateBool(const XPathExpression &compiled) const;
    [[nodiscard]] double evaluateNumber(const XPathExpression &compiled) const;
};
```

---

## 10. DOM Tree Classes (`Node`, `Element`, `XMLAttribute`, `XMLValue`)

### `Node`
```cpp
bool isEmpty() const;
bool isNameable() const;
bool isIndexable() const;
std::string getContents() const;

// Subscript Operators
const Node &operator[](int index) const;
const Node &operator[](const std::string_view &name) const;
const Node &operator[](std::string_view parent, std::string_view child) const; // C++23
const Node &operator[](std::string_view name, int index) const;                 // C++23

// Monadic Child Lookups (C++23)
std::optional<std::reference_wrapper<const Node>> findChild(std::string_view name) const;
std::optional<std::reference_wrapper<Node>> findChild(std::string_view name);

// Range Views (C++20)
auto elements() const;
auto elements(std::string_view name) const;

std::pmr::vector<Node> &getChildren();
const std::pmr::vector<Node> &getChildren() const;
```

### `Element` (Inherits `Variant`)
```cpp
const std::string &name() const;
std::string getPrefix() const;
std::string getLocalName() const;
std::string getNamespaceURI() const;

// Attribute Access
bool hasAttribute(const std::string_view &name) const;
const XMLAttribute &getAttribute(const std::string_view &name) const;
const std::vector<XMLAttribute> &getAttributes() const;
const XMLAttribute &operator[](const std::string_view &attrName) const;

// Monadic Attribute Lookup (C++23)
std::optional<std::reference_wrapper<const XMLAttribute>> 
findAttribute(const std::string_view &attributeName) const noexcept;

// Namespaces
bool hasNameSpace(const std::string_view &prefix) const;
const XMLAttribute &getNameSpace(const std::string_view &prefix) const;
const std::vector<XMLAttribute> &getNameSpaces() const;
```

---

## 11. C++20 Ranges & Views (`XML_Ranges.hpp`)

Defined in `<XML_Lib/XML_Ranges.hpp>`. Provides lazy, non-allocating ranges over DOM nodes:

```cpp
// Returns a view over only Element, Root, or Self children
auto childElements(const Node &parentNode);

// Returns a view over children matching a specific tag name
auto childElements(const Node &parentNode, std::string_view tagName);

// Returns a std::span over an element's attributes
auto elementAttributes(const Node &elementNode);
```

---

## 12. C++20 Concepts (`XML_Concepts.hpp`)

Defined in `<XML_Lib/XML_Concepts.hpp>`. Constrains template types for custom extensions:

```cpp
template<typename T>
concept XMLNodeLike = requires(T t) {
    { t.getChildren() };
    { t.getVariant() };
    { t.isEmpty() } -> std::convertible_to<bool>;
};

template<typename T>
concept XMLSourceLike = requires(T t) {
    { t.current() };
    { t.next() };
    { t.more() } -> std::convertible_to<bool>;
};

template<typename T>
concept XMLDestinationLike = requires(T t, std::string_view sv, char ch) {
    { t.add(sv) };
    { t.add(ch) };
};

template<typename T>
concept XMLValidatorLike = requires(T t, const Node &node) {
    { t.validate(node) };
};
```

---

## 13. Serialization & Formatting (`IStringify`, `StringifyOptions`)

Defined in `<XML_Lib/interface/IStringify.hpp>`.

### Formatting Options
```cpp
struct StringifyOptions {
    bool prettyPrint{ false };              // Enable indentation & line breaks
    int  indentSpaces{ 2 };                  // Spaces per indent level
    bool useTabs{ false };                  // Use tabs instead of spaces
    bool selfClosingSpacing{ false };       // <tag /> vs <tag/>
    bool attributeNewlineWrapping{ false };  // Wrap attributes on new lines
};
```

### Serializer Interface
```cpp
class IStringify {
public:
    virtual ~IStringify() = default;
    virtual void stringify(const Node &xNode, IDestination &dest, unsigned long indent) const = 0;
    virtual void setOptions(const StringifyOptions &options);
    [[nodiscard]] virtual const StringifyOptions &getOptions() const;
};
```

---

## 14. SOLID Role Interfaces

XML_Lib enforces strict role segregation across all I/O, entity, and traversal subsystems:

- **`ISource`**: Composite of `ICharStream`, `ILocationTracker`, `IRangeReader`, `IResettableStream`.
- **`IDestination`**: Composite of `ICharWriter`, `IStringWriter`, `IResettableDestination`.
- **`IEntityResolver`**: Single-method contract (`resolve(systemId, publicId)`) for pluggable entity/catalog lookups.
- **`IEntityRegistry`**, **`IEntityExpander`**, **`ISecurityPolicyManager`**: Role segregation for entity translation and recursion control.
- **`IVisitorRoles`**: Granular visitor interfaces (`IElementVisitor`, `ICommentVisitor`, `ICDATAVisitor`, `IContentVisitor`, etc.).

---

## 15. Exceptions and Error Types

All exceptions derive from `std::runtime_error`:

| Exception Class | Thrown By | Cause |
| :--- | :--- | :--- |
| `SyntaxError` | `Default_Parser`, `XMLReader` | Malformed XML, unclosed tags, undeclared prefix, illegal characters |
| `XML_Error` | `parseExpected`, `readExpected` | Non-throwing error struct containing `line`, `column`, `message` |
| `Node::Error` | `Node` | Invalid type cast, out-of-bounds child access, invalid variant access |
| `XMLAttribute::Error` | `XMLAttribute`, `Element` | Attribute not found in throwing lookups |
| `IValidator::Error` | `DTD_Validator`, `XSD_Validator` | Validation failure against DTD or W3C XML Schema |
| `XPath::Error` | `XPath`, `XPathExpression` | Malformed XPath syntax or evaluation failure |
| `BufferSource::Error`, `FileSource::Error`, `MMapSource::Error` | I/O Classes | File missing, permissions denied, buffer overflow |
