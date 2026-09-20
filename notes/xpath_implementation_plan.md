# XPath Implementation Plan for XML_Lib

## Overview

This document provides a concrete, phased plan for adding XPath 1.0 query support to XML_Lib. The implementation introduces a new `XPath` public class (Pimpl pattern, mirroring `XSD_Validator`) backed by `XPath_Impl`, which owns a lexer, parser, and evaluator. A convenience overload `XML::xpath(expression)` is added to the top-level `XML` class.

The plan targets **XPath 1.0** (W3C Recommendation 1999-11-16). It is split into seven sections (A–G).

---

## Section A — Public API

**Goal**: Define the public-facing `XPath` class and integrate it into `XML`.

### New header: `classes/include/XPath.hpp`

```cpp
namespace XML_Lib {

class XPath_Impl;
struct Node;

class XPath {
public:
  // Construct with the document root node (xml.root() or xml.prolog())
  explicit XPath(const Node &root);
  XPath() = delete;
  XPath(const XPath &) = delete;
  XPath &operator=(const XPath &) = delete;
  XPath(XPath &&) = delete;
  XPath &operator=(XPath &&) = delete;
  ~XPath();

  // Evaluate an XPath 1.0 expression.
  // Returns a node-set (pointers into the existing Node tree — do not store
  // beyond the lifetime of the XML object).
  [[nodiscard]] std::vector<const Node *> evaluate(std::string_view expression) const;

  // Convenience wrappers — return the string/boolean/number value of the result.
  [[nodiscard]] std::string evaluateString(std::string_view expression) const;
  [[nodiscard]] bool        evaluateBool  (std::string_view expression) const;
  [[nodiscard]] double      evaluateNumber(std::string_view expression) const;

private:
  const std::unique_ptr<XPath_Impl> implementation;
};

} // namespace XML_Lib
```

### Changes to `classes/include/XML.hpp`

Add one new method to the `XML` class:

```cpp
// Evaluate an XPath 1.0 expression against the parsed document.
[[nodiscard]] std::vector<const Node *> xpath(std::string_view expression) const;
```

### Changes to `classes/include/implementation/XML_Impl.hpp`

```cpp
std::vector<const Node *> xpath(std::string_view expression) const;
```

### Changes to `classes/source/XML.cpp`

```cpp
std::vector<const Node *> XML::xpath(const std::string_view expression) const {
  return implementation->xpath(expression);
}
```

### Changes to `classes/source/implementation/xml/XML_Impl.cpp`

```cpp
#include "XPath.hpp"

std::vector<const Node *> XML_Impl::xpath(const std::string_view expression) const {
  XPath xp(root());
  return xp.evaluate(expression);
}
```

### Error type

All XPath errors are thrown as:
```cpp
struct XPath::Error final : std::runtime_error {
  explicit Error(std::string_view message)
    : std::runtime_error(std::string("XPath Error: ").append(message)) {}
};
```
Declared inside `XPath.hpp`.

---

## Section B — Internal Data Model (AST & XPath Types)

**Goal**: Define the in-memory representation of a parsed XPath expression.

### New header: `classes/include/implementation/xpath/XPath_Impl.hpp`

Top-level Pimpl header. Owns parser and evaluator state. Key public methods:

```cpp
class XPath_Impl {
public:
  explicit XPath_Impl(const Node &root);
  std::vector<const Node *> evaluate(std::string_view expression) const;
  std::string  evaluateString(std::string_view expression) const;
  bool         evaluateBool  (std::string_view expression) const;
  double       evaluateNumber(std::string_view expression) const;
private:
  const Node &xmlRoot;
};
```

### XPath data types (internal)

```cpp
// Result of evaluating an XPath expression — one of four types.
enum class XPathResultType { NodeSet, String, Number, Boolean };

struct XPathResult {
  XPathResultType type{ XPathResultType::NodeSet };
  std::vector<const Node *> nodeSet;
  std::string               stringValue;
  double                    numberValue{ 0.0 };
  bool                      boolValue{ false };
};
```

### AST node types

Defined in `classes/include/implementation/xpath/XPath_AST.hpp`:

```
XPathExpr (abstract base, owns children)
├── PathExpr          — /step/step/step, absolute or relative
├── StepExpr          — axis::node-test[predicate]*
├── UnionExpr         — expr | expr
├── BinaryExpr        — expr op expr  (op = +|-|*|div|mod|=|!=|<|>|<=|>=|and|or)
├── UnaryExpr         — -expr
├── FunctionCall      — name(arg, arg, ...)
├── PredicateExpr     — [expr]  (attached to StepExpr)
├── StringLiteral     — "foo" | 'foo'
└── NumberLiteral     — 42 | 3.14
```

### Axes (enum)

```cpp
enum class XPathAxis {
  Child, Parent, Self,
  Ancestor, AncestorOrSelf,
  Descendant, DescendantOrSelf,
  Following, FollowingSibling,
  Preceding, PrecedingSibling,
  Attribute, Namespace
};
```

### Node tests (union-style)

```
NodeTestKind: NameTest ("foo"), Wildcard (*), NodeType (node()|text()|comment()|processing-instruction())
```

---

## Section C — Lexer

**Goal**: Tokenize an XPath expression string into a flat token stream.

### New source file: `classes/source/implementation/xpath/XPath_Lexer.cpp`

Token types:

| Token | Lexeme example |
|-------|---------------|
| `Slash` | `/` |
| `DoubleSlash` | `//` |
| `Dot` | `.` |
| `DotDot` | `..` |
| `At` | `@` |
| `ColonColon` | `::` |
| `LeftBracket` | `[` |
| `RightBracket` | `]` |
| `LeftParen` | `(` |
| `RightParen` | `)` |
| `Comma` | `,` |
| `Pipe` | `\|` |
| `Star` | `*` |
| `Plus` | `+` |
| `Minus` | `-` |
| `Eq` | `=` |
| `Neq` | `!=` |
| `Lt` | `<` |
| `Gt` | `>` |
| `LtEq` | `<=` |
| `GtEq` | `>=` |
| `Name` | `foo`, `xs:bar`, `div`, `and` |
| `StringLiteral` | `"hello"`, `'world'` |
| `NumberLiteral` | `42`, `3.14` |
| `End` | (end of input) |

Disambiguation rules (XPath 1.0 spec §3.7):
- `*` after `::`, `[`, or at start → `Wildcard` node-test; otherwise → multiply operator.
- `div`, `mod`, `and`, `or` are keywords only when preceded by an expression token.

Key function: `std::vector<XPathToken> tokenize(std::string_view expression)`.

---

## Section D — Parser

**Goal**: Convert the token stream into an AST.

### New source file: `classes/source/implementation/xpath/XPath_Parser.cpp`

Grammar (simplified — follows XPath 1.0 spec §3):

```
Expr           ::= OrExpr
OrExpr         ::= AndExpr ('or' AndExpr)*
AndExpr        ::= EqualityExpr ('and' EqualityExpr)*
EqualityExpr   ::= RelationalExpr (('='|'!=') RelationalExpr)*
RelationalExpr ::= AdditiveExpr (('<'|'>'|'<='|'>=') AdditiveExpr)*
AdditiveExpr   ::= MultiplicativeExpr (('+' | '-') MultiplicativeExpr)*
MultiplicativeExpr ::= UnaryExpr (('*'|'div'|'mod') UnaryExpr)*
UnaryExpr      ::= '-' UnaryExpr | UnionExpr
UnionExpr      ::= PathExpr ('|' PathExpr)*
PathExpr       ::= LocationPath | FilterExpr ('/' | '//' RelativeLocationPath)?
LocationPath   ::= RelativeLocationPath | AbsoluteLocationPath
AbsoluteLocationPath ::= '/' RelativeLocationPath? | AbbreviatedAbsoluteLocationPath
AbbreviatedAbsoluteLocationPath ::= '//' RelativeLocationPath
RelativeLocationPath ::= Step (('/'|'//') Step)*
Step           ::= AxisSpecifier NodeTest Predicate* | AbbreviatedStep
AxisSpecifier  ::= AxisName '::' | '@' | (empty = child::)
AbbreviatedStep ::= '.' | '..'
NodeTest       ::= NameTest | 'node()' | 'text()' | 'comment()' | 'processing-instruction()'
NameTest       ::= '*' | NCName | QName
Predicate      ::= '[' Expr ']'
FilterExpr     ::= PrimaryExpr Predicate*
PrimaryExpr    ::= '(' Expr ')' | StringLiteral | NumberLiteral | FunctionCall
FunctionCall   ::= FunctionName '(' (Expr (',' Expr)*)? ')'
```

Abbreviated syntax normalization at parse time:
- `//step` → `/descendant-or-self::node()/step`
- `.`  → `self::node()`
- `..` → `parent::node()`
- `@attr` → `attribute::attr`

Key function: `std::unique_ptr<XPathExpr> parse(std::span<const XPathToken> tokens)`.

---

## Section E — Evaluator

**Goal**: Walk the Node tree executing the AST and returning an `XPathResult`.

### New source file: `classes/source/implementation/xpath/XPath_Evaluator.cpp`

Entry point:
```cpp
XPathResult evaluate(const XPathExpr &expr, const Node &contextNode,
                     size_t contextPosition, size_t contextSize,
                     const Node &documentRoot);
```

#### Axis traversal

Each axis is implemented as a function returning `std::vector<const Node *>`:

| Axis | Behaviour |
|------|-----------|
| `child` | Direct element/content/comment/pi children |
| `descendant` | All descendant nodes (depth-first) |
| `descendant-or-self` | Self + all descendants |
| `parent` | Single parent node (tracked during traversal) |
| `ancestor` | All ancestors up to root |
| `ancestor-or-self` | Self + all ancestors |
| `self` | Just the context node |
| `following-sibling` | All siblings after context |
| `preceding-sibling` | All siblings before context |
| `following` | All nodes in document order after context subtree |
| `preceding` | All nodes in document order before context subtree |
| `attribute` | `XMLAttribute` values exposed as synthetic attribute nodes |
| `namespace` | Namespace declarations on the element |

> **Implementation note**: The Node tree has no parent pointer. During evaluation, maintain a `path` (stack of `const Node *`) representing the current ancestry chain, updated as the evaluator recurses. This replaces the need for stored parent links.

#### Node test evaluation

- `NameTest("foo")` — match elements/attributes whose local name equals `"foo"`.
- `NameTest("*")` — match all nodes on the axis.
- `node()` — match any node type.
- `text()` — match `Content` nodes.
- `comment()` — match `Comment` nodes.
- `processing-instruction()` — match `PI` nodes.

#### Predicate evaluation

A predicate `[expr]` is evaluated for each candidate node in the node-set. If `expr` produces a number, it is compared to the context position. Otherwise, it is cast to boolean.

#### Core function library

Implemented in the same evaluator file:

**Node-set functions:**
| Function | Behaviour |
|----------|-----------|
| `count(node-set)` | Number of nodes in set |
| `last()` | Context size |
| `position()` | 1-based context position |
| `name([node-set])` | Qualified name of first node |
| `local-name([node-set])` | Local part of name |
| `namespace-uri([node-set])` | Namespace URI of first node |

**String functions:**
| Function | Behaviour |
|----------|-----------|
| `string([object])` | String value of object |
| `concat(s, s, ...)` | Concatenate strings |
| `starts-with(s, prefix)` | Boolean |
| `contains(s, sub)` | Boolean |
| `substring(s, pos[, len])` | Substring (1-based) |
| `substring-before(s, sub)` | Substring before first match |
| `substring-after(s, sub)` | Substring after first match |
| `string-length([s])` | Length in characters |
| `normalize-space([s])` | Collapse whitespace |
| `translate(s, from, to)` | Character replacement |

**Number functions:**
| Function | Behaviour |
|----------|-----------|
| `number([object])` | Convert to number (NaN on failure) |
| `sum(node-set)` | Sum of string-values as numbers |
| `floor(n)` | Round down |
| `ceiling(n)` | Round up |
| `round(n)` | Round to nearest integer |

**Boolean functions:**
| Function | Behaviour |
|----------|-----------|
| `boolean(object)` | Convert to boolean |
| `not(boolean)` | Logical negation |
| `true()` | Literal true |
| `false()` | Literal false |
| `lang(string)` | Language match (simplified) |

#### String-value rules (XPath 1.0 §5)

- **Element node**: concatenation of all descendant `Content` text nodes.
- **Attribute node**: the parsed attribute value string.
- **Text node** (`Content`): the text content string.
- **Comment** / **PI**: the raw comment/instruction text.

#### Type coercion

| From \ To | string | number | boolean |
|-----------|--------|--------|---------|
| node-set  | string-value of first node | `number(string(first))` | non-empty = true |
| string    | — | parse as double, NaN on fail | non-empty = true |
| number    | decimal repr, "NaN", "Infinity" | — | 0/NaN = false |
| boolean   | "true" / "false" | 1 / 0 | — |

---

## Section F — New Source/Header Files Summary

```
classes/
  include/
    XPath.hpp                                      ← public API + XPath::Error
    implementation/
      xpath/
        XPath_Impl.hpp                             ← Pimpl header + XPathResult + XPathResultType
        XPath_AST.hpp                              ← AST node types and axis/node-test enums
        XPath_Lexer.hpp                            ← XPathToken + tokenize() declaration
        XPath_Parser.hpp                           ← parse() declaration
  source/
    XPath.cpp                                      ← thin forwarder
    implementation/
      xpath/
        XPath_Impl.cpp                             ← constructor, evaluate() dispatch
        XPath_Lexer.cpp                            ← tokenizer
        XPath_Parser.cpp                           ← recursive-descent parser
        XPath_Evaluator.cpp                        ← AST evaluation, axis traversal, functions
```

Existing files modified:
- `classes/include/XML.hpp` — add `xpath()` method
- `classes/include/implementation/XML_Impl.hpp` — add `xpath()` method
- `classes/source/XML.cpp` — forward to implementation
- `classes/source/implementation/xml/XML_Impl.cpp` — add `#include "XPath.hpp"` + method body
- `CMakeLists.txt` — add 5 new source files to `XML_Lib` target
- `tests/CMakeLists.txt` — add XPath test file

---

## Section G — Tests

**Goal**: Thorough regression-free test coverage using Catch2, following the existing pattern in `tests/source/`.

### New file: `tests/source/xpath/XML_Lib_Tests_XPath.cpp`

Include in `tests/include/XML_Lib_Tests.hpp`:
```cpp
#include "XPath.hpp"
```

#### TEST_CASE 1 — `XPath basic navigation`
- `child::*` / default `child` axis selects direct element children.
- `/root` selects root element from absolute path.
- `//element` selects all descendants named `element`.
- `.` returns the context node itself.
- `..` returns the parent (via ancestor tracking).
- `@attribute` returns attribute node value.
- `*` wildcard matches all child elements.

#### TEST_CASE 2 — `XPath axis traversal`
- `ancestor::foo` — all ancestors named `foo`.
- `descendant::bar` — all descendants named `bar`.
- `following-sibling::*` — all siblings after context.
- `preceding-sibling::*` — all siblings before context.
- `self::node()` — node-type test.
- `descendant-or-self::node()` combined with `//`.

#### TEST_CASE 3 — `XPath predicates`
- `book[1]` — position predicate (first element).
- `book[last()]` — last element.
- `book[title='XML']` — attribute/child value predicate.
- `book[@id]` — existence predicate.
- `book[@id='2']` — attribute equality predicate.
- `book[position() > 1]` — relational predicate.
- `book[price < 20]` — numeric comparison.

#### TEST_CASE 4 — `XPath node-set functions`
- `count(//book)` → correct integer.
- `name(//book)` → "book".
- `local-name(//xs:element)` → "element" (prefix stripped).
- `position()` within predicate context.
- `last()` within predicate context.

#### TEST_CASE 5 — `XPath string functions`
- `string(//title)` — string value of first match.
- `contains(//title, 'XML')` — true/false.
- `starts-with(//title, 'A')` — true/false.
- `string-length(//title)` — correct length.
- `normalize-space(' a  b ')` = "a b".
- `concat('foo', '-', 'bar')` = "foo-bar".
- `substring('abcdef', 2, 3)` = "bcd".
- `translate('aabb', 'ab', 'xy')` = "xxyy".

#### TEST_CASE 6 — `XPath number and boolean functions`
- `number('42')` → 42.0.
- `number('NaN value')` → NaN.
- `sum(//price)` → correct total.
- `floor(3.7)` → 3.
- `ceiling(3.2)` → 4.
- `round(3.5)` → 4; `round(3.4)` → 3.
- `boolean('')` → false; `boolean('x')` → true.
- `not(false())` → true.

#### TEST_CASE 7 — `XPath abbreviated syntax`
- `//foo` equals `/descendant-or-self::node()/child::foo`.
- `./child` equals `self::node()/child::child`.
- `../sibling` navigates to sibling via parent.
- `@id` equals `attribute::id`.

#### TEST_CASE 8 — `XPath union and compound expressions`
- `//title | //author` — union of two node-sets.
- `(//book)[1]` — filtered union result.
- `//book[price > 10 and @category='fiction']`.
- `//book[price < 5 or @category='reference']`.

#### TEST_CASE 9 — `XPath error cases`
- Empty expression throws `XPath::Error`.
- Unmatched `[` throws `XPath::Error`.
- Unknown function throws `XPath::Error`.
- Invalid axis name throws `XPath::Error`.

### Test data files

Create `tests/files/xpath/`:
- `bookstore.xml` — multiple `<book category="...">` with `<title>`, `<author>`, `<price>` children.
- `namespaced.xml` — elements with namespace prefixes for `local-name` / `namespace-uri` tests.
- `mixed.xml` — mixed content (text + elements) for text-node and `normalize-space` tests.

---

## Section H — CMake Wiring

### Root `CMakeLists.txt`

Add to the `XML_Lib` source list:

```cmake
classes/source/XPath.cpp
classes/source/implementation/xpath/XPath_Impl.cpp
classes/source/implementation/xpath/XPath_Lexer.cpp
classes/source/implementation/xpath/XPath_Parser.cpp
classes/source/implementation/xpath/XPath_Evaluator.cpp
```

### `tests/CMakeLists.txt`

Add:
```cmake
source/xpath/XML_Lib_Tests_XPath.cpp
```

After adding the new sources, regenerate with:
```shell
cmake .. -G "Visual Studio 17 2022" -A x64
```

---

## Implementation Order

| Step | Section | Files | Notes |
|------|---------|-------|-------|
| 1 | B | `XPath_AST.hpp`, `XPath_Impl.hpp` | Define all types before writing any .cpp |
| 2 | C | `XPath_Lexer.hpp/cpp` | Tokenizer — no Node tree dependency |
| 3 | D | `XPath_Parser.hpp/cpp` | Depends on lexer tokens and AST types |
| 4 | A | `XPath.hpp/cpp`, `XML_Impl.cpp` | Thin shell, compile-checks headers |
| 5 | E | `XPath_Evaluator.cpp` | Largest file; build axis-by-axis |
| 6 | F | `XPath_Impl.cpp` | Wire lexer → parser → evaluator |
| 7 | G | Test data + test source | One TEST_CASE at a time with build checks |
| 8 | H | CMake files | Add sources, regenerate, full build |

---

## Estimated Scope

| Component | Approx. lines |
|-----------|--------------|
| Lexer | ~200 |
| AST headers | ~150 |
| Parser | ~450 |
| Evaluator (axes) | ~350 |
| Evaluator (functions) | ~400 |
| Public API + Pimpl glue | ~100 |
| Tests | ~400 |
| **Total** | **~2050** |

---

## References

- [W3C XPath 1.0 Specification](https://www.w3.org/TR/xpath-10/)
- [W3C XPath Data Model](https://www.w3.org/TR/xpath-datamodel/)
