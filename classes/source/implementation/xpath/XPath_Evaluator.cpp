//
// XPath_Evaluator.cpp
//
// Description: XPath 1.0 evaluator — walks the Node tree executing an AST
// and returning an XPathResult.
//
// Dependencies: C++20 - Language standard features used.
//

#include "XPath_Impl.hpp"
#include "XPath_EvalHelpers.hpp"
#include "XPath_AxisHelpers.hpp"
#include "XPath_AST.hpp"
#include "XPath_Lexer.hpp"
#include "XPath_Parser.hpp"
#include "XPath.hpp"

#include <algorithm>
#include <charconv>
#include <cmath>
#include <functional>
#include <limits>
#include <sstream>
#include <string_view>
#include <unordered_set>

namespace XML_Lib {

// ========================================================================
// Forward declarations
// ========================================================================
static XPathResult evalExpr(const XPathExpr &expr,
  const Node &contextNode,
  size_t contextPosition,
  size_t contextSize,
  const Node &documentRoot,
  const std::vector<const Node *> &ancestorStack);

static std::string nodeNamespaceURI(const Node &node)
{
  if (isA<Element>(node)) return NRef<Element>(node).getNamespaceURI();
  if (isA<Root>(node)) return NRef<Root>(node).getNamespaceURI();
  if (isA<Self>(node)) return NRef<Self>(node).getNamespaceURI();
  return "";
}

// ========================================================================
// Type conversions
// ========================================================================
static double resultToNumber(const XPathResult &r);
static bool resultToBool(const XPathResult &r);

static double resultToNumber(const XPathResult &r)
{
  switch (r.type) {
  case XPathResultType::Number:
    return r.numberValue;
  case XPathResultType::Boolean:
    return r.boolValue ? 1.0 : 0.0;
  case XPathResultType::String:
    return stringToNumber(r.stringValue);
  case XPathResultType::NodeSet: {
    if (r.nodeSet.empty()) return std::numeric_limits<double>::quiet_NaN();
    if (const auto it = r.attrValues.find(r.nodeSet.front()); it != r.attrValues.end()) {
      return stringToNumber(it->second);
    }
    return stringToNumber(nodeStringValue(*r.nodeSet.front()));
  }
  }
  return 0.0;
}

static bool resultToBool(const XPathResult &r)
{
  switch (r.type) {
  case XPathResultType::Boolean:
    return r.boolValue;
  case XPathResultType::Number:
    return r.numberValue != 0.0 && !std::isnan(r.numberValue);
  case XPathResultType::String:
    return !r.stringValue.empty();
  case XPathResultType::NodeSet:
    return !r.nodeSet.empty();
  }
  return false;
}

// ========================================================================
// XPathResult factory helpers
// ========================================================================
static XPathResult makeNumber(double v)
{
  XPathResult r;
  r.type = XPathResultType::Number;
  r.numberValue = v;
  return r;
}
static XPathResult makeString(std::string s)
{
  XPathResult r;
  r.type = XPathResultType::String;
  r.stringValue = std::move(s);
  return r;
}
static XPathResult makeBool(bool b)
{
  XPathResult r;
  r.type = XPathResultType::Boolean;
  r.boolValue = b;
  return r;
}
static XPathResult makeNodeSet(std::vector<const Node *> ns = {},
  std::unordered_map<const Node *, std::string> attrs = {})
{
  XPathResult r;
  r.nodeSet = std::move(ns);
  r.attrValues = std::move(attrs);
  return r;
}

// ========================================================================
// Collect all descendants (depth-first, not including self)
// ========================================================================
static void collectDescendants(const Node &node, std::vector<const Node *> &out)
{
  std::vector<const Node *> stack;
  stack.reserve(16);
  for (const auto &child : node.getChildren()) { stack.push_back(&child); }

  while (!stack.empty()) {
    const Node *current = stack.back();
    stack.pop_back();
    out.push_back(current);
    const auto &children = current->getChildren();
    for (auto it = children.rbegin(); it != children.rend(); ++it) { stack.push_back(&*it); }
  }
}

// ========================================================================
// Node-test match
// ========================================================================
static bool matchNodeTest(const Node &node,
  const XPathNodeTest &test,
  [[maybe_unused]] XPathAxis axis,
  const std::string &attrName = "",
  bool isAttrNode = false)
{
  if (isAttrNode) {
    // Attribute axis
    if (test.kind == XPathNodeTestKind::NodeType_Node) return true;
    if (test.kind == XPathNodeTestKind::NameTest) { return test.name == "*" || test.name == attrName; }
    return false;
  }

  switch (test.kind) {
  case XPathNodeTestKind::NodeType_Node:
    return true;// any node matches node()
  case XPathNodeTestKind::NodeType_Text:
    return isA<Content>(node);
  case XPathNodeTestKind::NodeType_Comment:
    return isA<Comment>(node);
  case XPathNodeTestKind::NodeType_PI:
    return isA<PI>(node);
  case XPathNodeTestKind::NameTest: {
    if (!isElementLikeNode(node)) return false;
    return matchNodeName(node, test.name);
  }
  }
  return false;
}

// ========================================================================
// Evaluate a predicate for a specific node in a candidate set
// ========================================================================
static bool evalPredicate(const XPathPredicate &pred,
  const Node &node,
  size_t position,// 1-based
  size_t total,
  const Node &docRoot,
  const std::vector<const Node *> &ancestors)
{
  XPathResult r = evalExpr(*pred.expr, node, position, total, docRoot, ancestors);
  // If result is a number, compare to position
  if (r.type == XPathResultType::Number) { return static_cast<size_t>(r.numberValue) == position; }
  return resultToBool(r);
}

// ========================================================================
// Axis traversal: returns candidate (node, attrname) pairs
// ========================================================================
struct CandidateNode
{
  const Node *node{ nullptr };
  std::string attrName;// non-empty only for attribute axis
  bool isAttr{ false };
};


static std::vector<CandidateNode> axisNodes(XPathAxis axis,
  const Node &contextNode,
  [[maybe_unused]] const Node &documentRoot,
  const std::vector<const Node *> &ancestorStack)
{
  std::vector<CandidateNode> result;
  result.reserve(8);

  switch (axis) {
  case XPathAxis::Child:
    for (const auto &child : contextNode.getChildren()) { result.push_back({ &child, "", false }); }
    break;

  case XPathAxis::Self:
    result.push_back({ &contextNode, "", false });
    break;

  case XPathAxis::Parent:
    if (!ancestorStack.empty()) { result.push_back({ ancestorStack.back(), "", false }); }
    break;

  case XPathAxis::Ancestor:
    for (auto it = ancestorStack.rbegin(); it != ancestorStack.rend(); ++it) { result.push_back({ *it, "", false }); }
    break;

  case XPathAxis::AncestorOrSelf:
    result.push_back({ &contextNode, "", false });
    for (auto it = ancestorStack.rbegin(); it != ancestorStack.rend(); ++it) { result.push_back({ *it, "", false }); }
    break;

  case XPathAxis::Descendant: {
    std::vector<const Node *> tmp;
    tmp.reserve(16);
    collectDescendants(contextNode, tmp);
    for (const auto *n : tmp) result.push_back({ n, "", false });
    break;
  }

  case XPathAxis::DescendantOrSelf: {
    result.push_back({ &contextNode, "", false });
    std::vector<const Node *> tmp;
    tmp.reserve(16);
    collectDescendants(contextNode, tmp);
    for (const auto *n : tmp) result.push_back({ n, "", false });
    break;
  }

  case XPathAxis::Attribute:
    if (const auto *attrs = nodeAttributes(contextNode); attrs != nullptr) {
      for (const auto &attr : *attrs) {
        // Skip namespace declarations — they are on the namespace axis
        if (attr.getName().starts_with("xmlns")) continue;
        result.push_back({ &contextNode, attr.getName(), true });
      }
    }
    break;

  case XPathAxis::FollowingSibling:
  case XPathAxis::PrecedingSibling:
  case XPathAxis::Following:
  case XPathAxis::Preceding: {
    // Need parent to find siblings
    if (ancestorStack.empty()) break;
    const Node *parent = ancestorStack.back();
    const auto &siblings = parent->getChildren();
    bool found = false;
    if (axis == XPathAxis::FollowingSibling || axis == XPathAxis::Following) {
      for (const auto &sib : siblings) {
        if (&sib == &contextNode) {
          found = true;
          continue;
        }
        if (found) result.push_back({ &sib, "", false });
      }
    } else {
      for (const auto &sib : siblings) {
        if (&sib == &contextNode) break;
        result.push_back({ &sib, "", false });
      }
      std::reverse(result.begin(), result.end());
    }
    break;
  }

  case XPathAxis::Namespace:
    // Namespace axis: expose namespace declarations as pseudo-nodes
    // For simplicity, we skip this — return empty set for now
    break;
  }

  return result;
}

// ========================================================================
// Evaluate a single step, producing a new node-set (as XPathResult)
// ========================================================================
static XPathResult evalStepResult(const XPathStep &step,
  const std::vector<const Node *> &inputNodeSet,
  [[maybe_unused]] const std::unordered_map<const Node *, std::string> &inputAttrValues,
  const Node &docRoot,
  const std::vector<const Node *> &ancestorsOfContext)
{
  std::vector<const Node *> output;
  output.reserve(inputNodeSet.size());
  std::unordered_map<const Node *, std::string> outAttrValues;
  std::vector<CandidateNode> passing;
  std::vector<CandidateNode> surviving;
  passing.reserve(16);
  surviving.reserve(16);

  for (const auto *inputNode : inputNodeSet) {
    // For attribute proxies, the "real" context is still the element
    const auto candidates = axisNodes(step.axis, *inputNode, docRoot, ancestorsOfContext);

    // Filter by node-test
    passing.clear();
    passing.reserve(candidates.size());
    for (const auto &c : candidates) {
      if (matchNodeTest(*c.node, step.nodeTest, step.axis, c.attrName, c.isAttr)) { passing.push_back(c); }
    }

    // Apply predicates
    for (const auto &pred : step.predicates) {
      surviving.clear();
      surviving.reserve(passing.size());
      const size_t total = passing.size();
      size_t pos = 1;
      for (const auto &c : passing) {
        std::vector<const Node *> candAncestors = ancestorsOfContext;
        candAncestors.reserve(ancestorsOfContext.size() + 1);
        candAncestors.push_back(inputNode);
        if (evalPredicate(pred, *c.node, pos, total, docRoot, candAncestors)) { surviving.push_back(c); }
        ++pos;
      }
      passing.swap(surviving);
    }

    for (const auto &c : passing) {
      if (std::find(output.begin(), output.end(), c.node) == output.end()) {
        output.push_back(c.node);
        if (c.isAttr) {
          outAttrValues[c.node] = findAttributeValue(*c.node, c.attrName);
        }
      }
    }
  }

  return makeNodeSet(output, outAttrValues);
}

// ========================================================================
// Evaluate a PathExpr, starting from docRoot or contextNode
// ========================================================================
static XPathResult evalPathExpr(const XPathPathExpr &pathExpr,
  const Node &contextNode,
  const Node &docRoot,
  const std::vector<const Node *> &ancestorStack)
{
  std::vector<const Node *> current;
  std::unordered_map<const Node *, std::string> currentAttrs;

  if (pathExpr.absolute) {
    if (pathExpr.steps.empty()) {
      return makeNodeSet({ &docRoot });
    }

    const auto &step0 = pathExpr.steps[0];
    if (step0.axis == XPathAxis::Child) {
      if (matchNodeTest(docRoot, step0.nodeTest, step0.axis)) { current.push_back(&docRoot); }
      if (!step0.predicates.empty()) {
        std::vector<const Node *> surv;
        const size_t total = current.size();
        for (size_t i = 0; i < current.size(); ++i) {
          bool pass = true;
          for (const auto &pred : step0.predicates) {
            if (!evalPredicate(pred, *current[i], i + 1, total, docRoot, {})) {
              pass = false;
              break;
            }
          }
          if (pass) surv.push_back(current[i]);
        }
        current = std::move(surv);
      }
      for (size_t i = 1; i < pathExpr.steps.size(); ++i) {
        auto sr = evalStepResult(pathExpr.steps[i], current, currentAttrs, docRoot, {});
        current = std::move(sr.nodeSet);
        currentAttrs = std::move(sr.attrValues);
      }
    } else {
      current.push_back(&docRoot);
      for (size_t i = 0; i < pathExpr.steps.size(); ++i) {
        const auto &step = pathExpr.steps[i];
        auto sr = evalStepResult(step, current, currentAttrs, docRoot, ancestorStack);
        std::vector<const Node *> nextSet = sr.nodeSet;
        if (i == 1 && step0.axis == XPathAxis::DescendantOrSelf && step.axis == XPathAxis::Child) {
          if (matchNodeTest(docRoot, step.nodeTest, step.axis)
            && std::find(nextSet.begin(), nextSet.end(), &docRoot) == nextSet.end()) {
            nextSet.insert(nextSet.begin(), &docRoot);
          }
        }
        current = std::move(nextSet);
        currentAttrs = std::move(sr.attrValues);
      }
    }

    return makeNodeSet(current, currentAttrs);
  }

  // Relative path
  current.push_back(&contextNode);
  for (const auto &step : pathExpr.steps) {
    auto sr = evalStepResult(step, current, currentAttrs, docRoot, ancestorStack);
    current = std::move(sr.nodeSet);
    currentAttrs = std::move(sr.attrValues);
  }

  return makeNodeSet(current, currentAttrs);
}

// ========================================================================
// String functions
// ========================================================================
static std::string fnNormalizeSpace(const std::string &s)
{
  std::string result;
  bool space = false;
  for (unsigned char c : s) {
    if (std::isspace(c)) {
      space = true;
    } else {
      if (space && !result.empty()) result += ' ';
      result += static_cast<char>(c);
      space = false;
    }
  }
  return result;
}

static std::string fnTranslate(const std::string &str, const std::string &from, const std::string &to)
{
  std::string result;
  for (char c : str) {
    const auto pos = from.find(c);
    if (pos == std::string::npos) {
      result += c;
    } else if (pos < to.size()) {
      result += to[pos];
    }
    // else: character maps to nothing (delete)
  }
  return result;
}

// ========================================================================
// Built-in function dispatch
// ========================================================================
static XPathResult evalBuiltinFunction(const std::string &name,
  const std::vector<XPathExprPtr> &argExprs,
  const Node &contextNode,
  size_t contextPosition,
  size_t contextSize,
  const Node &docRoot,
  const std::vector<const Node *> &ancestorStack)
{
  // Helper: evaluate all args
  auto evalArgs = [&]() {
    std::vector<XPathResult> res;
    res.reserve(argExprs.size());
    for (const auto &a : argExprs) {
      res.push_back(evalExpr(*a, contextNode, contextPosition, contextSize, docRoot, ancestorStack));
    }
    return res;
  };
  auto nodeFromOptArg = [&]() -> const Node * {
    auto args = evalArgs();
    if (!args.empty() && args[0].type == XPathResultType::NodeSet && !args[0].nodeSet.empty()) {
      return args[0].nodeSet.front();
    }
    return &contextNode;
  };

  // --- Node-set functions ---
  if (name == "position") {
    return makeNumber(static_cast<double>(contextPosition));
  }
  if (name == "last") {
    return makeNumber(static_cast<double>(contextSize));
  }
  if (name == "count") {
    auto args = evalArgs();
    if (args.empty() || args[0].type != XPathResultType::NodeSet) {
      return makeNumber(0);
    }
    return makeNumber(static_cast<double>(args[0].nodeSet.size()));
  }
  if (name == "name" || name == "local-name") {
    const Node *n = nodeFromOptArg();
    return makeString((name == "local-name") ? std::string(nodeLocalNameView(*n)) : std::string(nodeNameView(*n)));
  }
  if (name == "namespace-uri") {
    return makeString(nodeNamespaceURI(*nodeFromOptArg()));
  }

  // --- Boolean functions ---
  if (name == "true") {
    return makeBool(true);
  }
  if (name == "false") {
    return makeBool(false);
  }
  if (name == "not") {
    auto args = evalArgs();
    return makeBool(args.empty() ? true : !resultToBool(args[0]));
  }
  if (name == "boolean") {
    auto args = evalArgs();
    return makeBool(args.empty() ? false : resultToBool(args[0]));
  }
  if (name == "lang") {
    // Simplified: always return false
    return makeBool(false);
  }

  // --- Number functions ---
  if (name == "number") {
    auto args = evalArgs();
    return makeNumber(args.empty() ? std::numeric_limits<double>::quiet_NaN() : resultToNumber(args[0]));
  }
  if (name == "sum") {
    auto args = evalArgs();
    double total = 0.0;
    if (!args.empty() && args[0].type == XPathResultType::NodeSet) {
      for (const auto *n : args[0].nodeSet) {
        const double value = resultToNumber(makeString(nodeStringValue(*n)));
        if (std::isnan(value)) {
          total = std::numeric_limits<double>::quiet_NaN();
          break;
        }
        total += value;
      }
    }
    return makeNumber(total);
  }
  if (name == "floor" || name == "ceiling") {
    auto args = evalArgs();
    if (args.empty()) { return makeNumber(std::numeric_limits<double>::quiet_NaN()); }
    const double val = resultToNumber(args[0]);
    return makeNumber(name == "floor" ? std::floor(val) : std::ceil(val));
  }
  if (name == "round") {
    auto args = evalArgs();
    if (args.empty()) { return makeNumber(std::numeric_limits<double>::quiet_NaN()); }
    return makeNumber(std::floor(resultToNumber(args[0]) + 0.5));
  }

  // --- String functions ---
  if (name == "string") {
    auto args = evalArgs();
    return makeString(args.empty() ? nodeStringValue(contextNode) : resultToString(args[0]));
  }
  if (name == "concat") {
    auto args = evalArgs();
    std::string s;
    std::string scratch;
    for (const auto &a : args) {
      s.append(resultToStringView(a, scratch));
    }
    return makeString(std::move(s));
  }
  if (name == "starts-with" || name == "contains") {
    auto args = evalArgs();
    if (args.size() < 2) { return makeBool(false); }
    std::string scratchLeft;
    std::string scratchRight;
    const std::string_view left = resultToStringView(args[0], scratchLeft);
    const std::string_view right = resultToStringView(args[1], scratchRight);
    return makeBool(name == "starts-with" ? left.starts_with(right)
                                          : left.find(right) != std::string::npos);
  }
  if (name == "string-length") {
    auto args = evalArgs();
    if (args.empty()) {
      return makeNumber(static_cast<double>(nodeStringValue(contextNode).size()));
    }
    std::string scratch;
    return makeNumber(static_cast<double>(resultToStringView(args[0], scratch).size()));
  }
  if (name == "normalize-space") {
    auto args = evalArgs();
    if (args.empty()) {
      return makeString(fnNormalizeSpace(nodeStringValue(contextNode)));
    }
    std::string scratch;
    return makeString(fnNormalizeSpace(std::string(resultToStringView(args[0], scratch))));
  }
  if (name == "translate") {
    auto args = evalArgs();
    if (args.size() < 3) {
      if (args.empty()) { return makeString(""); }
      std::string scratch;
      return makeString(std::string(resultToStringView(args[0], scratch)));
    }
    std::string scratch;
    const std::string source(resultToStringView(args[0], scratch));
    const std::string from(resultToStringView(args[1], scratch));
    const std::string to(resultToStringView(args[2], scratch));
    return makeString(fnTranslate(source, from, to));
  }
  if (name == "substring") {
    auto args = evalArgs();
    if (args.empty()) { return makeString(""); }
    std::string scratch;
    const std::string s(resultToStringView(args[0], scratch));
    const double startD = (args.size() >= 2) ? std::round(resultToNumber(args[1])) : 1.0;
    const long start = static_cast<long>(startD) - 1;
    std::string sub;
    if (args.size() >= 3) {
      const long len = static_cast<long>(std::round(resultToNumber(args[2])));
      const long begin = std::max(0L, start);
      const long end = std::min(static_cast<long>(s.size()), start + len);
      if (end > begin) sub = s.substr(static_cast<size_t>(begin), static_cast<size_t>(end - begin));
    } else {
      const long begin = std::max(0L, start);
      if (begin < static_cast<long>(s.size())) sub = s.substr(static_cast<size_t>(begin));
    }
    return makeString(std::move(sub));
  }
  if (name == "substring-before" || name == "substring-after") {
    auto args = evalArgs();
    if (args.size() < 2) { return makeString(""); }
    std::string scratchLeft;
    std::string scratchRight;
    const std::string_view haystack = resultToStringView(args[0], scratchLeft);
    const std::string_view needle = resultToStringView(args[1], scratchRight);
    const auto pos = haystack.find(needle);
    if (pos == std::string_view::npos) { return makeString(""); }
    return makeString(name == "substring-before"
      ? std::string(haystack.substr(0, pos))
      : std::string(haystack.substr(pos + needle.size())));
  }

  // --- Internal synthetic: path continuation (filter then path) ---
  if (name == "__pathcont__") {
    // args[0] = filter expression,  args[1] = path expression
    if (argExprs.size() < 2) {
      return makeNodeSet();
    }
    XPathResult filterResult =
      evalExpr(*argExprs[0], contextNode, contextPosition, contextSize, docRoot, ancestorStack);
    if (filterResult.type != XPathResultType::NodeSet) {
      return makeNodeSet();
    }
    // Apply the path expression to each node in the filter result
    // The path is a PathExpr (already has steps)
    const auto *pathExprPtr = dynamic_cast<const XPathPathExpr *>(argExprs[1].get());
    if (!pathExprPtr) { return filterResult; }
    std::vector<const Node *> combined;
    for (const auto *n : filterResult.nodeSet) {
      auto sub = evalPathExpr(*pathExprPtr, *n, docRoot, ancestorStack);
      for (const auto *r : sub.nodeSet) {
        if (std::find(combined.begin(), combined.end(), r) == combined.end()) combined.push_back(r);
      }
    }
    return makeNodeSet(std::move(combined));
  }

  throw XPath::Error(std::string("Unknown function '") + name + "'.");
}

// ========================================================================
// Main evaluator
// ========================================================================
static XPathResult evalExpr(const XPathExpr &expr,
  const Node &contextNode,
  const size_t contextPosition,
  const size_t contextSize,
  const Node &documentRoot,
  const std::vector<const Node *> &ancestorStack)
{
  // PathExpr (location path)
  if (const auto *p = dynamic_cast<const XPathPathExpr *>(&expr)) {
    return evalPathExpr(*p, contextNode, documentRoot, ancestorStack);
  }

  // Union  expr | expr
  if (const auto *u = dynamic_cast<const XPathUnionExpr *>(&expr)) {
    auto left = evalExpr(*u->left, contextNode, contextPosition, contextSize, documentRoot, ancestorStack);
    auto right = evalExpr(*u->right, contextNode, contextPosition, contextSize, documentRoot, ancestorStack);
    auto ns = left.type == XPathResultType::NodeSet ? left.nodeSet : std::vector<const Node *>{};
    if (right.type == XPathResultType::NodeSet) {
      for (const auto *n : right.nodeSet) {
        if (std::find(ns.begin(), ns.end(), n) == ns.end()) ns.push_back(n);
      }
    }
    return makeNodeSet(std::move(ns));
  }

  // Binary expression
  if (const auto *b = dynamic_cast<const XPathBinaryExpr *>(&expr)) {
    // Short-circuit for and / or
    if (b->op == XPathBinaryExpr::Op::And) {
      auto left = evalExpr(*b->left, contextNode, contextPosition, contextSize, documentRoot, ancestorStack);
      if (!resultToBool(left)) {
        return makeBool(false);
      }
      auto right = evalExpr(*b->right, contextNode, contextPosition, contextSize, documentRoot, ancestorStack);
      return makeBool(resultToBool(right));
    }
    if (b->op == XPathBinaryExpr::Op::Or) {
      auto left = evalExpr(*b->left, contextNode, contextPosition, contextSize, documentRoot, ancestorStack);
      if (resultToBool(left)) {
        return makeBool(true);
      }
      auto right = evalExpr(*b->right, contextNode, contextPosition, contextSize, documentRoot, ancestorStack);
      return makeBool(resultToBool(right));
    }

    auto left = evalExpr(*b->left, contextNode, contextPosition, contextSize, documentRoot, ancestorStack);
    auto right = evalExpr(*b->right, contextNode, contextPosition, contextSize, documentRoot, ancestorStack);

    // Equality / relational use special node-set comparison rules
    auto nodeSetContains = [&](const XPathResult &nodeSetRes, const XPathResult &other) -> bool {
      if (nodeSetRes.type != XPathResultType::NodeSet) return false;
      std::vector<std::string> otherStrings;
      if (other.type == XPathResultType::NodeSet) {
        otherStrings.reserve(other.nodeSet.size());
        for (const auto *m : other.nodeSet) {
          if (const auto it = other.attrValues.find(m); it != other.attrValues.end()) {
            otherStrings.push_back(it->second);
          } else {
            otherStrings.push_back(nodeStringValue(*m));
          }
        }
      }

      for (const auto *n : nodeSetRes.nodeSet) {
        std::string sv;
        if (const auto it = nodeSetRes.attrValues.find(n); it != nodeSetRes.attrValues.end()) {
          sv = it->second;
        } else {
          sv = nodeStringValue(*n);
        }
        if (other.type == XPathResultType::String && sv == other.stringValue) return true;
        if (other.type == XPathResultType::Number) {
          XPathResult nr;
          nr.type = XPathResultType::String;
          nr.stringValue = sv;
          if (resultToNumber(nr) == other.numberValue) return true;
        }
        if (other.type == XPathResultType::Boolean) {
          XPathResult br;
          br.type = XPathResultType::String;
          br.stringValue = sv;
          if (resultToBool(br) == other.boolValue) return true;
        }
        if (other.type == XPathResultType::NodeSet) {
          for (const auto &otherSv : otherStrings) {
            if (sv == otherSv) return true;
          }
        }
      }
      return false;
    };

    if (b->op == XPathBinaryExpr::Op::Eq || b->op == XPathBinaryExpr::Op::Neq) {
      bool eq = false;
      if (left.type == XPathResultType::NodeSet || right.type == XPathResultType::NodeSet) {
        if (left.type == XPathResultType::NodeSet)
          eq = nodeSetContains(left, right);
        else
          eq = nodeSetContains(right, left);
      } else if (left.type == XPathResultType::Boolean || right.type == XPathResultType::Boolean) {
        eq = (resultToBool(left) == resultToBool(right));
      } else if (left.type == XPathResultType::Number || right.type == XPathResultType::Number) {
        eq = (resultToNumber(left) == resultToNumber(right));
      } else {
        eq = (resultToString(left) == resultToString(right));
      }
      return makeBool((b->op == XPathBinaryExpr::Op::Eq) ? eq : !eq);
    }

    // Relational and arithmetic: coerce to numbers
    const double lv = resultToNumber(left);
    const double rv = resultToNumber(right);
    switch (b->op) {
    case XPathBinaryExpr::Op::Lt:   return makeBool(lv < rv);
    case XPathBinaryExpr::Op::Gt:   return makeBool(lv > rv);
    case XPathBinaryExpr::Op::LtEq: return makeBool(lv <= rv);
    case XPathBinaryExpr::Op::GtEq: return makeBool(lv >= rv);
    case XPathBinaryExpr::Op::Add:  return makeNumber(lv + rv);
    case XPathBinaryExpr::Op::Sub:  return makeNumber(lv - rv);
    case XPathBinaryExpr::Op::Mul:  return makeNumber(lv * rv);
    case XPathBinaryExpr::Op::Div:
      return makeNumber((rv == 0.0) ? std::numeric_limits<double>::infinity() : lv / rv);
    case XPathBinaryExpr::Op::Mod:  return makeNumber(std::fmod(lv, rv));
    default:                        return {};
    }
  }

  // Unary minus
  if (const auto *u = dynamic_cast<const XPathUnaryExpr *>(&expr)) {
    auto val = evalExpr(*u->operand, contextNode, contextPosition, contextSize, documentRoot, ancestorStack);
    return makeNumber(-resultToNumber(val));
  }

  // Function call
  if (const auto *fc = dynamic_cast<const XPathFunctionCall *>(&expr)) {
    return evalBuiltinFunction(
      fc->name, fc->args, contextNode, contextPosition, contextSize, documentRoot, ancestorStack);
  }

  // Filter expression (primary + predicates)
  if (const auto *fe = dynamic_cast<const XPathFilterExpr *>(&expr)) {
    auto primary = evalExpr(*fe->primary, contextNode, contextPosition, contextSize, documentRoot, ancestorStack);
    if (fe->predicates.empty()) return primary;
    if (primary.type != XPathResultType::NodeSet) return primary;
    for (const auto &pred : fe->predicates) {
      std::vector<const Node *> surviving;
      const size_t total = primary.nodeSet.size();
      size_t pos = 1;
      for (const auto *n : primary.nodeSet) {
        if (evalPredicate(pred, *n, pos, total, documentRoot, ancestorStack)) surviving.push_back(n);
        ++pos;
      }
      primary.nodeSet = std::move(surviving);
    }
    return primary;
  }

  // String literal
  if (const auto *sl = dynamic_cast<const XPathStringLiteral *>(&expr)) {
    return makeString(sl->value);
  }

  // Number literal
  if (const auto *nl = dynamic_cast<const XPathNumberLiteral *>(&expr)) {
    return makeNumber(nl->value);
  }

  throw XPath::Error("Internal evaluator error: unknown AST node type.");
}

// ========================================================================
// Shared evaluation entry point
// ========================================================================
/// <summary>
/// Tokenize, parse and evaluate an XPath expression against docRoot.
/// Throws XPath::Error on empty expression, syntax errors, or runtime errors.
/// </summary>
static XPathResult evalExpression(const std::string_view expression, const Node &docRoot)
{
  if (expression.empty()) { throw XPath::Error("Empty expression."); }
  const auto tokens = xpathTokenize(expression);
  const auto ast = xpathParse(tokens);
  const std::vector<const Node *> emptyAncestors;
  return evalExpr(*ast, docRoot, 1, 1, docRoot, emptyAncestors);
}

// ========================================================================
// XPath_Impl public methods (called from XPath::evaluate etc.)
// ========================================================================
XPath_Impl::XPath_Impl(const Node &root) : xmlRoot(root) {}

std::vector<const Node *> XPath_Impl::evaluate(const std::string_view expression) const
{
  try {
    XPathResult result = evalExpression(expression, xmlRoot);
    if (result.type == XPathResultType::NodeSet) return result.nodeSet;
    return {};
  } catch (const XPath::Error &) {
    throw;
  } catch (const std::exception &e) {
    throw XPath::Error(e.what());
  }
}

std::string XPath_Impl::evaluateString(const std::string_view expression) const
{
  try {
    return resultToString(evalExpression(expression, xmlRoot));
  } catch (const XPath::Error &) {
    throw;
  } catch (const std::exception &e) {
    throw XPath::Error(e.what());
  }
}

bool XPath_Impl::evaluateBool(const std::string_view expression) const
{
  try {
    return resultToBool(evalExpression(expression, xmlRoot));
  } catch (const XPath::Error &) {
    throw;
  } catch (const std::exception &e) {
    throw XPath::Error(e.what());
  }
}

double XPath_Impl::evaluateNumber(const std::string_view expression) const
{
  try {
    return resultToNumber(evalExpression(expression, xmlRoot));
  } catch (const XPath::Error &) {
    throw;
  } catch (const std::exception &e) {
    throw XPath::Error(e.what());
  }
}

}// namespace XML_Lib
