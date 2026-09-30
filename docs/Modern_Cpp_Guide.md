# XML_Lib Modern C++23 & C++20 Idioms Guide

A comprehensive guide demonstrating how **XML_Lib** leverages modern C++23 and C++20 language features, standard library types, and idiomatic patterns for type safety, performance, and developer ergonomics.

---

## 1. Modern C++ Standard & Compiler Requirements

`XML_Lib` is built on the **ISO/IEC 14882:2023 (C++23)** standard. It takes full advantage of modern standard library abstractions while retaining zero external runtime dependencies.

### Supported Compilers
- **GCC**: ≥ 13.0
- **Clang**: ≥ 17.0
- **AppleClang**: ≥ 15.0
- **MSVC**: ≥ 19.38 (Visual Studio 2022 v17.8+)

---

## 2. Non-Throwing Error Handling (`std::expected`)

Traditional C++ XML libraries rely heavily on exceptions for error reporting. However, exception unwinding introduces overhead and is often restricted or forbidden in embedded systems, game engines, and high-frequency trading platforms.

`XML_Lib` provides native **`std::expected`** non-throwing APIs alongside traditional throwing methods.

### 2.1 The `XML_Error` Structure
Defined in `<XML_Lib/XML_Expected.hpp>`:
```cpp
struct XML_Error {
    std::string message;
    long line{ 0 };
    long column{ 0 };
};
```

### 2.2 Parsing with `XML::parseExpected`
```cpp
#include <XML_Lib/XML.hpp>
#include <iostream>

using namespace XML_Lib;

void loadConfig(std::string_view configXml) {
    auto result = XML::parseExpected(configXml);

    if (result) {
        std::unique_ptr<XML> doc = std::move(*result);
        std::cout << "Successfully parsed root: " << doc->root().getContents() << "\n";
    } else {
        const XML_Error &err = result.error();
        std::cerr << "XML Parse Failure [" << err.line << ":" << err.column << "]: " 
                  << err.message << "\n";
    }
}
```

### 2.3 Functional Composition with Monadic Operations
C++23 `std::expected` supports monadic composition via `.and_then()`, `.transform()`, and `.or_else()`:

```cpp
auto processUserRecord(std::string_view xml) -> std::expected<std::string, XML_Error> {
    return XML::parseExpected(xml)
        .and_then([](std::unique_ptr<XML> doc) -> std::expected<std::string, XML_Error> {
            auto &root = doc->root();
            if (root.isEmpty() || !root.isNameable()) {
                return std::unexpected(XML_Error{"Empty document root", 1, 1});
            }
            return root["user", "name"].getContents();
        });
}
```

### 2.4 Non-Throwing Validation
```cpp
XML xml{"<item id='1'/>"};

// DTD validation
std::expected<void, std::string> dtdResult = xml.validateExpected();

// XSD validation
std::expected<void, std::string> xsdResult = xml.validateExpected(xsdSchemaString);
if (!xsdResult) {
    std::cerr << "Schema validation failed: " << xsdResult.error() << "\n";
}
```

---

## 3. Monadic Optional Lookups (`std::optional`)

`Node`, `Element`, and `XMLReader` provide C++23 monadic optional lookups:
- `Node::findChild(name)`
- `Element::findAttribute(name)`
- `XMLReader::findAttribute(name)`

These methods return `std::optional<std::reference_wrapper<T>>` or `std::optional<std::string_view>`, enabling safe, expressive traversal without nested `if` statements:

```cpp
#include <XML_Lib/XML.hpp>
#include <iostream>

using namespace XML_Lib;

void printBookMetadata(const Node &rootNode) {
    // Chain lookups safely using C++23 monadic operations
    auto isbn = rootNode.findChild("book")
        .and_then([](const Node &book) {
            return NRef<Element>(book).findAttribute("isbn");
        })
        .transform([](const auto &attr) {
            return attr.get().getParsed();
        })
        .value_or("N/A");

    std::cout << "Book ISBN: " << isbn << "\n";
}
```

---

## 4. Multidimensional Subscripting (C++23)

C++23 allows `operator[]` to take multiple arguments. `XML_Lib` implements multidimensional indexing on `Node` to streamline tree queries:

```cpp
XML xml{R"(
    <config>
        <database>
            <host>localhost</host>
            <port>5432</port>
        </database>
        <server name="primary"/>
        <server name="replica"/>
    </config>
)"};

auto &root = xml.root();

// 1. Traverse parent tag and child tag in a single expression:
std::string host = root["database", "host"].getContents(); // "localhost"
std::string port = root["database", "port"].getContents(); // "5432"

// 2. Access child tag by name and index in a single expression:
std::string replicaName = root["server", 1]["name"].getContents();
```

---

## 5. Standard C++20 Ranges and Views

Defined in `<XML_Lib/XML_Ranges.hpp>`, `XML_Lib` integrates directly with the `<ranges>` library, providing zero-allocation views for element filtering and attribute iteration:

### 5.1 Iterating Child Elements
```cpp
#include <XML_Lib/XML.hpp>
#include <XML_Lib/XML_Ranges.hpp>
#include <ranges>
#include <iostream>

using namespace XML_Lib;

void listActiveUsers(const XML &doc) {
    // Filter and transform using standard C++20 range pipelines
    auto activeUsers = doc.root().elements("user")
        | std::views::filter([](const Node &user) {
            return NRef<Element>(user).findAttribute("active")
                                      .transform([](const auto &a) { return a.get().getParsed() == "true"; })
                                      .value_or(false);
        })
        | std::views::transform([](const Node &user) {
            return user["name"].getContents();
        });

    for (const std::string &name : activeUsers) {
        std::cout << "Active user: " << name << "\n";
    }
}
```

### 5.2 Attribute Spans with `elementAttributes`
```cpp
for (const XMLAttribute &attr : elementAttributes(xml.root())) {
    std::cout << attr.getName() << " = " << attr.getParsed() << "\n";
}
```

---

## 6. Type Constraints with C++20 Concepts

Defined in `<XML_Lib/XML_Concepts.hpp>`, `XML_Lib` exposes four core concepts to constrain template parameters and enable custom extensions:

### 6.1 Defined Concepts
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

### 6.2 Generic Algorithm Example
Using `XMLNodeLike` to write type-safe generic tree utilities:

```cpp
#include <XML_Lib/XML_Concepts.hpp>
#include <iostream>

template<XML_Lib::XMLNodeLike TNode>
std::size_t countLeaves(const TNode &node) {
    if (node.getChildren().empty()) {
        return 1;
    }
    std::size_t leaves = 0;
    for (const auto &child : node.getChildren()) {
        leaves += countLeaves(child);
    }
    return leaves;
}
```

---

## 7. C++23 Native String & Utility Upgrades

- **`std::string::contains` & `std::string_view::contains`**: Native membership testing without verbose `.find(...) != std::string_view::npos` comparisons.
- **`std::unreachable()`**: Employed in `XMLReader::nodeTypeToString` and state machine enumerations to guarantee compile-time completeness and assist optimizer code generation.
- **`std::pmr`**: Monotonic memory resources for arena-allocated node vectors and attribute spans.
