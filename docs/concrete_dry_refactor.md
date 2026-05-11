# Concrete DRY Refactor Plan for XML_Lib (Post Task 1-4)

## Scope
This document is a refreshed, code-grounded DRY refactor plan based on the current repository state after completion of parser/entity/XSD/XPath helper extraction tasks.

Already completed:
- Task 1: parser helper extraction
- Task 2: entity mapper lookup/translation helper extraction
- Task 3: XSD parse traversal helper extraction
- Task 4: XPath name/value/result helper extraction

This plan covers the next set of high-value DRY opportunities that are still present.

## Analysis Basis
Reviewed current implementations in:
- classes/source/implementation/xpath/XPath_Evaluator.cpp
- classes/source/implementation/xpath/XPath_EvalHelpers.cpp
- classes/source/implementation/xsd/XSD_Validator_Impl_Validate.cpp
- classes/source/implementation/dtd/DTD_Validator_Impl_Validator.cpp
- classes/source/implementation/xml/parser/Default_Parser.cpp
- classes/source/implementation/xml/XML_Impl.cpp
- CMakeLists.txt

## Current Duplication Findings

### 1. XPath attribute extraction is duplicated
In classes/source/implementation/xpath/XPath_Evaluator.cpp:
- Axis attribute selection chooses attributes via repeated Element/Root/Self branches.
- Attribute value retrieval in evalStepResult repeats the same Element/Root/Self branching again.

Impact:
- Logic duplication and future bug risk when attribute behavior changes.

### 2. Element-like node classification repeats across modules
The same "Element or Root or Self" checks appear in multiple modules:
- classes/source/implementation/xpath/XPath_Evaluator.cpp
- classes/source/implementation/xsd/XSD_Validator_Impl_Validate.cpp
- classes/source/implementation/dtd/DTD_Validator_Impl_Validator.cpp
- classes/source/implementation/xsd/XSD_NodeHelpers.cpp

Impact:
- Inconsistent behavior risk and repetitive type-dispatch logic.

### 3. XSD validation has repeated child counting and declaration checks
In classes/source/implementation/xsd/XSD_Validator_Impl_Validate.cpp:
- Building childCounts and childOrder is repetitive infrastructure code.
- Unexpected-child checks are repeated for sequence/all and choice branches.
- Similar loops resolve particles then resolve type refs for recursive validation.

Impact:
- High cognitive load and difficult-to-change validation flow.

### 4. DTD validator has long node-type dispatch chain
In classes/source/implementation/dtd/DTD_Validator_Impl_Validator.cpp:
- checkElements uses a long if/else chain for node type behavior.
- Multiple branches recurse children similarly.

Impact:
- Harder to maintain and reason about edge cases.

### 5. Default parser repeats child-tail whitespace state transitions
In classes/source/implementation/xml/parser/Default_Parser.cpp:
- Repeated checks for "last child is Content" followed by setIsWhiteSpace(false).
- Similar content/CDATA/entity transition handling appears in multiple branches.

Impact:
- Subtle parsing behavior spread across branches instead of centralized policy.

### 6. XML_Impl repeats child lookup patterns
In classes/source/implementation/xml/XML_Impl.cpp:
- dtd() and root() both iterate prolog children with similar predicate-based return behavior.

Impact:
- Small duplication, but easy cleanup that improves readability.

## Concrete Refactor Tasks (Next Wave)

### Task 5: Consolidate XPath attribute-axis and attribute-value helpers
Add helpers:
- classes/include/implementation/xpath/XPath_AxisHelpers.hpp
- classes/source/implementation/xpath/XPath_AxisHelpers.cpp

Introduce reusable functions:
- nodeAttributes(const Node&) -> const std::vector<XMLAttribute>*
- findAttributeValue(const Node&, std::string_view) -> std::string
- isElementLikeNode(const Node&) -> bool

Refactor in classes/source/implementation/xpath/XPath_Evaluator.cpp:
- axisNodes attribute axis branch
- evalStepResult attribute value extraction branch

Acceptance criteria:
- No behavior change in XPath attribute axis tests.
- Element/Root/Self attribute branching appears in one helper only.

### Task 6: Centralize element-like node predicates for validators
Add common node classification helper:
- classes/include/implementation/common/XML_NodeKindHelpers.hpp
- classes/source/implementation/common/XML_NodeKindHelpers.cpp

Provide helpers:
- isElementLikeNode(const Node&)
- isContentNode(const Node&)
- isStructuralNode(const Node&) (optional)

Refactor call sites:
- classes/source/implementation/xsd/XSD_NodeHelpers.cpp
- classes/source/implementation/xsd/XSD_Validator_Impl_Validate.cpp
- classes/source/implementation/dtd/DTD_Validator_Impl_Validator.cpp
- classes/source/implementation/xpath/XPath_Evaluator.cpp (if Task 5 does not fully cover it)

Acceptance criteria:
- Duplicate Element/Root/Self checks removed from target files.
- Unit tests pass unchanged.

### Task 7: Extract XSD validation traversal utilities
Add helper module:
- classes/include/implementation/xsd/XSD_ValidateHelpers.hpp
- classes/source/implementation/xsd/XSD_ValidateHelpers.cpp

Move duplicated validation mechanics out of XSD_Validator_Impl_Validate.cpp:
- collectElementChildCounts(const Node&) -> child map
- findDeclaredParticle(const XSD_ComplexType&, std::string_view)
- validateUnexpectedChildren(...)
- validateParticleOccurrenceBounds(...)

Refactor sequence/all/choice branches to call shared utilities.

Acceptance criteria:
- Same validation errors/messages for existing tests.
- Reduced branching and repeated loops in validateElement().

### Task 8: Simplify DTD node dispatch with internal handlers
Refactor classes/source/implementation/dtd/DTD_Validator_Impl_Validator.cpp:
- Replace long if/else node-type chain with focused handler functions:
  - handlePrologNode
  - handleElementNode
  - handleSelfNode
  - handleContentNode
  - handleIgnorableNode

Optional (if style permits):
- table-based dispatch by node kind enum generated through helper predicates.

Acceptance criteria:
- Validation semantics unchanged.
- checkElements() shrinks to orchestration-only logic.

### Task 9: Centralize parser content-tail state updates
Refactor classes/source/implementation/xml/parser/Default_Parser.cpp:
- Add helpers in parser module:
  - markTrailingContentNonWhitespace(Node&)
  - appendEntityOrContent(Node&, const XMLValue&, IEntityMapper&)

Use these helpers in:
- parseContent
- parseElementInternal (CDATA branch and related content transitions)

Acceptance criteria:
- Content whitespace behavior remains unchanged in parse/stringify tests.
- No repeated "last child is Content -> setIsWhiteSpace(false)" blocks.

### Task 10: Add generic child lookup helper in XML_Impl
Refactor classes/source/implementation/xml/XML_Impl.cpp:
- Add internal helper:
  - findFirstChild(Node&, predicate) -> Node*

Use it in:
- dtd()
- root()

Acceptance criteria:
- No behavior change.
- Lookup code paths become single-point reusable patterns.

### Task 11: DRY CMake include path declarations
Current include directories are verbose and manually repeated in CMakeLists.txt.

Refactor options:
- Group implementation include paths into a list variable.
- Keep only classes/include public.
- Keep implementation directories private and centrally declared once.

Acceptance criteria:
- No include regressions.
- Cleaner target_include_directories configuration.

## Suggested Implementation Order
1. Task 5 (XPath attribute helpers)
2. Task 7 (XSD validation helpers)
3. Task 6 (common node-kind helpers)
4. Task 9 (Default_Parser content-tail helper)
5. Task 8 (DTD dispatch cleanup)
6. Task 10 (XML_Impl child lookup helper)
7. Task 11 (CMake cleanup)

Rationale:
- Starts with high-duplication, low-risk consolidations.
- Defers behavior-sensitive DTD/parser control-flow edits until helper foundation is in place.

## Validation Strategy
For each task:
- Build with existing CMake configuration.
- Run full unit suite:
  - build/tests/XML_Lib_Unit_Tests
- Compare error message strings in tests where behavior is expected to be identical.

## Deliverables
- Updated docs/concrete_dry_refactor.md (this file)
- New helper modules per task
- Refactored call sites with no public API changes
- Passing existing test suite
