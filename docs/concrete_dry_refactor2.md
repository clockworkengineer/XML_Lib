# Concrete DRY Refactor Plan — Wave 2 (Post Tasks 1–10)

## Scope

This document is a code-grounded DRY refactor plan based on the repository state
after completion of tasks 1–10 (parser helpers, entity helpers, XSD/XPath helpers,
node-kind helpers, XSD validation helpers, DTD dispatch handlers, parser content-tail
helpers, XML_Impl child lookup).

Reviewed files:
- classes/source/implementation/xpath/XPath_Evaluator.cpp   (1 074 lines)
- classes/source/implementation/xpath/XPath_Parser.cpp      (603 lines)
- classes/source/implementation/xpath/XPath_EvalHelpers.cpp (120 lines)
- classes/source/implementation/xpath/XPath_AxisHelpers.cpp (32 lines)
- classes/source/implementation/xsd/XSD_Validator_Impl_Validate.cpp
- classes/source/implementation/xsd/XSD_ValidateHelpers.cpp
- classes/source/implementation/xsd/XSD_NodeHelpers.cpp
- classes/source/implementation/dtd/DTD_Validator_Impl_Parse.cpp
- classes/source/implementation/dtd/DTD_Validator_Impl_Parse_Content.cpp
- classes/source/implementation/entity/XML_EntityMapper.cpp
- classes/source/implementation/common/XML_Parse.cpp
- classes/source/implementation/common/XML_ParseHelpers.cpp
- classes/source/implementation/common/XML_Character.cpp

---

## Current Duplication Findings

### 1. XPath evaluate* public methods have identical boilerplate (4×)

In `XPath_Impl::evaluate`, `evaluateString`, `evaluateBool`, `evaluateNumber`
(all at the bottom of `XPath_Evaluator.cpp`) each method contains the same
4-statement block:

```cpp
if (expression.empty()) { throw XPath::Error("Empty expression."); }
const auto tokens = xpathTokenize(expression);
const auto ast    = xpathParse(tokens);
const std::vector<const Node *> emptyAncestors;
XPathResult result = evalExpr(*ast, xmlRoot, 1, 1, xmlRoot, emptyAncestors);
```

followed by the same `catch (const XPath::Error &) { throw; } catch (const
std::exception &e) { throw XPath::Error(e.what()); }` guard. Only the final
conversion (nodeSet / string / bool / double) differs.

Impact: bug fixes to error handling or the evaluation entry point must be applied
in 4 places.

---

### 2. XPathResult value factories are verbose and repeated (42× inline)

`evalBuiltinFunction` and surrounding helpers construct `XPathResult` objects
with a 2–3 line idiom each time:

```cpp
XPathResult r;
r.type = XPathResultType::Number;
r.numberValue = …;
return r;
```

There are 42 occurrences of `XPathResult r;` in `XPath_Evaluator.cpp`, split
among Number (11), String (10), Boolean (7), NodeSet (7+). Each is a separate
inline construct-and-return.

Impact: verbose and error-prone — changing the struct layout or adding a field
requires touching every factory site.

---

### 3. XPath parser repeats `DescendantOrSelf::node()` step construction (5×)

In `XPath_Parser.cpp` the same 3-line block appears 5 times (line pairs ~235,
257, 271–274, 298, 403):

```cpp
XPathStep ds;
ds.axis = XPathAxis::DescendantOrSelf;
ds.nodeTest = { XPathNodeTestKind::NodeType_Node, "" };
path->steps.push_back(std::move(ds));
```

(A sixth variant uses a differently-named variable `ds2`.)

Impact: adding a predicate or changing the implicit `//` step requires 5–6 edits.

---

### 4. XSD: `getTextContent` + `validateXxx` is a 2-line pattern repeated 5×

In `XSD_Validator_Impl_Validate.cpp` the same pattern appears five times:

```cpp
const auto text = getTextContent(someNode);
validateBuiltinType(text, typeRef, context);   // or validateRestrictions / validateSimpleValue
```

The `getTextContent` static helper lives inside the .cpp; it is not shared with
any other module but its extraction-then-validate idiom repeats across both
`validateElement` and `validate`.

Impact: any change to how text content is extracted before validation must be
applied at every call site.

---

### 5. `parseCharacterReference` duplicates `parseCharacterOrReference`'s `&#` branch

In `XML_Parse.cpp`, `parseCharacterReference` contains ~22 lines that are almost
identical to the `if (source.match("&#"))` branch inside `parseCharacterOrReference`
in `XML_ParseHelpers.cpp`. The only difference is the entry condition (caller
has already consumed `&#` prefix vs. match inside the function).

`parseCharacterReference` is called only from `validAttributeValue`
(`XML_Character.cpp`) which has already matched `"&#"` and then calls back into
it — creating a matched-prefix then reparse mismatch.

Impact: two slightly inconsistent implementations of the same character-reference
decode logic; errors fixed in one may not propagate to the other.

---

### 6. Entity mapper predicate accessors repeat `findEntityMapping` lookup (6×)

In `XML_EntityMapper.cpp` the three boolean query methods and three getter methods
each independently call `findEntityMapping` / `getEntityMapping` and then test one
field:

```cpp
bool XML_EntityMapper::isInternal(const std::string_view &entityName)
{
  if (const auto *entity = findEntityMapping(entityMappings, entityName)) { return entity->isInternal(); }
  return false;
}
// … repeated for isExternal, isNotation, getInternal, getExternal, getNotation
```

Each getter also calls `getEntityMapping` twice (once to check, once to retrieve).

Impact: 6 near-identical single-lookup wrappers; changing the lookup strategy
requires touching all 6.

---

### 7. DTD `parseAttributeType` repeats `match + ignoreWS + return` (10×)

In `DTD_Validator_Impl_Parse.cpp::parseAttributeType`, every keyword token
resolves via the same idiom:

```cpp
if (source.match("KEYWORD")) {
  attribute.type = DTD::AttributeType::keyword;
  source.ignoreWS();
  return;
}
```

This block is repeated 8 times for the keyword types (CDATA, IDREFS, IDREF, ID,
NMTOKENS, NMTOKEN, ENTITY, ENTITIES) plus one variant for NOTATION (no return).
Ten attribute-type assignments in 30 lines share this structure.

Impact: adding a new attribute keyword requires copy-pasting the 4-line block;
mismatches in ordering (e.g. IDREF before IDREFS matters) are invisible.

---

## Concrete Refactor Tasks

### Task A: Extract XPath expression evaluation entry point

**File:** `classes/source/implementation/xpath/XPath_Evaluator.cpp`

Add a private static helper:

```cpp
static XPathResult evalExpression(const Node &root,
                                  const std::string_view expression);
```

which performs: empty-check → tokenize → parse → evalExpr → return, wrapped in
the shared catch block.

Refactor `evaluate`, `evaluateString`, `evaluateBool`, `evaluateNumber` to call
it and convert only the result type.

Acceptance criteria:
- All four public methods share one tokenize/parse/evalExpr path.
- No behaviour change for any XPath test.

---

### Task B: Introduce XPathResult factory helpers

**File:** `classes/source/implementation/xpath/XPath_Evaluator.cpp` (file-scope
statics) or a new `XPath_ResultFactory.hpp` / `.cpp` pair if reuse beyond
the evaluator is desired.

Add inline or static factory functions:

```cpp
static XPathResult makeNumber(double v);
static XPathResult makeString(std::string s);
static XPathResult makeBool(bool b);
static XPathResult makeNodeSet(std::vector<const Node *> ns = {});
```

Replace all 42 inline `XPathResult r; r.type = …; return r;` constructions with
factory calls.

Acceptance criteria:
- All XPath tests pass unchanged.
- No `XPathResult r;` construct-then-assign patterns remain in `evalBuiltinFunction`
  or `evalExpr`.

---

### Task C: Introduce `makeDescendantOrSelfStep()` in XPath_Parser

**File:** `classes/source/implementation/xpath/XPath_Parser.cpp`

Add a file-scope helper:

```cpp
static XPathStep makeDescendantOrSelfStep()
{
  XPathStep s;
  s.axis = XPathAxis::DescendantOrSelf;
  s.nodeTest = { XPathNodeTestKind::NodeType_Node, "" };
  return s;
}
```

Replace all 5 (+ 1 variant) inline expansions with `path->steps.push_back(makeDescendantOrSelfStep())`.

Acceptance criteria:
- XPath path and axis tests unchanged.
- All 6 raw 3-line constructions removed.

---

### Task D: Extract `validateNodeText` helper in XSD_Validator_Impl_Validate

**File:** `classes/source/implementation/xsd/XSD_Validator_Impl_Validate.cpp`

The 5 occurrences of `const auto text = getTextContent(x); validateXxx(text, …)`
can be unified as:

```cpp
static void validateNodeText(const Node &xNode,
                              XSD_Impl &self,
                              const std::string &typeRef,
                              const std::string &context);
```

which internally calls `getTextContent`, then dispatches to
`validateBuiltinType`, `validateRestrictions`, or `validateSimpleValue` based on
type look-up.

Alternatively, move `getTextContent` to `XSD_ValidateHelpers` and add an
overload of `validateSimpleValue(const Node&, …)` that does the extraction
internally.

Acceptance criteria:
- No repeated `getTextContent` + validate pair outside of the single helper.
- All XSD tests pass.

---

### Task E: Deduplicate `parseCharacterReference` / `parseCharacterOrReference`

**File:** `classes/source/implementation/common/XML_Parse.cpp` and
`XML_ParseHelpers.cpp`

`parseCharacterReference` should be implemented in terms of
`parseCharacterOrReference` (which already handles the `&#` branch), or the
shared decoding logic should be extracted into a private helper
`decodeCharacterReference(std::string_view unparsed) -> XMLValue` that both
call.

The `validAttributeValue` call site in `XML_Character.cpp` already matches `"&#"`
before calling `parseCharacterReference`; the source position should be rewound
or the function signature adjusted so the caller does not pre-consume the prefix.

Acceptance criteria:
- Character reference decoding logic exists in exactly one place.
- All parse and stringify tests pass.

---

### Task F: Consolidate entity mapper predicate/accessor methods

**File:** `classes/source/implementation/entity/XML_EntityMapper.cpp`

Replace the 3 boolean + 3 getter methods with:

```cpp
// Generic predicate (replaces isInternal, isExternal, isNotation)
template<typename Predicate>
bool entitySatisfies(const std::string_view &entityName, Predicate &&pred) const
{
  if (const auto *e = findEntityMapping(entityMappings, entityName)) return pred(*e);
  return false;
}
```

And inline implementations:

```cpp
bool isInternal(const std::string_view &n) const
{ return entitySatisfies(n, [](const XML_EntityMapping &e){ return e.isInternal(); }); }
```

Or simply collapse each getter to use a single `getEntityMapping` call (removing
the double-lookup pattern in `getInternal` / `getExternal` / `getNotation`).

Acceptance criteria:
- `findEntityMapping` called once per lookup in each public method.
- All entity and DTD tests pass.

---

### Task G: Table-drive `parseAttributeType` in DTD parser

**File:** `classes/source/implementation/dtd/DTD_Validator_Impl_Parse.cpp`

Replace the 8 keyword `if/match/assign/ignoreWS/return` blocks with a lookup
table:

```cpp
static constexpr std::pair<std::string_view, DTD::AttributeType> kAttrTypeTable[] = {
  { "CDATA",     DTD::AttributeType::cdata    },
  { "IDREFS",    DTD::AttributeType::idrefs   },
  { "IDREF",     DTD::AttributeType::idref    },
  { "ID",        DTD::AttributeType::id       },
  { "NMTOKENS",  DTD::AttributeType::nmtokens },
  { "NMTOKEN",   DTD::AttributeType::nmtoken  },
  { "ENTITIES",  DTD::AttributeType::entities },
  { "ENTITY",    DTD::AttributeType::entity   },
};
```

Iterate the table; on match, assign and `ignoreWS()`/return. Handle NOTATION as
a special case (no return) after the table loop.

Acceptance criteria:
- Ordering invariant preserved (IDREFS before IDREF, NMTOKENS before NMTOKEN,
  ENTITIES before ENTITY).
- All DTD attribute-type parse tests pass.

---

## Suggested Implementation Order

1. **Task C** — `makeDescendantOrSelfStep` (smallest, highest ratio, zero risk)
2. **Task B** — XPathResult factories (high repetition, contained in one file)
3. **Task A** — XPath evaluation entry point (unifies public API boilerplate)
4. **Task D** — `validateNodeText` XSD helper (medium scope, isolated)
5. **Task G** — DTD attribute-type table (mechanical, well-tested)
6. **Task E** — `parseCharacterReference` deduplication (requires care with caller state)
7. **Task F** — Entity mapper accessor consolidation (straightforward after reading)

## Rationale

Tasks C, B, A are pure XPath internal cleanups — no public API change, all tests
cover the paths. Tasks D and G are inside already-isolated modules. Task E is
listed last among core changes because the caller in `XML_Character.cpp` already
consumed `&#`, so the refactor involves either adjusting the call site or changing
the function contract. Task F is last because the entity mapper has many call
sites across DTD/entity modules; a template approach must be verified not to
regress any entity test.
