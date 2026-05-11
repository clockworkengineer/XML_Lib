# Concrete DRY Refactor Plan – Pass 3

Full-library analysis after completing all tasks in passes 1 and 2.  
Nine concrete, independently testable tasks ordered by lines-eliminated / risk ratio.

---

## Task A — Cache `xDTD.getElement(xElement.name())` in `checkContentSpecification`

**File:** `classes/source/implementation/dtd/DTD_Validator_Impl_Validator.cpp`  
**Function:** `DTD_Impl::checkContentSpecification`

### Problem

`xDTD.getElement(xElement.name())` is called **five times** in a single function body,
performing five separate map lookups on the same key.

```cpp
void DTD_Impl::checkContentSpecification(const Node &xNode) const
{
  const auto &xElement = NRef<Element>(xNode);
  if (xDTD.getElementCount() == 0) { return; }
  if (xDTD.getElement(xElement.name()).content.getParsed() == "((<#PCDATA>))") { // lookup 1
    ...
  }
  if (xDTD.getElement(xElement.name()).content.getParsed() == "EMPTY") {         // lookup 2
    ...
  }
  if (xDTD.getElement(xElement.name()).content.getParsed() == "ANY") { return; } // lookup 3
  const std::regex match{ xDTD.getElement(xElement.name()).content.getParsed() };// lookup 4
  ...
  if (!std::regex_match(elements, match)) {
    elementError(xElement,
      "does not conform to the content specification "
        + xDTD.getElement(xElement.name()).content.getUnparsed() + "."); // lookup 5
  }
}
```

### Fix

Add `const auto &elemDecl = xDTD.getElement(xElement.name());` immediately after the
early-return guard and use `elemDecl` throughout.

```cpp
void DTD_Impl::checkContentSpecification(const Node &xNode) const
{
  const auto &xElement = NRef<Element>(xNode);
  if (xDTD.getElementCount() == 0) { return; }
  const auto &elemDecl = xDTD.getElement(xElement.name());   // single lookup
  if (elemDecl.content.getParsed() == "((<#PCDATA>))") {
    if (!checkIsPCDATA(xNode)) { elementError(xElement, "does not contain just any parsable data."); }
    return;
  }
  if (elemDecl.content.getParsed() == "EMPTY") {
    if (!checkIsEMPTY(xNode)) { elementError(xElement, "is not empty."); }
    return;
  }
  if (elemDecl.content.getParsed() == "ANY") { return; }
  const std::regex match{ elemDecl.content.getParsed() };
  std::string elements;
  for (auto &child : xElement.getChildren()) {
    if (isA<Element>(child) || isA<Self>(child)) {
      elements += "<" + NRef<Element>(child).name() + ">";
    } else if (isA<Content>(child)) {
      if (!NRef<Content>(child).isWhiteSpace()) { elements += "<#PCDATA>"; }
    }
  }
  if (!std::regex_match(elements, match)) {
    elementError(xElement,
      "does not conform to the content specification " + elemDecl.content.getUnparsed() + ".");
  }
}
```

**Lines eliminated:** 4 (4 redundant calls replaced by references).  
**Risk:** Minimal — pure local caching, no behaviour change.

---

## Task B — Extract `buildEnumerationSet` for NOTATION / ENUMERATION branches in `checkAttributeType`

**File:** `classes/source/implementation/dtd/DTD_Validator_Impl_Validator.cpp`  
**Function:** `DTD_Impl::checkAttributeType`

### Problem

The `notation` and `enumeration` branches both contain an **identical 3-line
set-build block** followed by `contains` check:

```cpp
// notation branch
std::set<std::string> notations;
for (auto &notation : splitString(attribute.enumeration.substr(1, attribute.enumeration.size() - 2), '|')) {
  notations.insert(notation);
}
if (!notations.contains(elementAttribute.getParsed())) { ... }

// enumeration branch
std::set<std::string> enumeration;
for (auto &option : splitString(attribute.enumeration.substr(1, attribute.enumeration.size() - 2), '|')) {
  enumeration.insert(option);
}
if (!enumeration.contains(elementAttribute.getParsed())) { ... }
```

### Fix

Add a file-local helper before `checkAttributeType`:

```cpp
static std::set<std::string> buildEnumerationSet(const std::string &enumStr)
{
  std::set<std::string> result;
  for (auto &item : splitString(enumStr.substr(1, enumStr.size() - 2), '|')) {
    result.insert(item);
  }
  return result;
}
```

Replace both branches:

```cpp
} else if ((attribute.type & DTD::AttributeType::notation) != 0) {
  if (!buildEnumerationSet(attribute.enumeration).contains(elementAttribute.getParsed())) {
    elementError(xElement,
      "NOTATION attribute '" + attribute.name + "' value '" + elementAttribute.getParsed() + "' is not defined.");
  }
} else if ((attribute.type & DTD::AttributeType::enumeration) != 0) {
  if (!buildEnumerationSet(attribute.enumeration).contains(elementAttribute.getParsed())) {
    elementError(xElement,
      "attribute '" + attribute.name + "' contains invalid enumeration value '" + elementAttribute.getParsed() + "'.");
  }
}
```

**Lines eliminated:** 6 (two 3-line build blocks → two 1-line calls).  
**Risk:** Minimal.

---

## Task C — Unify ENTITY / ENTITIES presence check in `checkAttributeType`

**File:** `classes/source/implementation/dtd/DTD_Validator_Impl_Validator.cpp`  
**Function:** `DTD_Impl::checkAttributeType`

### Problem

The ENTITIES branch is the ENTITY check applied inside a loop; the inner check body
is copy-pasted:

```cpp
} else if ((attribute.type & DTD::AttributeType::entity) != 0) {
  if (!xDTD.getEntityMapper().isPresent("&" + elementAttribute.getParsed() + ";")) {
    elementError(xElement,
      "ENTITY attribute '" + attribute.name + "' value '" + elementAttribute.getParsed() + "' is not defined.");
  }
} else if ((attribute.type & DTD::AttributeType::entities) != 0) {
  for (const auto &entity : splitString(elementAttribute.getParsed(), ' ')) {
    if (!xDTD.getEntityMapper().isPresent("&" + entity + ";")) {
      elementError(xElement,
        "ENTITIES attribute '" + attribute.name + "' value '" + entity + "' is not defined.");
    }
  }
}
```

### Fix

Add a local lambda (or file-scope static helper) inside `checkAttributeType`:

```cpp
const auto checkEntity = [&](const std::string &entityName) {
  if (!xDTD.getEntityMapper().isPresent("&" + entityName + ";")) {
    elementError(xElement,
      "ENTITY attribute '" + attribute.name + "' value '" + entityName + "' is not defined.");
  }
};
```

Then the two branches become:

```cpp
} else if ((attribute.type & DTD::AttributeType::entity) != 0) {
  checkEntity(elementAttribute.getParsed());
} else if ((attribute.type & DTD::AttributeType::entities) != 0) {
  for (const auto &entity : splitString(elementAttribute.getParsed(), ' ')) { checkEntity(entity); }
}
```

**Lines eliminated:** 6.  
**Risk:** Minimal.

---

## Task D — Merge `parseElementChoice` / `parseElementSequence` via a shared body helper

**File:** `classes/source/implementation/dtd/DTD_Validator_Impl_Parse_Content.cpp`  
**Functions:** `DTD_Impl::parseElementChoice`, `DTD_Impl::parseElementSequence`

### Problem

The two functions are structurally identical; they differ only in the delimiter
character (`'|'` vs `','`) and whether the separator lambda emits a character:

```cpp
void DTD_Impl::parseElementChoice(ISource &src, IDestination &dst)
{
  if (src.current() != '(') { throw SyntaxError("Invalid element content specification."); }
  dst.add("(");
  parseElementCP(src, dst);
  parseDelimitedList(src, '|',
    [&](ISource &) { dst.add("|"); },
    [&](ISource &) { parseElementCP(src, dst); });
  if (src.current() != ')') { throw SyntaxError("Invalid element content specification."); }
  dst.add(")");
  src.next();
  src.ignoreWS();
}

void DTD_Impl::parseElementSequence(ISource &src, IDestination &dst)
{
  if (src.current() != '(') { throw SyntaxError("Invalid element content specification."); }
  dst.add("(");
  parseElementCP(src, dst);
  parseDelimitedList(src, ',',
    [&](ISource &) {},                                   // ← only difference
    [&](ISource &) { parseElementCP(src, dst); });
  if (src.current() != ')') { throw SyntaxError("Invalid element content specification."); }
  dst.add(")");
  src.next();
  src.ignoreWS();
}
```

### Fix

Extract a shared body helper in the anonymous namespace:

```cpp
namespace {
void parseGroupBody(ISource &src,
  IDestination &dst,
  char delimiter,
  std::function<void(ISource &)> separatorAction,
  std::function<void(ISource &, IDestination &)> parseCP)
{
  if (src.current() != '(') { throw SyntaxError("Invalid element content specification."); }
  dst.add("(");
  parseCP(src, dst);
  parseDelimitedList(src, delimiter, separatorAction,
    [&](ISource &) { parseCP(src, dst); });
  if (src.current() != ')') { throw SyntaxError("Invalid element content specification."); }
  dst.add(")");
  src.next();
  src.ignoreWS();
}
} // namespace
```

Then the two public functions thin-wrap it:

```cpp
void DTD_Impl::parseElementChoice(ISource &src, IDestination &dst)
{
  parseGroupBody(src, dst, '|',
    [&](ISource &) { dst.add("|"); },
    [&](ISource &s, IDestination &d) { parseElementCP(s, d); });
}

void DTD_Impl::parseElementSequence(ISource &src, IDestination &dst)
{
  parseGroupBody(src, dst, ',',
    [&](ISource &) {},
    [&](ISource &s, IDestination &d) { parseElementCP(s, d); });
}
```

**Lines eliminated:** ~14 (the full body duplication).  
**Risk:** Low — structure identical, parameter types are lambdas already used in the original.

---

## Task E — Extract `tryParseCommentOrPI` in `Default_Parser.cpp`

**File:** `classes/source/implementation/xml/parser/Default_Parser.cpp`  
**Functions:** `parseCommentsPIAndWhiteSpace`, `parseElementInternal`

### Problem

Both functions open with the same two `<!--` / `<?` match-and-dispatch blocks:

```cpp
// in parseCommentsPIAndWhiteSpace:
if (source.match("<!--")) { xProlog.addChild(parseComment(source)); return true; }
if (source.match("<?"))   { xProlog.addChild(parsePI(source));      return true; }

// in parseElementInternal:
if (source.match("<!--")) { xNode.addChild(parseComment(source)); }
else if (source.match("<?")) { xNode.addChild(parsePI(source)); }
else if ...
```

### Fix

Add a private static helper:

```cpp
static bool tryParseCommentOrPI(ISource &source, Node &xNode, Default_Parser &parser)
{
  if (source.match("<!--")) { xNode.addChild(parser.parseComment(source)); return true; }
  if (source.match("<?"))   { xNode.addChild(parser.parsePI(source));      return true; }
  return false;
}
```

`parseCommentsPIAndWhiteSpace` becomes:

```cpp
bool Default_Parser::parseCommentsPIAndWhiteSpace(ISource &source, Node &xProlog)
{
  if (tryParseCommentOrPI(source, xProlog, *this)) { return true; }
  if (source.isWS()) { parseWhiteSpaceToContent(source, xProlog); return true; }
  return false;
}
```

`parseElementInternal` replaces its opening two branches with a single call:

```cpp
void Default_Parser::parseElementInternal(ISource &source, Node &xNode, IEntityMapper &entityMapper)
{
  if (tryParseCommentOrPI(source, xNode, *this)) { return; }
  else if (source.match("<![CDATA[")) { ... }
  ...
}
```

**Lines eliminated:** ~4 (two duplicated branches).  
**Risk:** Low.

---

## Task F — Replace heap-allocated encoding set with `constexpr` array in `parseDeclaration`

**File:** `classes/source/implementation/xml/parser/Default_Parser.cpp`  
**Function:** `Default_Parser::parseDeclaration`

### Problem

The allowed-encodings set is declared as `static const std::set<std::string>` which is
heap-allocated on first use:

```cpp
static const std::set<std::string> allowedEncodings = {
  "UTF-8", "UTF-16", "ISO-10646-UCS-2", "ISO-10646-UCS-4",
  "ISO-8859-1", "ISO-8859-2", "ISO-8859-3", "ISO-8859-4",
  "ISO-8859-5", "ISO-8859-6", "ISO-8859-7", "ISO-8859-8",
  "ISO-8859-9", "ISO-2022-JP", "ISO-2022-JP-2", "ISO-2022-KR",
  "ISO-2022-CN", "EUC-JP", "EUC-KR", "EUC-CN",
  "SHIFT_JIS", "US-ASCII" };
encoding = parseDeclarationAttribute(source, "encoding", allowedEncodings);
```

But `parseDeclarationAttribute` takes `const std::set<std::string> &values` and calls
`values.contains(value)`. The function signature is what forces the set type.

### Fix

Change `parseDeclarationAttribute` to accept `std::initializer_list<std::string_view>`
(or `std::span<const std::string_view>`) and use `std::ranges::any_of` for the
membership test. Replace the static set with a `static constexpr` array:

```cpp
// In header / file scope:
static constexpr std::array<std::string_view, 22> kAllowedEncodings{
  "UTF-8", "UTF-16", "ISO-10646-UCS-2", "ISO-10646-UCS-4",
  "ISO-8859-1", "ISO-8859-2", "ISO-8859-3", "ISO-8859-4",
  "ISO-8859-5", "ISO-8859-6", "ISO-8859-7", "ISO-8859-8",
  "ISO-8859-9", "ISO-2022-JP", "ISO-2022-JP-2", "ISO-2022-KR",
  "ISO-2022-CN", "EUC-JP", "EUC-KR", "EUC-CN",
  "SHIFT_JIS", "US-ASCII"
};
```

Update `parseDeclarationAttribute` signature to:

```cpp
std::string Default_Parser::parseDeclarationAttribute(ISource &source,
  const std::string_view &name,
  std::span<const std::string_view> values)
```

And change the `values.contains(value)` call to:

```cpp
if (!std::ranges::any_of(values, [&](const std::string_view sv) { return sv == value; })) {
  XML_LIB_THROW(SyntaxError(...));
}
```

The two other call-sites (`"version"` and `"standalone"`) pass small inline
`std::initializer_list`-style spans.

**Lines eliminated:** 0 (same count), but heap allocation removed.  
**Risk:** Low — purely a storage-type change; semantics identical.

---

## Task G — Extract translate-and-parse helper in `parseExternalContent`

**File:** `classes/source/implementation/dtd/DTD_Validator_Impl_Parse_External.cpp`  
**Function:** `DTD_Impl::parseExternalContent`

### Problem

Four consecutive blocks each contain the same two-line translate+BufferSource setup,
differing only in the parse function called:

```cpp
if (source.match("<!ENTITY")) {
  BufferSource dtdTranslatedSource(xDTD.getEntityMapper().translate(parseTagBody(source)));
  parseEntity(dtdTranslatedSource);
} else if (source.match("<!ELEMENT")) {
  BufferSource dtdTranslatedSource(xDTD.getEntityMapper().translate(parseTagBody(source)));
  parseElement(dtdTranslatedSource);
} else if (source.match("<!ATTLIST")) {
  BufferSource dtdTranslatedSource(xDTD.getEntityMapper().translate(parseTagBody(source)));
  parseAttributeList(dtdTranslatedSource);
} else if (source.match("<!NOTATION")) {
  BufferSource dtdTranslatedSource(xDTD.getEntityMapper().translate(parseTagBody(source)));
  parseNotation(dtdTranslatedSource);
}
```

### Fix

Add a lambda inside `parseExternalContent`:

```cpp
void DTD_Impl::parseExternalContent(ISource &source)
{
  const auto dispatch = [&](auto parseFn) {
    BufferSource translated(xDTD.getEntityMapper().translate(parseTagBody(source)));
    (this->*parseFn)(translated);
  };

  while (source.more()) {
    if      (source.match("<!ENTITY"))   { dispatch(&DTD_Impl::parseEntity);        }
    else if (source.match("<!ELEMENT"))  { dispatch(&DTD_Impl::parseElement);       }
    else if (source.match("<!ATTLIST"))  { dispatch(&DTD_Impl::parseAttributeList); }
    else if (source.match("<!NOTATION")) { dispatch(&DTD_Impl::parseNotation);      }
    else if (source.match("<!--"))       { parseComment(source); }
    else if (source.current() == '%')    { parseParameterEntityReference(source); continue; }
    else if (source.match("<!["))        { parseConditional(source); continue; }
    else { throw SyntaxError(source.getPosition(), "Invalid DTD tag."); }
    if (source.current() != '>') { throw SyntaxError(source.getPosition(), "Missing '>' terminator."); }
    source.next();
    source.ignoreWS();
  }
}
```

**Lines eliminated:** 6 (three redundant `BufferSource` local-variable declarations).  
**Risk:** Low — lambda captures `this` and `source`, identical semantics.

---

## Task H — Extract `parseOccurrenceBounds` in `XSD_Validator_Impl_Parse.cpp`

**File:** `classes/source/implementation/xsd/XSD_Validator_Impl_Parse.cpp`  
**Functions:** `XSD_Impl::parseTopLevelElement`, `XSD_Impl::parseParticle`

### Problem

Both functions contain an **identical 4-line block** that reads `minOccurs`/`maxOccurs`:

```cpp
// in parseTopLevelElement:
const auto minStr = attrValue(elemNode, "minOccurs");
const auto maxStr = attrValue(elemNode, "maxOccurs");
decl.minOccurs = parseUint(minStr, 1u);
decl.maxOccurs = maxStr == "unbounded" ? 0u : maxStr.empty() ? 1u : parseUint(maxStr, 1u);

// in parseParticle (identical logic, different variable names):
const auto minStr = attrValue(particleNode, "minOccurs");
const auto maxStr = attrValue(particleNode, "maxOccurs");
particle.minOccurs = parseUint(minStr, 1u);
particle.maxOccurs = maxStr == "unbounded" ? 0u : maxStr.empty() ? 1u : parseUint(maxStr, 1u);
```

### Fix

Add a file-local helper in the anonymous namespace (alongside the existing `parseUint`):

```cpp
namespace {
void parseOccurrenceBounds(const Node &node, uint32_t &minOccurs, uint32_t &maxOccurs)
{
  const auto minStr = attrValueView(node, "minOccurs");
  const auto maxStr = attrValueView(node, "maxOccurs");
  minOccurs = parseUint(minStr, 1u);
  maxOccurs = maxStr == "unbounded" ? 0u : maxStr.empty() ? 1u : parseUint(maxStr, 1u);
}
} // namespace
```

Call it from both functions:

```cpp
// in parseTopLevelElement:
parseOccurrenceBounds(elemNode, decl.minOccurs, decl.maxOccurs);

// in parseParticle:
parseOccurrenceBounds(particleNode, particle.minOccurs, particle.maxOccurs);
```

**Lines eliminated:** 6.  
**Risk:** Minimal.

---

## Task I — Extract `checkNumericFacet` for the four numeric comparison facets in `validateRestrictions`

**File:** `classes/source/implementation/xsd/XSD_Validator_Impl_Validate.cpp`  
**Function:** `XSD_Impl::validateRestrictions`

### Problem

The four numeric-comparison cases in the `switch` each repeat the same 3-line
parse-then-compare-then-error structure:

```cpp
case XSD_Restriction::Facet::minInclusive: {
  const double dval = std::stod(value);
  const double dmin = std::stod(r.value);
  if (dval < dmin) { xsdError(context, "value '" + value + "' is less than minInclusive '" + r.value + "'."); }
  break;
}
case XSD_Restriction::Facet::maxInclusive: {
  const double dval = std::stod(value);
  const double dmax = std::stod(r.value);
  if (dval > dmax) { xsdError(context, "value '" + value + "' exceeds maxInclusive '" + r.value + "'."); }
  break;
}
case XSD_Restriction::Facet::minExclusive: {
  const double dval = std::stod(value);
  const double dmin = std::stod(r.value);
  if (dval <= dmin) { xsdError(context, "value '" + value + "' is not greater than minExclusive '" + r.value + "'."); }
  break;
}
case XSD_Restriction::Facet::maxExclusive: {
  const double dval = std::stod(value);
  const double dmax = std::stod(r.value);
  if (dval >= dmax) { xsdError(context, "value '" + value + "' is not less than maxExclusive '" + r.value + "'."); }
  break;
}
```

### Fix

Add a file-local helper:

```cpp
static void checkNumericFacet(const std::string &value,
  const std::string &bound,
  bool failCondition,
  const std::string &context,
  const std::string &message)
{
  const double dval = std::stod(value);
  const double dbound = std::stod(bound);
  if (failCondition ? dval < dbound
                    : false)  // replaced inline per call-site
  {
    xsdError(context, message);
  }
}
```

Actually, since each call-site has a different comparison operator, pass a predicate:

```cpp
static void checkNumericFacet(const std::string &value,
  const std::string &bound,
  const std::string &context,
  const std::string &message,
  auto cmp)
{
  const double dval  = std::stod(value);
  const double dbound = std::stod(bound);
  if (cmp(dval, dbound)) { xsdError(context, message); }
}
```

The four cases become:

```cpp
case XSD_Restriction::Facet::minInclusive:
  checkNumericFacet(value, r.value, context,
    "value '" + value + "' is less than minInclusive '" + r.value + "'.",
    std::less<double>{});
  break;
case XSD_Restriction::Facet::maxInclusive:
  checkNumericFacet(value, r.value, context,
    "value '" + value + "' exceeds maxInclusive '" + r.value + "'.",
    std::greater<double>{});
  break;
case XSD_Restriction::Facet::minExclusive:
  checkNumericFacet(value, r.value, context,
    "value '" + value + "' is not greater than minExclusive '" + r.value + "'.",
    std::less_equal<double>{});
  break;
case XSD_Restriction::Facet::maxExclusive:
  checkNumericFacet(value, r.value, context,
    "value '" + value + "' is not less than maxExclusive '" + r.value + "'.",
    std::greater_equal<double>{});
  break;
```

**Lines eliminated:** 8 (four `stod` pairs collapsed to four single calls).  
**Risk:** Minimal — predicate pattern is idiomatic C++20.

---

## Summary

| Task | File | Lines Saved | Risk |
|------|------|-------------|------|
| A    | `DTD_Validator_Impl_Validator.cpp` | 4   | Minimal |
| B    | `DTD_Validator_Impl_Validator.cpp` | 6   | Minimal |
| C    | `DTD_Validator_Impl_Validator.cpp` | 6   | Minimal |
| D    | `DTD_Validator_Impl_Parse_Content.cpp` | 14 | Low |
| E    | `Default_Parser.cpp`               | 4   | Low    |
| F    | `Default_Parser.cpp`               | 0 (quality) | Low |
| G    | `DTD_Validator_Impl_Parse_External.cpp` | 6 | Low |
| H    | `XSD_Validator_Impl_Parse.cpp`     | 6   | Minimal |
| I    | `XSD_Validator_Impl_Validate.cpp`  | 8   | Minimal |

**Total:** ~54 lines removed, no semantic changes. All tasks are independently
buildable and testable (1513 assertions in 93 test cases must continue to pass after
each task).
