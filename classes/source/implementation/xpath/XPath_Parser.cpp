//
// XPath_Parser.cpp
//
// Description: Recursive-descent XPath 1.0 parser.
// Converts the token stream produced by XPath_Lexer into an AST.
//
// Dependencies: C++20 - Language standard features used.
//

#include "XPath_Parser.hpp"

namespace XML_Lib {

// ======================================================================
// Parser state
// ======================================================================
struct Parser
{
  const std::vector<XPathToken> &tokens;
  size_t pos{ 0 };

  [[nodiscard]] const XPathToken &cur() const { return tokens[pos]; }
  [[nodiscard]] const XPathToken &peek(size_t ahead = 1) const
  {
    size_t idx = pos + ahead;
    if (idx >= tokens.size()) { return tokens.back(); }// End token
    return tokens[idx];
  }

  void consume()
  {
    if (cur().type != XPathTokenType::End) { ++pos; }
  }

  bool accept(XPathTokenType t)
  {
    if (cur().type == t) {
      ++pos;
      return true;
    }
    return false;
  }

  XPathToken expect(XPathTokenType t)
  {
    if (cur().type != t) {
      throw std::runtime_error(std::string("XPath Error: Unexpected token '") + cur().value + "'.");
    }
    XPathToken tok = cur();
    ++pos;
    return tok;
  }
};

// ======================================================================
// Forward declarations (all helpers are mutually recursive)
// ======================================================================
static XPathExprPtr parseExpr(Parser &p);
static XPathExprPtr parseOrExpr(Parser &p);
static XPathExprPtr parseAndExpr(Parser &p);
static XPathExprPtr parseEqualityExpr(Parser &p);
static XPathExprPtr parseRelationalExpr(Parser &p);
static XPathExprPtr parseAdditiveExpr(Parser &p);
static XPathExprPtr parseMultiplicativeExpr(Parser &p);
static XPathExprPtr parseUnaryExpr(Parser &p);
static XPathExprPtr parseUnionExpr(Parser &p);
static XPathExprPtr parsePathExpr(Parser &p);
static XPathExprPtr parseFilterExpr(Parser &p);
static XPathExprPtr parsePrimaryExpr(Parser &p);
static std::unique_ptr<XPathPathExpr> parseLocationPath(Parser &p);
static std::unique_ptr<XPathPathExpr> parseRelativeLocationPath(Parser &p, bool absolute);
static XPathStep parseStep(Parser &p);
static XPathPredicate parsePredicate(Parser &p);
static XPathNodeTest parseNodeTest(Parser &p);
static XPathAxis parseAxisSpecifier(Parser &p);

// ======================================================================
// Helpers
// ======================================================================


static XPathAxis nameToAxis(const std::string &s)
{
  if (s == "child") return XPathAxis::Child;
  if (s == "parent") return XPathAxis::Parent;
  if (s == "self") return XPathAxis::Self;
  if (s == "ancestor") return XPathAxis::Ancestor;
  if (s == "ancestor-or-self") return XPathAxis::AncestorOrSelf;
  if (s == "descendant") return XPathAxis::Descendant;
  if (s == "descendant-or-self") return XPathAxis::DescendantOrSelf;
  if (s == "following") return XPathAxis::Following;
  if (s == "following-sibling") return XPathAxis::FollowingSibling;
  if (s == "preceding") return XPathAxis::Preceding;
  if (s == "preceding-sibling") return XPathAxis::PrecedingSibling;
  if (s == "attribute") return XPathAxis::Attribute;
  if (s == "namespace") return XPathAxis::Namespace;
  throw std::runtime_error(std::string("XPath Error: Unknown axis '") + s + "'.");
}

// Is the current token potentially the start of a LocationPath?
static bool isLocationPathStart(const Parser &p)
{
  const XPathTokenType t = p.cur().type;
  if (t == XPathTokenType::Slash || t == XPathTokenType::DoubleSlash) return true;
  if (t == XPathTokenType::Dot || t == XPathTokenType::DotDot) return true;
  if (t == XPathTokenType::At) return true;
  if (t == XPathTokenType::Star) return true;
  if (t == XPathTokenType::Name) {
    // axis::  → location path
    if (p.peek().type == XPathTokenType::ColonColon) return true;
    // name( is a function call → not a location path start
    if (p.peek().type == XPathTokenType::LeftParen) return false;
    return true;
  }
  return false;
}

// ======================================================================
// Node test
// ======================================================================
static XPathNodeTest parseNodeTest(Parser &p)
{
  XPathNodeTest nt;
  if (p.cur().type == XPathTokenType::Star) {
    nt.kind = XPathNodeTestKind::NameTest;
    nt.name = "*";
    p.consume();
    return nt;
  }
  if (p.cur().type == XPathTokenType::Name) {
    const std::string nm = p.cur().value;

    // node-type functions: node(), text(), comment(), processing-instruction()
    if ((nm == "node" || nm == "text" || nm == "comment" || nm == "processing-instruction")
        && p.peek().type == XPathTokenType::LeftParen) {
      p.consume();// name
      p.consume();// (
      p.expect(XPathTokenType::RightParen);
      if (nm == "node")
        nt.kind = XPathNodeTestKind::NodeType_Node;
      else if (nm == "text")
        nt.kind = XPathNodeTestKind::NodeType_Text;
      else if (nm == "comment")
        nt.kind = XPathNodeTestKind::NodeType_Comment;
      else
        nt.kind = XPathNodeTestKind::NodeType_PI;
      return nt;
    }

    nt.kind = XPathNodeTestKind::NameTest;
    nt.name = nm;
    p.consume();
    return nt;
  }
  throw std::runtime_error(std::string("XPath Error: Expected node test, got '") + p.cur().value + "'.");
}

// ======================================================================
// Predicate
// ======================================================================
static XPathPredicate parsePredicate(Parser &p)
{
  p.expect(XPathTokenType::LeftBracket);
  XPathPredicate pred;
  pred.expr = parseExpr(p);
  p.expect(XPathTokenType::RightBracket);
  return pred;
}

// ======================================================================
// Axis specifier
// ======================================================================
static XPathAxis parseAxisSpecifier(Parser &p)
{
  // @  →  attribute::
  if (p.cur().type == XPathTokenType::At) {
    p.consume();
    return XPathAxis::Attribute;
  }
  // Name::
  if (p.cur().type == XPathTokenType::Name && p.peek().type == XPathTokenType::ColonColon) {
    const std::string axisName = p.cur().value;
    p.consume();// name
    p.consume();// ::
    return nameToAxis(axisName);
  }
  // default child::
  return XPathAxis::Child;
}

// ======================================================================
// Step
// ======================================================================
static XPathStep parseStep(Parser &p)
{
  XPathStep step;

  // Abbreviated steps
  if (p.cur().type == XPathTokenType::DotDot) {
    p.consume();
    // .. → parent::node()
    step.axis = XPathAxis::Parent;
    step.nodeTest = { XPathNodeTestKind::NodeType_Node, "" };
    return step;
  }
  if (p.cur().type == XPathTokenType::Dot) {
    p.consume();
    // . → self::node()
    step.axis = XPathAxis::Self;
    step.nodeTest = { XPathNodeTestKind::NodeType_Node, "" };
    return step;
  }

  step.axis = parseAxisSpecifier(p);
  step.nodeTest = parseNodeTest(p);

  while (p.cur().type == XPathTokenType::LeftBracket) { step.predicates.push_back(parsePredicate(p)); }

  return step;
}

// ======================================================================
// Relative location path  (step (/ or // step)*)
// ======================================================================
static std::unique_ptr<XPathPathExpr> parseRelativeLocationPath(Parser &p, const bool absolute)
{
  auto path = std::make_unique<XPathPathExpr>();
  path->absolute = absolute;
  path->steps.push_back(parseStep(p));

  while (p.cur().type == XPathTokenType::Slash || p.cur().type == XPathTokenType::DoubleSlash) {
    if (p.cur().type == XPathTokenType::DoubleSlash) {
      p.consume();
      // // → insert  descendant-or-self::node()
      XPathStep ds;
      ds.axis = XPathAxis::DescendantOrSelf;
      ds.nodeTest = { XPathNodeTestKind::NodeType_Node, "" };
      path->steps.push_back(std::move(ds));
    } else {
      p.consume();// /
    }
    path->steps.push_back(parseStep(p));
  }
  return path;
}

// ======================================================================
// Location path  (absolute or relative)
// ======================================================================
static std::unique_ptr<XPathPathExpr> parseLocationPath(Parser &p)
{
  if (p.cur().type == XPathTokenType::DoubleSlash) {
    p.consume();
    // //rel  →  /descendant-or-self::node()/rel
    auto path = std::make_unique<XPathPathExpr>();
    path->absolute = true;
    XPathStep ds;
    ds.axis = XPathAxis::DescendantOrSelf;
    ds.nodeTest = { XPathNodeTestKind::NodeType_Node, "" };
    path->steps.push_back(std::move(ds));
    // now parse the rest as relative steps (same path object)
    if (p.cur().type != XPathTokenType::End && p.cur().type != XPathTokenType::RightBracket
        && p.cur().type != XPathTokenType::RightParen && p.cur().type != XPathTokenType::Pipe) {
      // append more steps
      while (true) {
        path->steps.push_back(parseStep(p));
        if (p.cur().type == XPathTokenType::Slash) {
          p.consume();
        } else if (p.cur().type == XPathTokenType::DoubleSlash) {
          p.consume();
          XPathStep ds2;
          ds2.axis = XPathAxis::DescendantOrSelf;
          ds2.nodeTest = { XPathNodeTestKind::NodeType_Node, "" };
          path->steps.push_back(std::move(ds2));
        } else {
          break;
        }
      }
    }
    return path;
  }

  if (p.cur().type == XPathTokenType::Slash) {
    p.consume();
    auto path = std::make_unique<XPathPathExpr>();
    path->absolute = true;
    // Could be just "/" (select document root) or "/rel"
    if (p.cur().type == XPathTokenType::End || p.cur().type == XPathTokenType::RightBracket
        || p.cur().type == XPathTokenType::RightParen || p.cur().type == XPathTokenType::Pipe) {
      // bare "/" — root only; return path with no steps (root = context of evaluation)
      return path;
    }
    // absolute path with steps
    path->steps.push_back(parseStep(p));
    while (p.cur().type == XPathTokenType::Slash || p.cur().type == XPathTokenType::DoubleSlash) {
      if (p.cur().type == XPathTokenType::DoubleSlash) {
        p.consume();
        XPathStep ds;
        ds.axis = XPathAxis::DescendantOrSelf;
        ds.nodeTest = { XPathNodeTestKind::NodeType_Node, "" };
        path->steps.push_back(std::move(ds));
      } else {
        p.consume();
      }
      path->steps.push_back(parseStep(p));
    }
    return path;
  }

  // relative path
  return parseRelativeLocationPath(p, false);
}

// ======================================================================
// Primary expression
// ======================================================================
static XPathExprPtr parsePrimaryExpr(Parser &p)
{
  // ( expr )
  if (p.cur().type == XPathTokenType::LeftParen) {
    p.consume();
    auto inner = parseExpr(p);
    p.expect(XPathTokenType::RightParen);
    auto fe = std::make_unique<XPathFilterExpr>();
    fe->primary = std::move(inner);
    return fe;
  }

  // String literal
  if (p.cur().type == XPathTokenType::StringLiteral) {
    auto lit = std::make_unique<XPathStringLiteral>();
    lit->value = p.cur().value;
    p.consume();
    return lit;
  }

  // Number literal
  if (p.cur().type == XPathTokenType::NumberLiteral) {
    auto lit = std::make_unique<XPathNumberLiteral>();
    lit->value = std::stod(p.cur().value);
    p.consume();
    return lit;
  }

  // Function call: Name(
  if (p.cur().type == XPathTokenType::Name && p.peek().type == XPathTokenType::LeftParen) {
    // But node-type names that form node-tests are handled in parseStep — not here unless we're in
    // an expression context. We permit them as 0-arg function calls returning no nodes.
    auto call = std::make_unique<XPathFunctionCall>();
    call->name = p.cur().value;
    p.consume();// name
    p.consume();// (
    if (p.cur().type != XPathTokenType::RightParen) {
      call->args.push_back(parseExpr(p));
      while (p.cur().type == XPathTokenType::Comma) {
        p.consume();
        call->args.push_back(parseExpr(p));
      }
    }
    p.expect(XPathTokenType::RightParen);
    return call;
  }

  throw std::runtime_error(std::string("XPath Error: Expected primary expression, got '") + p.cur().value + "'.");
}

// ======================================================================
// Filter expression  (primary [pred]*)
// ======================================================================
static XPathExprPtr parseFilterExpr(Parser &p)
{
  auto primary = parsePrimaryExpr(p);
  if (p.cur().type != XPathTokenType::LeftBracket) { return primary; }
  auto fe = std::make_unique<XPathFilterExpr>();
  fe->primary = std::move(primary);
  while (p.cur().type == XPathTokenType::LeftBracket) { fe->predicates.push_back(parsePredicate(p)); }
  return fe;
}

// ======================================================================
// Path expression
// ======================================================================
static XPathExprPtr parsePathExpr(Parser &p)
{
  if (isLocationPathStart(p)) { return parseLocationPath(p); }

  // filter-expr ('/' RelativeLocationPath)?
  auto fe = parseFilterExpr(p);

  if (p.cur().type == XPathTokenType::Slash || p.cur().type == XPathTokenType::DoubleSlash) {
    // Combine: the filter result feeds a path − for simplicity produce a PathExpr
    // wrapping the filter as a step with predicates. This handles (//book)[1]/title patterns.
    // We represent this as a FilterExpr whose primary is the FilterExpr and whose continuation
    // is stored separately. Since our evaluator handles XPathFilterExpr with a path continuation,
    // we just return the filter expression here and let the caller handle the /step part.
    // For now, wrap into another filter expression combining steps.
    bool doubleSlash = (p.cur().type == XPathTokenType::DoubleSlash);
    p.consume();

    auto path = std::make_unique<XPathPathExpr>();
    path->absolute = false;
    if (doubleSlash) {
      XPathStep ds;
      ds.axis = XPathAxis::DescendantOrSelf;
      ds.nodeTest = { XPathNodeTestKind::NodeType_Node, "" };
      path->steps.push_back(std::move(ds));
    }
    path->steps.push_back(parseStep(p));

    // Represent as a binary "path continuation" — we'll evaluate left as node-set then walk right
    auto binary = std::make_unique<XPathBinaryExpr>();
    binary->op = XPathBinaryExpr::Op::Add;// placeholder — evaluator recognises FilterExpr+PathExpr
    // Instead represent this whole thing as a filter-then-path using a dedicated encoding:
    // Wrap in FilterExpr with the filter as primary and path kept separate via a union trick.
    // Simpler: store as a "path continuation" marker. Since our AST doesn't have a dedicated
    // StepChain node, encode it as FilterExpr::primary = original filter, and store the
    // continuation path as a second argument of a synthetic function call "__pathcont__".
    auto cont = std::make_unique<XPathFunctionCall>();
    cont->name = "__pathcont__";
    cont->args.push_back(std::move(fe));
    cont->args.push_back(std::move(path));
    return cont;
  }

  return fe;
}

// ======================================================================
// Union expression
// ======================================================================
static XPathExprPtr parseUnionExpr(Parser &p)
{
  auto left = parsePathExpr(p);
  while (p.cur().type == XPathTokenType::Pipe) {
    p.consume();
    auto right = parsePathExpr(p);
    auto u = std::make_unique<XPathUnionExpr>();
    u->left = std::move(left);
    u->right = std::move(right);
    left = std::move(u);
  }
  return left;
}

// ======================================================================
// Unary expression
// ======================================================================
static XPathExprPtr parseUnaryExpr(Parser &p)
{
  if (p.cur().type == XPathTokenType::Minus) {
    p.consume();
    auto u = std::make_unique<XPathUnaryExpr>();
    u->operand = parseUnaryExpr(p);
    return u;
  }
  return parseUnionExpr(p);
}

// ======================================================================
// Multiplicative
// ======================================================================
static XPathExprPtr parseMultiplicativeExpr(Parser &p)
{
  auto left = parseUnaryExpr(p);
  while (true) {
    XPathBinaryExpr::Op op;
    if (p.cur().type == XPathTokenType::Star) {
      op = XPathBinaryExpr::Op::Mul;
    } else if (p.cur().type == XPathTokenType::Name && (p.cur().value == "div" || p.cur().value == "mod")) {
      op = (p.cur().value == "div") ? XPathBinaryExpr::Op::Div : XPathBinaryExpr::Op::Mod;
    } else {
      break;
    }
    p.consume();
    auto b = std::make_unique<XPathBinaryExpr>();
    b->op = op;
    b->left = std::move(left);
    b->right = parseUnaryExpr(p);
    left = std::move(b);
  }
  return left;
}

// ======================================================================
// Additive
// ======================================================================
static XPathExprPtr parseAdditiveExpr(Parser &p)
{
  auto left = parseMultiplicativeExpr(p);
  while (p.cur().type == XPathTokenType::Plus || p.cur().type == XPathTokenType::Minus) {
    XPathBinaryExpr::Op op =
      (p.cur().type == XPathTokenType::Plus) ? XPathBinaryExpr::Op::Add : XPathBinaryExpr::Op::Sub;
    p.consume();
    auto b = std::make_unique<XPathBinaryExpr>();
    b->op = op;
    b->left = std::move(left);
    b->right = parseMultiplicativeExpr(p);
    left = std::move(b);
  }
  return left;
}

// ======================================================================
// Relational
// ======================================================================
static XPathExprPtr parseRelationalExpr(Parser &p)
{
  auto left = parseAdditiveExpr(p);
  while (true) {
    XPathBinaryExpr::Op op;
    if (p.cur().type == XPathTokenType::Lt)
      op = XPathBinaryExpr::Op::Lt;
    else if (p.cur().type == XPathTokenType::Gt)
      op = XPathBinaryExpr::Op::Gt;
    else if (p.cur().type == XPathTokenType::LtEq)
      op = XPathBinaryExpr::Op::LtEq;
    else if (p.cur().type == XPathTokenType::GtEq)
      op = XPathBinaryExpr::Op::GtEq;
    else
      break;
    p.consume();
    auto b = std::make_unique<XPathBinaryExpr>();
    b->op = op;
    b->left = std::move(left);
    b->right = parseAdditiveExpr(p);
    left = std::move(b);
  }
  return left;
}

// ======================================================================
// Equality
// ======================================================================
static XPathExprPtr parseEqualityExpr(Parser &p)
{
  auto left = parseRelationalExpr(p);
  while (p.cur().type == XPathTokenType::Eq || p.cur().type == XPathTokenType::Neq) {
    XPathBinaryExpr::Op op = (p.cur().type == XPathTokenType::Eq) ? XPathBinaryExpr::Op::Eq : XPathBinaryExpr::Op::Neq;
    p.consume();
    auto b = std::make_unique<XPathBinaryExpr>();
    b->op = op;
    b->left = std::move(left);
    b->right = parseRelationalExpr(p);
    left = std::move(b);
  }
  return left;
}

// ======================================================================
// And expression
// ======================================================================
static XPathExprPtr parseAndExpr(Parser &p)
{
  auto left = parseEqualityExpr(p);
  while (p.cur().type == XPathTokenType::Name && p.cur().value == "and") {
    p.consume();
    auto b = std::make_unique<XPathBinaryExpr>();
    b->op = XPathBinaryExpr::Op::And;
    b->left = std::move(left);
    b->right = parseEqualityExpr(p);
    left = std::move(b);
  }
  return left;
}

// ======================================================================
// Or expression
// ======================================================================
static XPathExprPtr parseOrExpr(Parser &p)
{
  auto left = parseAndExpr(p);
  while (p.cur().type == XPathTokenType::Name && p.cur().value == "or") {
    p.consume();
    auto b = std::make_unique<XPathBinaryExpr>();
    b->op = XPathBinaryExpr::Op::Or;
    b->left = std::move(left);
    b->right = parseAndExpr(p);
    left = std::move(b);
  }
  return left;
}

// ======================================================================
// Top-level
// ======================================================================
static XPathExprPtr parseExpr(Parser &p) { return parseOrExpr(p); }

// ======================================================================
// Public entry point
// ======================================================================
XPathExprPtr xpathParse(const std::vector<XPathToken> &tokens)
{
  if (tokens.empty()) { throw std::runtime_error("XPath Error: Empty expression."); }
  Parser p{ tokens, 0 };
  auto expr = parseExpr(p);
  if (p.cur().type != XPathTokenType::End) {
    throw std::runtime_error(
      std::string("XPath Error: Unexpected token '") + p.cur().value + "' at end of expression.");
  }
  return expr;
}

}// namespace XML_Lib
