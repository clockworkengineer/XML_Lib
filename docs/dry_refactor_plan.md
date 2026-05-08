# XML_Lib — Concrete DRY Refactor Plan

This document identifies concrete code-duplication patterns across the XML_Lib codebase,
provides the exact files and line numbers involved, and prescribes specific refactoring
actions ordered by estimated impact.

---

## Summary

| # | Category | Files Affected | Duplicated Instances | Estimated Savings |
|---|----------|---------------|---------------------|-------------------|
| 1 | Variant rule-of-five boilerplate | 9 variant headers | 9× identical copy/move/dtor blocks | ~54 lines removed |
| 2 | `IAction` const/non-const method pairs | `IAction.hpp` | 12× doubled virtual methods | ~24 declarations removed |
| 3 | `CMakeLists.txt` include-directory blocks | `tests/CMakeLists.txt` | 3× identical 15-entry PRIVATE list | ~30 lines removed |
| 4 | `struct Error` pattern | 7 files | 7× identical constructor body shape | ~21 lines removed + typo fixed |
| 5 | Validator pimpl wrapper | `DTD_Validator.cpp/hpp`, `XSD_Validator.cpp/hpp` | 2× identical 5-method wrapper | ~20 lines removed |
| 6 | `parseValue()` near-duplication | `XML_Parse.cpp` | 2× 90%-duplicate overloads | ~25 lines removed |
| 7 | DTD "delimiter loop" pattern | 3 DTD parse files | 5+ near-identical parse loops | ~40 lines removed |
| 8 | XSD restriction facet `if-else` chain | `XSD_Validator_Impl_Parse.cpp` | 12 identical emplace branches | ~24 lines removed |

---

## Refactor 1 — Variant Rule-of-Five Boilerplate

### Problem

Every `Variant` subtype defines the same five-line block:

```cpp
// Repeated identically in: XML_CDATA.hpp, XML_Comment.hpp, XML_Content.hpp,
// XML_Declaration.hpp, XML_EntityReference.hpp, XML_PI.hpp, XML_Prolog.hpp,
// XML_Root.hpp, XML_Self.hpp
TypeName(const TypeName &other) = delete;
TypeName &operator=(const TypeName &other) = delete;
TypeName(TypeName &&other) = default;
TypeName &operator=(TypeName &&other) = default;
~TypeName() override = default;
```

This is copy-pasted **9 times**, once per variant header. Only the class name changes.

### Affected Files

| File | Lines |
|------|-------|
| `classes/include/implementation/variants/XML_CDATA.hpp` | 9–13 |
| `classes/include/implementation/variants/XML_Comment.hpp` | 9–13 |
| `classes/include/implementation/variants/XML_Content.hpp` | ~9–13 |
| `classes/include/implementation/variants/XML_Declaration.hpp` | ~9–13 |
| `classes/include/implementation/variants/XML_EntityReference.hpp` | ~9–13 |
| `classes/include/implementation/variants/XML_PI.hpp` | 11–15 |
| `classes/include/implementation/variants/XML_Prolog.hpp` | 9–13 |
| `classes/include/implementation/variants/XML_Root.hpp` | 13–17 |
| `classes/include/implementation/variants/XML_Self.hpp` | ~9–13 |

### Recommended Fix

Add a `VARIANT_NO_COPY` macro to `XML_Variant.hpp` (or a new
`classes/include/implementation/variants/XML_VariantBase.hpp`):

```cpp
// In XML_Variant.hpp (or a shared macro header)
#define XML_LIB_NO_COPY_MOVE_DTOR(TypeName)          \
  TypeName(const TypeName &) = delete;               \
  TypeName &operator=(const TypeName &) = delete;    \
  TypeName(TypeName &&) = default;                   \
  TypeName &operator=(TypeName &&) = default;        \
  ~TypeName() override = default
```

Each variant then becomes:

```cpp
struct CDATA final : Variant
{
  explicit CDATA(const std::string_view &cdata) : Variant(Type::cdata), cdata(cdata) {}
  XML_LIB_NO_COPY_MOVE_DTOR(CDATA);
  [[nodiscard]] std::string_view value() const { return cdata; }
  [[nodiscard]] std::string getContents() const override { return cdata; }
private:
  std::string cdata;
};
```

**Note:** `Variant` base already defines the same five lines at
`classes/include/implementation/variants/XML_Variant.hpp` lines 14–18, so this
macro would unify all ten occurrences.

---

## Refactor 2 — `IAction` Const/Non-Const Method Pairs

### Problem

Every visitor method in `IAction.hpp` is declared twice — once mutable and once const —
with an empty default body each time. There are 12 event types, giving 24 declarations
for what is conceptually 12 events.

```cpp
// classes/include/interface/IAction.hpp  lines 32–90
virtual void onNode([[maybe_unused]] Node &Node){ }
virtual void onNode([[maybe_unused]] const Node &Node){ }

virtual void onCDATA([[maybe_unused]] Node &Node){ }
virtual void onCDATA([[maybe_unused]] const Node &Node){ }
// ... repeated for: onComment, onContent, onDeclaration, onDTD,
//     onElement, onEntityReference, onPI, onProlog, onRoot, onSelf
```

The `traverseNodes<T>` template in `XML_Impl.hpp` already unifies the two traversal
paths via template deduction (`T` resolves to `Node` or `const Node`), so the interface
duplication is redundant boilerplate.

### Affected Files

| File | Lines |
|------|-------|
| `classes/include/interface/IAction.hpp` | 32–93 |

### Recommended Fix

Add a macro to `IAction.hpp` that declares both overloads from a single line:

```cpp
#define IACTION_EVENT(name)                                                \
  virtual void on##name([[maybe_unused]] Node &N) { }                     \
  virtual void on##name([[maybe_unused]] const Node &N) { }

class IAction {
  // ...
  IACTION_EVENT(Node)
  IACTION_EVENT(CDATA)
  IACTION_EVENT(Comment)
  IACTION_EVENT(Content)
  IACTION_EVENT(Declaration)
  IACTION_EVENT(DTD)
  IACTION_EVENT(Element)
  IACTION_EVENT(EntityReference)
  IACTION_EVENT(PI)
  IACTION_EVENT(Prolog)
  IACTION_EVENT(Root)
  IACTION_EVENT(Self)
};
#undef IACTION_EVENT
```

This reduces 48 lines of declarations to 14 lines and ensures that adding a new node
type requires exactly one line in two places (add the `Type` enum value and one
`IACTION_EVENT` line).

---

## Refactor 3 — `tests/CMakeLists.txt` Include-Directory Block Repeated 3×

### Problem

The 15-entry `target_include_directories(...PRIVATE ...)` block is copy-pasted in
`tests/CMakeLists.txt` for all three test executables:

```cmake
# Appears at approximately lines 82–95, 131–144, and 174–187
target_include_directories(${<EXECUTABLE_VAR>} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../build
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/interface
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/converter
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/data
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/entity
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/io
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/parser
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/validator
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/node
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/stringify
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/xpath
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/variants
)
```

This block is identical for `XML_Lib_Unit_Tests`, `XML_Lib_Embedded_Unit_Tests`, and
`XML_Lib_Performance_Tests`.

### Recommended Fix

Extract to a CMake list variable near the top of `tests/CMakeLists.txt`, before any
`add_executable` calls:

```cmake
# Common implementation headers shared by all test targets
set(XML_LIB_TEST_PRIVATE_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/../build
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/interface
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/converter
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/data
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/entity
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/io
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/parser
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/validator
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/node
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/stringify
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/xpath
    ${CMAKE_CURRENT_SOURCE_DIR}/../classes/include/implementation/variants
)
```

Then replace all three blocks:

```cmake
target_include_directories(${TEST_EXECUTABLE}          PRIVATE ${XML_LIB_TEST_PRIVATE_INCLUDES})
target_include_directories(${EMBEDDED_TEST_EXECUTABLE} PRIVATE ${XML_LIB_TEST_PRIVATE_INCLUDES})
target_include_directories(${PERFORMANCE_EXECUTABLE}   PRIVATE ${XML_LIB_TEST_PRIVATE_INCLUDES})
```

The same pattern applies to the `PUBLIC` include block (3× identical 3-entry list) and
could be extracted as `XML_LIB_TEST_PUBLIC_INCLUDES`.

---

## Refactor 4 — `struct Error` Construction Pattern Repeated 7×

### Problem

Seven files each define a nested `struct Error` using the same constructor body shape:

```cpp
struct Error final : std::runtime_error
{
  explicit Error(const std::string_view &message)
    : std::runtime_error(std::string("<ClassName> Error: ").append(message)) {}
};
```

Occurrences:

| File | Prefix String |
|------|---------------|
| `classes/include/interface/IAction.hpp` line 22 | `"IAction Error: "` |
| `classes/include/implementation/common/XML_Error.hpp` line 17 | `"XML Error: "` |
| `classes/include/implementation/variants/XML_DTD.hpp` line 14 | `"DTD Error: "` |
| `classes/include/implementation/io/XML_BufferSource.hpp` line 22 | `"BufferSource Error: "` |
| `classes/include/implementation/io/XML_FileSource.hpp` line 17 | `"FileSource Error: "` |
| `classes/include/implementation/io/XML_BufferDestination.hpp` line 17 | `"BufferDestination Error: "` |
| `classes/include/implementation/io/XML_FileDestination.hpp` line 18 | `"FileSestination Error: "` ← **typo** |

The `FileDestination` entry also has a typo: `"FileSestination"` (S instead of D).

### Recommended Fix

Add a helper macro to `XML_Error.hpp` immediately above the `struct Error` definitions:

```cpp
// In classes/include/implementation/common/XML_Error.hpp
#define XML_LIB_DEFINE_ERROR(Prefix)                                     \
  struct Error final : std::runtime_error {                              \
    explicit Error(const std::string_view &message)                      \
      : std::runtime_error(std::string(Prefix " Error: ").append(message)) {} \
  }
```

Usage in each class:

```cpp
// IAction.hpp
XML_LIB_DEFINE_ERROR("IAction");

// XML_BufferSource.hpp
XML_LIB_DEFINE_ERROR("BufferSource");

// XML_FileDestination.hpp  (also fixes the typo)
XML_LIB_DEFINE_ERROR("FileDestination");
```

**Important:** Fix the `FileDestination` typo (`"FileSestination"` → `"FileDestination"`)
as part of this refactor. That typo currently makes error messages misleading.

---

## Refactor 5 — Validator Pimpl Wrapper Duplicated in DTD and XSD

### Problem

`DTD_Validator.cpp` and `XSD_Validator.cpp` are structurally identical files: each is a
thin pimpl wrapper that forwards every public method to `implementation->method(...)`.

**`DTD_Validator.cpp` lines 18–40:**
```cpp
DTD_Validator::DTD_Validator(Node &xNode) : implementation(std::make_unique<DTD_Impl>(xNode)) {}
DTD_Validator::~DTD_Validator() = default;
void DTD_Validator::parse(ISource &source)           { implementation->parse(source); }
void DTD_Validator::stringify(IDestination &dest)    { implementation->stringify(dest); }
void DTD_Validator::validate(const Node &xNode)      { implementation->validate(xNode); }
```

**`XSD_Validator.cpp` lines 18–40 — identical structure:**
```cpp
XSD_Validator::XSD_Validator(Node &xNode) : implementation(std::make_unique<XSD_Impl>(xNode)) {}
XSD_Validator::~XSD_Validator() = default;
void XSD_Validator::parse(ISource &source)           { implementation->parse(source); }
void XSD_Validator::stringify(IDestination &dest)    { implementation->stringify(dest); }
void XSD_Validator::validate(const Node &xNode)      { implementation->validate(xNode); }
```

The corresponding header declarations in `DTD_Validator.hpp` and `XSD_Validator.hpp` are
also structurally identical.

### Affected Files

| File | Role |
|------|------|
| `classes/source/implementation/dtd/DTD_Validator.cpp` | DTD wrapper impl |
| `classes/source/implementation/xsd/XSD_Validator.cpp` | XSD wrapper impl |
| `classes/include/DTD_Validator.hpp` | DTD public header |
| `classes/include/XSD_Validator.hpp` | XSD public header |

### Recommended Fix

Create a CRTP or template base in a new header
`classes/include/implementation/validator/ValidatorBase.hpp`:

```cpp
#pragma once
#include "IValidator.hpp"
#include <memory>

namespace XML_Lib {

template<typename Impl>
class ValidatorBase : public IValidator
{
public:
  explicit ValidatorBase(Node &xNode) : impl(std::make_unique<Impl>(xNode)) {}
  ~ValidatorBase() override = default;
  void parse(ISource &source) override           { impl->parse(source); }
  void stringify(IDestination &dest) override    { impl->stringify(dest); }
  void validate(const Node &xNode) override      { impl->validate(xNode); }
protected:
  std::unique_ptr<Impl> impl;
};

} // namespace XML_Lib
```

Each validator header becomes:

```cpp
// DTD_Validator.hpp
#include "implementation/validator/ValidatorBase.hpp"
class DTD_Validator final : public ValidatorBase<DTD_Impl> {
public:
  explicit DTD_Validator(Node &xNode);
};
```

And each `.cpp` file is reduced to just the constructor definition (one line each).

---

## Refactor 6 — `parseValue()` Two Near-Duplicate Overloads

### Problem

`classes/source/implementation/common/XML_Parse.cpp` contains two `parseValue`
overloads that are ~90% identical. The only difference is entity-reference handling:

```cpp
// Overload 1 (with entity mapper) — lines ~79–105
XMLValue parseValue(ISource &source, IEntityMapper &entityMapper) {
  // ...
  while (source.more() && source.current() != quote) {
    if (XMLValue character{ parseCharacter(source) }; character.isEntityReference()) {
      XMLValue entityReference{ entityMapper.map(character) };
      unparsed += entityReference.getUnparsed();
      parsed   += entityReference.getParsed();
    } else {
      unparsed += character.getUnparsed();
      parsed   += character.getParsed();
    }
  }
  // ...
}

// Overload 2 (no entity mapper) — lines ~108–130
XMLValue parseValue(ISource &source) {
  // ...
  while (source.more() && source.current() != quote) {
    XMLValue character{ parseCharacter(source) };
    unparsed += character.getUnparsed();
    parsed   += character.getParsed();
  }
  // ...
}
```

Both overloads share: quote detection, string reserve, loop termination, `source.next()`,
`source.ignoreWS()`, and the `XMLValue{...}` return. Only the inner loop body differs.

### Recommended Fix

Extract a private helper that takes an optional mapper via `std::optional<std::reference_wrapper<IEntityMapper>>` or a simple lambda:

```cpp
// Replace both overloads with:
static XMLValue parseValueImpl(ISource &source, IEntityMapper *mapper) {
  if (source.current() != '\'' && source.current() != '"') {
    XML_LIB_THROW(SyntaxError(source.getPosition(), "Expected quote character."));
  }
  std::string unparsed, parsed;
  unparsed.reserve(32);
  parsed.reserve(32);
  const Char quote = source.current();
  source.next();
  while (source.more() && source.current() != quote) {
    XMLValue character{ parseCharacter(source) };
    if (mapper && character.isEntityReference()) {
      XMLValue ref{ mapper->map(character) };
      unparsed += ref.getUnparsed();
      parsed   += ref.getParsed();
    } else {
      unparsed += character.getUnparsed();
      parsed   += character.getParsed();
    }
  }
  source.next();
  source.ignoreWS();
  return XMLValue{ unparsed, parsed, static_cast<char>(quote) };
}

XMLValue parseValue(ISource &source, IEntityMapper &mapper) {
  return parseValueImpl(source, &mapper);
}
XMLValue parseValue(ISource &source) {
  return parseValueImpl(source, nullptr);
}
```

---

## Refactor 7 — DTD "Delimiter Loop" Pattern Repeated 5+ Times

### Problem

The pattern "collect a delimited list of items by parsing an opening character,
calling a sub-parser, then looping on a separator character" appears at least five
times across the three DTD parse files:

**Pattern in `DTD_Validator_Impl_Parse.cpp` — `parseAttributeEnumerationType`:**
```cpp
enumerationType += parseName(source);
while (source.more() && source.current() == '|') {
  enumerationType += toUtf8(source.current());
  source.next();
  source.ignoreWS();
  enumerationType += parseName(source);
}
```

**Same structure in `DTD_Validator_Impl_Parse_Content.cpp` — `parseElementChoice`:**
```cpp
parseElementCP(contentSpecSource, contentSpecDestination);
while (contentSpecSource.more() && contentSpecSource.current() == '|') {
  contentSpecDestination.add("|");
  parseElementCP(contentSpecSource, contentSpecDestination);
}
```

And analogous loops exist for `,`-separated sequences and `*`-quantified groups.

### Recommended Fix

Extract a templated helper in
`classes/source/implementation/dtd/DTD_Validator_Impl_Parse.cpp` (file-local, or to a
shared DTD parsing utility header):

```cpp
// Generic delimiter loop: parse first item, then loop on separator
template<typename ParseFn, typename AppendFn>
static void parseDelimitedList(ISource &source, char separator, ParseFn parseItem, AppendFn append)
{
  parseItem(source);
  while (source.more() && source.current() == separator) {
    append(toUtf8(separator));
    source.next();
    source.ignoreWS();
    parseItem(source);
  }
}
```

Each call site reduces from 6+ lines to 1–3 lines.

---

## Refactor 8 — XSD Restriction Facet `if-else` Chain

### Problem

`XSD_Validator_Impl_Parse.cpp` — `XSD_Impl::parseRestriction()` contains 12
nearly-identical consecutive `if-else` branches:

```cpp
if (tag == "minLength") {
  st.restrictions.emplace_back(XSD_Restriction{ XSD_Restriction::Facet::minLength, std::string(val) });
} else if (tag == "maxLength") {
  st.restrictions.emplace_back(XSD_Restriction{ XSD_Restriction::Facet::maxLength, std::string(val) });
} else if (tag == "length") {
  st.restrictions.emplace_back(XSD_Restriction{ XSD_Restriction::Facet::length, std::string(val) });
}
// ... 9 more identical branches ...
```

Every branch has exactly the same structure: `emplace_back(XSD_Restriction{ Facet::<tag>, std::string(val) })`.
Only the string key and the `Facet` enum value differ.

### Recommended Fix

Replace with a lookup table:

```cpp
void XSD_Impl::parseRestriction(const Node &restrictNode, XSD_SimpleType &st)
{
  static const std::unordered_map<std::string_view, XSD_Restriction::Facet> kFacetMap {
    { "minLength",    XSD_Restriction::Facet::minLength    },
    { "maxLength",    XSD_Restriction::Facet::maxLength    },
    { "length",       XSD_Restriction::Facet::length       },
    { "pattern",      XSD_Restriction::Facet::pattern      },
    { "enumeration",  XSD_Restriction::Facet::enumeration  },
    { "minInclusive", XSD_Restriction::Facet::minInclusive },
    { "maxInclusive", XSD_Restriction::Facet::maxInclusive },
    { "minExclusive", XSD_Restriction::Facet::minExclusive },
    { "maxExclusive", XSD_Restriction::Facet::maxExclusive },
    { "totalDigits",  XSD_Restriction::Facet::totalDigits  },
    { "fractionDigits", XSD_Restriction::Facet::fractionDigits },
    { "whiteSpace",   XSD_Restriction::Facet::whiteSpace   },
  };

  st.baseType = resolveType(attrValue(restrictNode, "base"));
  for (const auto &childRef : childElements(restrictNode)) {
    const auto &child = childRef.get();
    const auto tag = localTag(child);
    const auto val = attrValue(child, "value");
    if (const auto it = kFacetMap.find(tag); it != kFacetMap.end()) {
      st.restrictions.emplace_back(XSD_Restriction{ it->second, std::string(val) });
    }
  }
}
```

This reduces ~30 lines to ~20 lines and makes adding a new facet a single-line change in
the map.

---

## Implementation Priority Order

Apply refactors in this order to minimise merge conflicts and maximise value per effort:

1. **Refactor 3** — CMakeLists.txt variable extraction (pure build system, no risk, immediate win)
2. **Refactor 4** — Error macro + fix `FileDestination` typo (all headers, no logic change)
3. **Refactor 8** — XSD facet map (self-contained, high legibility gain)
4. **Refactor 6** — `parseValue` private helper (small file, easy to test)
5. **Refactor 1** — Variant rule-of-five macro (many files, but mechanical and safe)
6. **Refactor 2** — `IAction` event macro (interface change; verify all override call sites compile)
7. **Refactor 7** — DTD delimiter loop helper (medium complexity, needs careful scoping)
8. **Refactor 5** — `ValidatorBase` template (introduce new header, touches public API surface)

---

## Bonus — Pre-existing Typo

`classes/include/implementation/io/XML_FileDestination.hpp` line ~18:

```cpp
// BUG: "FileSestination" should be "FileDestination"
: std::runtime_error(std::string("FileSestination Error: ").append(message))
```

This typo appears in error messages at runtime and makes diagnostics confusing. Fix as
part of Refactor 4.
