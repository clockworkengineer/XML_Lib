#pragma once

namespace XML_Lib {

// =====================================================
// XPath 1.0 — Axis enum
// =====================================================
enum class XPathAxis : uint8_t {
  Child,
  Parent,
  Self,
  Ancestor,
  AncestorOrSelf,
  Descendant,
  DescendantOrSelf,
  Following,
  FollowingSibling,
  Preceding,
  PrecedingSibling,
  Attribute,
  Namespace
};

// =====================================================
// XPath 1.0 — Node-test kinds
// =====================================================
enum class XPathNodeTestKind : uint8_t {
  NameTest,// element name or *
  NodeType_Node,// node()
  NodeType_Text,// text()
  NodeType_Comment,// comment()
  NodeType_PI// processing-instruction()
};

struct XPathNodeTest
{
  XPathNodeTestKind kind{ XPathNodeTestKind::NameTest };
  std::string name;// element/attr name (may be "*")
};

// =====================================================
// AST node types
// =====================================================

struct XPathExpr
{
  virtual ~XPathExpr() = default;
};

using XPathExprPtr = std::unique_ptr<XPathExpr>;

// Predicate:  [ expr ]
struct XPathPredicate
{
  XPathExprPtr expr;
};

// One step along an axis:  axis::node-test[pred]*
struct XPathStep
{
  XPathAxis axis{ XPathAxis::Child };
  XPathNodeTest nodeTest;
  std::vector<XPathPredicate> predicates;
};

// Absolute or relative location path
struct XPathPathExpr final : XPathExpr
{
  bool absolute{ false };// true if starts with /
  std::vector<XPathStep> steps;
};

// expr | expr
struct XPathUnionExpr final : XPathExpr
{
  XPathExprPtr left;
  XPathExprPtr right;
};

// binary arithmetic / comparison / logic
struct XPathBinaryExpr final : XPathExpr
{
  enum class Op : uint8_t { Add, Sub, Mul, Div, Mod, Eq, Neq, Lt, Gt, LtEq, GtEq, And, Or };
  Op op;
  XPathExprPtr left;
  XPathExprPtr right;
};

// unary minus
struct XPathUnaryExpr final : XPathExpr
{
  XPathExprPtr operand;
};

// function call
struct XPathFunctionCall final : XPathExpr
{
  std::string name;
  std::vector<XPathExprPtr> args;
};

// ( expr )  — filter expression (primary with predicates)
struct XPathFilterExpr final : XPathExpr
{
  XPathExprPtr primary;
  std::vector<XPathPredicate> predicates;
};

// "string" or 'string'
struct XPathStringLiteral final : XPathExpr
{
  std::string value;
};

// 42 or 3.14
struct XPathNumberLiteral final : XPathExpr
{
  double value{ 0.0 };
};

}// namespace XML_Lib
