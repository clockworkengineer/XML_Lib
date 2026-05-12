# XML_Lib User Guide

XML_Lib is a C++23 library for parsing, creating, manipulating, and serialising XML.
All public symbols live in the `XML_Lib` namespace.

**For a complete method listing see [API.md](API.md).**

---

## Contents

1. [Including and linking](#1-including-and-linking)
2. [Parsing XML](#2-parsing-xml)
3. [Accessing the document tree](#3-accessing-the-document-tree)
4. [Reading element contents and attributes](#4-reading-element-contents-and-attributes)
5. [Iterating children](#5-iterating-children)
6. [Serialising (stringify)](#6-serialising-stringify)
7. [Error handling](#7-error-handling)
8. [DTD validation](#8-dtd-validation-xml_lib_enable_dtd)
9. [XSD validation](#9-xsd-validation-xml_lib_enable_xsd)
10. [XPath queries](#10-xpath-queries-xml_lib_enable_xpath)
11. [XML Namespaces](#11-xml-namespaces)
12. [Advanced I/O (ISource / IDestination)](#12-advanced-io-isource--idestination)

---

## 1. Including and linking

```cpp
#include "XML.hpp"          // always required — top-level XML class
#include "XML_NodeRef.hpp"  // isA<T> / NRef<T> helpers and all variant types
#include "XML_Sources.hpp"  // BufferSource, FileSource (advanced I/O)
#include "XML_Destinations.hpp" // BufferDestination, FileDestination (advanced I/O)
using namespace XML_Lib;
```

CMake consumers that use `target_link_libraries(myTarget PRIVATE XML_Lib)` automatically
receive the necessary include paths — no manual `-I` flags are required.

---

## 2. Parsing XML

### From a string (convenience overload)

```cpp
#include "XML.hpp"
using namespace XML_Lib;

XML xml;
xml.parse("<?xml version=\"1.0\"?><root><item id=\"1\">Hello</item></root>");
```

### From a file path (convenience overload)

```cpp
xml.parse(std::filesystem::path{"data/config.xml"});
// or, accepting implicit conversion from a string literal:
xml.parse(std::filesystem::path{"data/config.xml"});
```

### Constructor shorthand

```cpp
XML xml{"<?xml version=\"1.0\"?><root/>"};  // parses immediately
```

### Assignment shorthand

```cpp
XML xml;
xml = "<?xml version=\"1.0\"?><root/>";
```

---

## 3. Accessing the document tree

After parsing, the document exposes three top-level nodes:

```cpp
Node &decl  = xml.declaration(); // <?xml version="1.0"?>
Node &prolog = xml.prolog();     // everything before the root element
Node &root  = xml.root();        // root element Node
```

### Casting a Node to a concrete variant type

`Node` is a type-erased wrapper. Use `isA<T>` to test and `NRef<T>` to cast:

```cpp
#include "XML_NodeRef.hpp"
using namespace XML_Lib;

if (isA<Root>(xml.root())) {
    auto &rootElem = NRef<Root>(xml.root());
    std::cout << rootElem.name() << "\n";  // e.g. "root"
}
```

Available variant types: `Root`, `Element`, `Self`, `Content`, `Comment`, `CDATA`,
`PI`, `EntityReference`, `DTD`, `Declaration`, `Prolog`.

`NRef<T>` throws `Node::Error` if the node is not of type `T`.

### Subscript access

```cpp
// By child index (0-based)
const Node &firstChild = xml.root()[0];

// By element name (first match)
const Node &item = xml.root()["item"];
```

---

## 4. Reading element contents and attributes

```cpp
XML xml{"<?xml version=\"1.0\"?><root><item id=\"42\" lang=\"en\">Hello</item></root>"};

auto &rootElem = NRef<Root>(xml.root());
auto &item     = NRef<Element>(rootElem[0]);

// Element name
std::cout << item.name() << "\n";        // "item"

// Text content
std::cout << item.getContents() << "\n"; // "Hello"

// Read a specific attribute
if (item.hasAttribute("id")) {
    std::cout << item["id"].getParsed() << "\n";  // "42"
}

// Iterate all attributes
for (const auto &attr : item.getAttributes()) {
    std::cout << attr.getName() << "=" << attr.getParsed() << "\n";
}
```

---

## 5. Iterating children

```cpp
XML xml;
xml.parse("<?xml version=\"1.0\"?>"
          "<library><book>C++23</book><book>XML</book></library>");

auto &lib = NRef<Root>(xml.root());

for (const auto &child : lib.getChildren()) {
    if (isA<Element>(child)) {
        std::cout << NRef<Element>(child).getContents() << "\n";
    }
}
```

Self-closing elements (`<tag/>`) are `Self` nodes, not `Element` nodes:

```cpp
if (isA<Self>(child)) {
    auto &sc = NRef<Self>(child);
    std::cout << sc.name() << "\n";
}
```

---

## 6. Serialising (stringify)

> Requires `XML_LIB_ENABLE_STRINGIFY` (on by default).

### To a string

```cpp
std::string text = xml.stringify();
```

### To a file

```cpp
xml.stringify(std::filesystem::path{"output.xml"});                       // UTF-8
xml.stringify(std::filesystem::path{"output_bom.xml"}, XML::Format::utf8BOM);
xml.stringify(std::filesystem::path{"output_u16.xml"}, XML::Format::utf16LE);
```

Available formats: `utf8`, `utf8BOM`, `utf16BE`, `utf16LE`, `utf32BE`, `utf32LE`.

---

## 7. Error handling

All XML_Lib exceptions derive from `std::runtime_error`:

| Exception | Thrown when |
|---|---|
| `SyntaxError` | Malformed XML or namespace violations |
| `Node::Error` | Invalid node cast or out-of-range access |
| `XMLAttribute::Error` | Attribute not found |
| `IValidator::Error` | DTD or XSD validation failure |
| `XPath::Error` | Invalid XPath expression or evaluation error |
| `BufferSource::Error` | Empty buffer supplied |
| `FileSource::Error` | File not found or unreadable |

```cpp
try {
    xml.parse("<?xml version=\"1.0\"?><unclosed>");
} catch (const SyntaxError &e) {
    std::cerr << e.what() << "\n";
    // e.g. "XML Syntax Error [Line: 1 Column: 35] ..."
}
```

---

## 8. DTD validation (`XML_LIB_ENABLE_DTD`)

When the document contains an internal or external DTD, call `validate()` after parsing:

```cpp
#include "XML.hpp"
#if defined(XML_LIB_ENABLE_DTD)
#include "DTD_Validator.hpp"
#endif
using namespace XML_Lib;

XML xml;
xml.parse(std::filesystem::path{"note.xml"});

try {
    xml.validate();  // validates against the DTD embedded in the document
} catch (const IValidator::Error &e) {
    std::cerr << e.what() << "\n";
    // e.g. "XML Validation Error [Line: 4] Element <to> not valid in context."
}
```

The DTD node is accessible as `xml.dtd()` (throws `Node::Error` if the document has none).

---

## 9. XSD validation (`XML_LIB_ENABLE_XSD`)

Supply an XSD schema as a string (or load it from a file) and call `validate(xsdSource)`:

```cpp
#include "XML.hpp"
using namespace XML_Lib;

const std::string schema = R"(
<?xml version="1.0" encoding="UTF-8"?>
<xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema">
  <xs:element name="note">
    <xs:complexType>
      <xs:sequence>
        <xs:element name="to"   type="xs:string"/>
        <xs:element name="from" type="xs:string"/>
        <xs:element name="body" type="xs:string"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>
</xs:schema>
)";

XML xml;
xml.parse("<?xml version=\"1.0\"?>"
          "<note><to>Alice</to><from>Bob</from><body>Hello!</body></note>");

try {
    xml.validate(schema);  // silent on success
} catch (const IValidator::Error &e) {
    std::cerr << e.what() << "\n";
}
```

For file-based schemas, read the XSD into a string first:

```cpp
const std::string xsdText = XML::fromFile("schema.xsd");
xml.validate(xsdText);
```

**Supported XSD features:** `xs:sequence`, `xs:choice`, `xs:all`; `minOccurs`/`maxOccurs`;
all built-in simple types; named `xs:simpleType` restrictions (`minInclusive`, `maxInclusive`,
`pattern`, `enumeration`, `length`, `minLength`, `maxLength`); attribute `use` / `fixed`;
`xs:anyAttribute`; anonymous inline complex and simple types.

---

## 10. XPath queries (`XML_LIB_ENABLE_XPATH`)

```cpp
#include "XML.hpp"
#include "XPath.hpp"
using namespace XML_Lib;

XML xml;
xml.parse(std::filesystem::path{"bookstore.xml"});

// Shorthand on the XML object
auto books = xml.xpath("//book");
std::cout << books.size() << " books\n";

// Filter by attribute
auto webBooks = xml.xpath("//book[@category='web']");

// The XPath class offers typed evaluation
XPath xp(xml.root());
std::string title  = xp.evaluateString("string(//title[1])");
double      count  = xp.evaluateNumber("count(//book)");
bool        hasWeb = xp.evaluateBool("count(//book[@category='web']) > 0");
```

Returned `const Node *` pointers are valid only while the `XML` object is alive.

```cpp
for (const Node *n : xml.xpath("//book/title")) {
    std::cout << NRef<Element>(*n).getContents() << "\n";
}
```

**Supported:** all 13 XPath 1.0 axes, abbreviated syntax (`//`, `.`, `..`, `@`),
predicates, union (`|`), all comparison operators, 28+ built-in functions.
See [API.md](API.md) for the full function list.

---

## 11. XML Namespaces

```cpp
XML xml;
xml.parse("<?xml version=\"1.0\"?>"
          "<root xmlns:h=\"http://www.w3.org/TR/html4/\">"
          "<h:table h:border=\"1\"><h:tr><h:td>Data</h:td></h:tr></h:table>"
          "</root>");

auto &root  = NRef<Root>(xml.root());
auto &table = NRef<Element>(root[0]);

std::cout << table.name();            // "h:table"
std::cout << table.getPrefix();       // "h"
std::cout << table.getLocalName();    // "table"
std::cout << table.getNamespaceURI(); // "http://www.w3.org/TR/html4/"
```

- `xmlns="uri"` — default namespace, accessible via `getNameSpace(":")`.
- `xmlns:prefix="uri"` — prefixed namespace, accessible via `getNameSpace("prefix")`.
- `getNameSpaces()` returns all in-scope declarations accumulated from root to that element.
- Undeclared prefixes and duplicate declarations on the same element throw `SyntaxError`.

---

## 12. Advanced I/O (ISource / IDestination)

The convenience overloads on `XML` cover the common cases. For custom sources or
destinations use `ISource` / `IDestination` directly:

```cpp
#include "XML_Sources.hpp"
#include "XML_Destinations.hpp"
using namespace XML_Lib;

// Parse from a std::string
std::string xmlText = "<?xml version=\"1.0\"?><root/>";
BufferSource src{xmlText};
xml.parse(src);

// Parse from a file
FileSource fileSrc{"data/config.xml"};
xml.parse(fileSrc);

// Stringify to a string
BufferDestination buf;
xml.stringify(buf);
std::string result = buf.toString();

// Stringify to a file (UTF-16 LE)
FileDestination fileDst{"output.xml", XML::Format::utf16LE};
xml.stringify(fileDst);
```

---

## 13. Performance

### Memory allocation model

XML_Lib uses a PMR (Polymorphic Memory Resource) monotonic arena to hold all
parsed node children.  Attribute and namespace lists on `Element`, `Root`, and
`Self` nodes also live in the same arena.  This means a typical parse-and-read
workload performs **zero heap allocations** for node children and attribute
storage after the initial arena buffer is committed.

The arena size is controlled at configure time:

```sh
cmake -S . -B build -DXML_LIB_ARENA_SIZE_KB=1024   # 1 MB arena (default: 256 KB)
```

The default of **256 KB** is sufficient for most documents up to a few hundred
kilobytes.  For large documents (≥ 1 MB of XML text) set the arena to at least
twice the uncompressed document size.  When the arena is exhausted the
`monotonic_buffer_resource` falls back to `new`/`delete` automatically —
correctness is preserved, only the "zero-allocation" guarantee is lost.

### Baseline benchmark results (v1.2.0, GCC 13, Debug+ASan, Linux)

| Benchmark | Mean | Std Dev |
|---|---|---|
| Parse large XML document | 76.1 ms | 7.9 ms |
| XPath evaluate item name | 143.2 ms | 28.4 ms |
| XSD validate large document | 3.4 ms | 0.96 ms |

*Results captured with `XML_LIB_ARENA_SIZE_KB=256` (default).*  
*Benchmarks are run with Catch2's built-in microbenchmark framework (100 samples, 1 iteration each).*

Run the benchmarks yourself:

```sh
cmake --build build --target XML_Lib_Performance_Tests
./build/tests/XML_Lib_Performance_Tests
```

### Content cache

`Element::getContents()` (which concatenates all descendant text nodes into a
single string) is memoised: the result is cached on first access and reused on
subsequent calls as long as the child count has not changed.  For parse-once /
read-many workloads this reduces repeated content access from O(n) per call to
O(1).

---

*For full method signatures and all variant types see the [API Reference](API.md).*  
*For runnable examples see the [examples/](../examples/) directory.*
