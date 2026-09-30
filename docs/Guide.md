# XML_Lib User Guide

**XML_Lib** is an enterprise-grade, high-performance C++23 library for parsing, streaming, creating, manipulating, querying, and serialising XML. All public symbols reside in the `XML_Lib` namespace.

**For complete API method signatures and type references, see [API.md](API.md).**

---

## Contents

1. [Including and linking](#1-including-and-linking)
2. [Parsing XML](#2-parsing-xml)
3. [Accessing the document tree](#3-accessing-the-document-tree)
4. [Reading element contents and attributes](#4-reading-element-contents-and-attributes)
5. [Iterating children (C++20 Ranges & Monadic Lookups)](#5-iterating-children-c20-ranges--monadic-lookups)
6. [Serialising (stringify) & Formatting Options](#6-serialising-stringify--formatting-options)
7. [Exception-Based Error Handling](#7-exception-based-error-handling)
8. [Non-Throwing Error Handling (`std::expected`)](#8-non-throwing-error-handling-stdexpected)
9. [DTD Validation](#9-dtd-validation-xml_lib_enable_dtd)
10. [XSD Validation & Pre-Compiled Schemas](#10-xsd-validation--pre-compiled-schemas)
11. [XPath Queries & Pre-Compiled ASTs](#11-xpath-queries--pre-compiled-asts)
12. [XML Namespaces](#12-xml-namespaces)
13. [Advanced I/O & Memory-Mapped Files (`MMapSource`)](#13-advanced-io--memory-mapped-files-mmapsource)
14. [Streaming Processing with `XMLReader` & `XMLWriter`](#14-streaming-processing-with-xmlreader--xmlwriter)
15. [Offline Catalog & Entity Resolution (`OASIS_Catalog`)](#15-offline-catalog--entity-resolution-oasis_catalog)
16. [Performance, Memory Model & PMR Arenas](#16-performance-memory-model--pmr-arenas)
17. [Role Visitors & SOLID Extensions](#17-role-visitors--solid-extensions)

---

## 1. Including and linking

```cpp
#include <XML_Lib/XML.hpp>             // Top-level XML class & ParseOptions
#include <XML_Lib/XML_Node.hpp>        // isA<T> / NRef<T> helpers and variant types
#include <XML_Lib/XML_Ranges.hpp>      // C++20 range views (elements(), childElements())
#include <XML_Lib/XMLReader.hpp>       // Streaming pull parser (O(1) memory)
#include <XML_Lib/XMLWriter.hpp>       // Streaming push serializer
#include <XML_Lib/XML_Sources.hpp>     // BufferSource, FileSource, MMapSource
#include <XML_Lib/XML_Destinations.hpp>// BufferDestination, FileDestination
#include <XML_Lib/OASIS_Catalog.hpp>   // OASIS XML Catalogs 1.1 resolver
#include <XML_Lib/XSD_Schema.hpp>      // Pre-compiled XSD schema
#include <XML_Lib/XPath.hpp>           // XPath engine & XPathExpression

using namespace XML_Lib;
```

CMake consumers linking with `target_link_libraries(myTarget PRIVATE XML_Lib::XML_Lib)` automatically receive all required include directories, standard flags (`-std=c++23`), and library dependencies.

---

## 2. Parsing XML

### From an in-memory string
```cpp
XML xml;
xml.parse("<?xml version=\"1.0\"?><root><item id=\"1\">Hello</item></root>");
```

### From a file path
```cpp
xml.parse(std::filesystem::path{"data/config.xml"});
```

### Constructor and assignment shorthands
```cpp
XML xml{"<?xml version=\"1.0\"?><root/>"}; // Parses immediately upon construction

XML target;
target = "<?xml version=\"1.0\"?><root/>";  // Replaces and parses immediately
```

### Controlling parser limits (`ParseOptions`)
```cpp
ParseOptions options;
options.maxXmlSize = 50 * 1024 * 1024;    // 50 MiB limit
options.maxEntityExpansionDepth = 100;     // Mitigate Billion Laughs attacks
options.allowExternalEntities = false;     // Reject XXE attacks
options.strictNamespaces = true;           // Forbid prefix unbinding

xml.parse(std::filesystem::path{"untrusted.xml"}, options);
```

---

## 3. Accessing the document tree

After parsing, the document provides three root accessors:

```cpp
Node &decl   = xml.declaration(); // <?xml version="1.0"?>
Node &prolog = xml.prolog();      // PIs, comments, and DTD before root element
Node &root   = xml.root();        // Document root element Node
```

### Downcasting with `isA<T>` and `NRef<T>`
`Node` is a type-erased wrapper around a concrete `Variant`. Use `isA<T>` to verify the variant and `NRef<T>` to retrieve a typed reference:

```cpp
if (isA<Root>(xml.root())) {
    auto &rootElem = NRef<Root>(xml.root());
    std::cout << "Root element name: " << rootElem.name() << "\n";
}
```

Available variant types: `Root`, `Element`, `Self`, `Content`, `Comment`, `CDATA`, `PI`, `EntityReference`, `DTD`, `Declaration`, `Prolog`.

### Subscript Access (C++23)

```cpp
// 1. Single index by child position (0-based)
const Node &firstChild = xml.root()[0];

// 2. By child element name (first match)
const Node &item = xml.root()["item"];

// 3. Multidimensional subscripting: parent and child tag name
const Node &author = xml.root()["book", "author"];

// 4. Multidimensional subscripting: child tag name and index
const Node &secondBook = xml.root()["book", 1];
```

---

## 4. Reading element contents and attributes

```cpp
XML xml{R"(
    <catalog>
        <book id="bk101" in_print="true">
            <title>C++23 in Action</title>
        </book>
    </catalog>
)"};

auto &root = NRef<Root>(xml.root());
auto &book = NRef<Element>(root["book"]);

// Element tag name
std::cout << book.name() << "\n"; // "book"

// Text content
std::cout << book["title"].getContents() << "\n"; // "C++23 in Action"

// Traditional attribute lookup
if (book.hasAttribute("id")) {
    std::cout << "ID: " << book["id"].getParsed() << "\n";
}

// C++23 Monadic optional lookup
auto inPrint = book.findAttribute("in_print")
                   .transform([](const auto &attr) { return attr.get().getParsed(); })
                   .value_or("false");
```

---

## 5. Iterating children (C++20 Ranges & Monadic Lookups)

### Modern C++20 Range Views
Include `<XML_Lib/XML_Ranges.hpp>` to iterate over children without boilerplate type checks:

```cpp
// Iterate over all child elements (Element, Root, or Self)
for (const Node &child : xml.root().elements()) {
    std::cout << "Child element: " << NRef<Element>(child).name() << "\n";
}

// Filter child elements by tag name
for (const Node &book : xml.root().elements("book")) {
    std::cout << "Book: " << book["title"].getContents() << "\n";
}

// Iterate over attributes using std::span
for (const XMLAttribute &attr : elementAttributes(xml.root()["book"])) {
    std::cout << attr.getName() << " = " << attr.getParsed() << "\n";
}
```

### C++23 Monadic Child Lookup
```cpp
// Returns std::optional<std::reference_wrapper<const Node>>
xml.root().findChild("book")
          .and_then([](const Node &node) { return node.findChild("title"); })
          .if_present([](const Node &title) {
              std::cout << "Found title: " << title.getContents() << "\n";
          });
```

---

## 6. Serialising (stringify) & Formatting Options

### Serialising to String or File
```cpp
// To string
std::string xmlString = xml.stringify();

// Directly to file with encoding format
xml.stringify(std::filesystem::path{"output.xml"}, XML::Format::utf8);
xml.stringify(std::filesystem::path{"output_bom.xml"}, XML::Format::utf8BOM);
xml.stringify(std::filesystem::path{"output_u16.xml"}, XML::Format::utf16LE);
```

### Pretty-Printing with `StringifyOptions`
```cpp
#include <XML_Lib/XML_Factories.hpp>
#include <XML_Lib/interface/IStringify.hpp>

StringifyOptions options;
options.prettyPrint = true;
options.indentSpaces = 4;
options.selfClosingSpacing = true; // <tag /> instead of <tag/>

auto stringifier = XML_Factories::createDefaultStringify();
stringifier->setOptions(options);

XML xmlWithFormatting(std::move(stringifier));
xmlWithFormatting.parse("<root><item>Data</item></root>");
std::cout << xmlWithFormatting.stringify() << "\n";
```

---

## 7. Exception-Based Error Handling

All throwing methods report errors using exceptions derived from `std::runtime_error`:

```cpp
try {
    XML xml{"<unclosed>tag"};
} catch (const SyntaxError &ex) {
    std::cerr << "Malformed XML: " << ex.what() << "\n";
} catch (const Node::Error &ex) {
    std::cerr << "DOM tree error: " << ex.what() << "\n";
} catch (const IValidator::Error &ex) {
    std::cerr << "Validation failed: " << ex.what() << "\n";
} catch (const std::exception &ex) {
    std::cerr << "General error: " << ex.what() << "\n";
}
```

---

## 8. Non-Throwing Error Handling (`std::expected`)

For performance-critical code paths or systems running with `-fno-exceptions`, `XML_Lib` provides first-class `std::expected` APIs:

```cpp
#include <XML_Lib/XML.hpp>
#include <XML_Lib/XML_Expected.hpp>

// Parse without throwing
auto result = XML::parseExpected(std::filesystem::path{"document.xml"});

if (result.has_value()) {
    std::unique_ptr<XML> xml = std::move(result.value());
    std::cout << "Root: " << xml->root().getContents() << "\n";
} else {
    const XML_Error &err = result.error();
    std::cerr << "Parse failed at line " << err.line 
              << ", column " << err.column << ": " << err.message << "\n";
}
```

---

## 9. DTD Validation (`XML_LIB_ENABLE_DTD`)

```cpp
XML xml{R"(
    <!DOCTYPE root [
        <!ELEMENT root (item+)>
        <!ELEMENT item (#PCDATA)>
    ]>
    <root>
        <item>Valid content</item>
    </root>
)"};

// Throwing validation
xml.validate();

// Non-throwing validation (C++23)
auto valid = xml.validateExpected();
if (!valid) {
    std::cerr << "DTD invalid: " << valid.error() << "\n";
}
```

---

## 10. XSD Validation & Pre-Compiled Schemas

### Ad-hoc Validation
```cpp
XML xml{"<note><to>Alice</to><from>Bob</from></note>"};
xml.validate(R"(
    <xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema">
        <xs:element name="note">
            <xs:complexType>
                <xs:sequence>
                    <xs:element name="to" type="xs:string"/>
                    <xs:element name="from" type="xs:string"/>
                </xs:sequence>
            </xs:complexType>
        </xs:element>
    </xs:schema>
)");
```

### High-Speed Pre-Compilation with `XSD_Schema`
Parsing schemas repeatedly in hot loops is inefficient. Use `XSD_Schema` to parse and build the schema model once, then validate millions of documents concurrently:

```cpp
#include <XML_Lib/XSD_Schema.hpp>

// Compile schema once
XSD_Schema schema = XSD_Schema::fromFile("order.xsd");

// Reuse across threads and documents
XML doc1 = XML::fromFile("order_001.xml");
doc1.validate(schema);

XML doc2 = XML::fromFile("order_002.xml");
auto result = doc2.validateExpected(schema);
```

---

## 11. XPath Queries & Pre-Compiled ASTs

### Ad-hoc XPath Queries
```cpp
XML xml{"<inventory><item category='it'>Laptop</item></inventory>"};

// Direct helper on XML
auto results = xml.xpath("//item[@category='it']");
for (const Node *node : results) {
    std::cout << "Match: " << node->getContents() << "\n";
}
```

### Pre-Compiling Expressions with `XPathExpression`
```cpp
#include <XML_Lib/XPath.hpp>

// Compile AST once
XPathExpression query{"//item[@price > 100]"};

// Evaluate repeatedly on any document or node
for (const auto &file : xmlFiles) {
    XML doc{file};
    auto matches = doc.xpath(query);
    std::cout << file << " has " << matches.size() << " matching items.\n";
}
```

---

## 12. XML Namespaces

`XML_Lib` provides full W3C XML Namespaces 1.0 conformance:

```cpp
XML xml{R"(
    <root xmlns="urn:default" xmlns:svg="http://www.w3.org/2000/svg">
        <svg:circle svg:r="10"/>
    </root>
)"};

auto &circle = NRef<Element>(xml.root()[0]);
std::cout << circle.name();             // "svg:circle"
std::cout << circle.getPrefix();        // "svg"
std::cout << circle.getLocalName();     // "circle"
std::cout << circle.getNamespaceURI();  // "http://www.w3.org/2000/svg"
```

---

## 13. Advanced I/O & Memory-Mapped Files (`MMapSource`)

For multi-hundred-megabyte files, standard filesystem reads incur unnecessary kernel-to-userspace memory copying. `MMapSource` maps the file directly into process address space via OS page caching (`mmap` on Linux/macOS, `CreateFileMappingA` on Windows):

```cpp
#include <XML_Lib/XML_Sources.hpp>

// Zero-copy ingestion
MMapSource mmapSource{"huge_database.xml"};
XML xml;
xml.parse(mmapSource);
```

---

## 14. Streaming Processing with `XMLReader` & `XMLWriter`

For multi-gigabyte files that exceed available RAM, use `XMLReader` and `XMLWriter`.

### Pull Parsing with `XMLReader` ($O(1)$ Memory)
```cpp
#include <XML_Lib/XMLReader.hpp>

auto reader = XMLReader::fromFile("50gb_data.xml");

while (reader.read()) {
    if (reader.nodeType() == XMLReader::NodeType::ElementStart && reader.name() == "transaction") {
        auto amount = reader.findAttribute("amount").value_or("0");
        std::cout << "Transaction: " << amount << "\n";
        
        // Skip unneeded child subtrees to save CPU time
        reader.skip();
    }
}
```

### Push Generation with `XMLWriter`
```cpp
#include <XML_Lib/XMLWriter.hpp>

auto writer = XMLWriter::toFile("export.xml");
writer.setIndent(true, 2);

writer.writeStartDocument("1.0", "UTF-8");
writer.writeStartElement("records");

for (int i = 0; i < 1000000; ++i) {
    writer.writeStartElement("record");
    writer.writeAttribute("id", std::to_string(i));
    writer.writeElement("status", "processed");
    writer.writeEndElement(); // </record>
}

writer.writeEndElement(); // </records>
writer.writeEndDocument();
```

---

## 15. Offline Catalog & Entity Resolution (`OASIS_Catalog`)

To protect against external entity attacks (XXE) and enable validation in air-gapped production networks without internet access, use `OASIS_Catalog`:

```cpp
#include <XML_Lib/OASIS_Catalog.hpp>

OASIS_Catalog catalog;
catalog.loadCatalogFile("catalog.xml");

// Map a remote schema URL to a local disk file
catalog.addSystemMapping("http://example.com/schema.xsd", "/etc/schemas/schema.xsd");

ParseOptions options;
options.entityResolver = &catalog;

XML xml;
xml.parse(FileSource{"untrusted.xml"}, options);
```

---

## 16. Performance, Memory Model & PMR Arenas

`XML_Lib` uses `std::pmr` (Polymorphic Memory Resources) monotonic buffer arenas to allocate all DOM child nodes and attributes in contiguous chunks. This provides:
- **Zero heap fragmentation** during parsing.
- **Cache locality** for lightning-fast traversal.
- **Instant bulk deallocation** on document destruction.

Arena buffer size can be configured at build time:
```bash
cmake -B build -S . -DXML_LIB_ARENA_SIZE_KB=1024
```

---

## 17. Role Visitors & SOLID Extensions

To inspect the tree without writing monolithic visitor classes, use narrow role visitors:

```cpp
#include <XML_Lib/interface/IVisitorRoles.hpp>

struct ElementCounter : public IElementVisitor {
    size_t count{ 0 };
    void onElement(const Node &node) override { ++count; }
};

XML xml{"<root><a><b/><c/></a></root>"};
ElementCounter counter;
xml.traverse(counter);
std::cout << "Elements count: " << counter.count << "\n"; // 4
```

---

*For further details, refer to the specialized guides in `docs/`:*
- [Streaming Processing Guide](Streaming_Guide.md)
- [Modern C++ Guide](Modern_Cpp_Guide.md)
- [Security & Catalog Resolution Guide](Entity_Catalog_Resolution.md)
- [Performance Tuning Guide](Performance_Tuning_Guide.md)
- [SOLID Architecture Guide](SOLID_Architecture_Guide.md)
