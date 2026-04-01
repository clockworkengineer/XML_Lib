//
// XPath_Evaluator.cpp
//
// Description: XPath 1.0 evaluator — walks the Node tree executing an AST
// and returning an XPathResult.
//
// Dependencies: C++20 - Language standard features used.
//

#include "XPath_Impl.hpp"
#include "XPath_AST.hpp"
#include "XPath_Lexer.hpp"
#include "XPath_Parser.hpp"
#include "XPath.hpp"

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

// ========================================================================
// String-value of a node
// ========================================================================
static std::string nodeStringValue(const Node &node)
{
  // Text nodes
  if (isA<Content>(node)) { return NRef<Content>(node).getContents(); }
  // Attribute — not a node in the tree, handled separately
  // Element / Root / Self: concatenate all descendant text
  std::string result;
  std::function<void(const Node &)> walk = [&](const Node &n) {
    if (isA<Content>(n)) { result += NRef<Content>(n).getContents(); }
    for (const auto &child : n.getChildren()) { walk(child); }
  };
  walk(node);
  return result;
}

// ========================================================================
// Get the element name from a node (empty if not an element type)
// ========================================================================
static std::string nodeName(const Node &node)
{
  if (isA<Element>(node)) return NRef<Element>(node).name();
  if (isA<Root>(node)) return NRef<Root>(node).name();
  if (isA<Self>(node)) return NRef<Self>(node).name();
  if (isA<Comment>(node)) return "";
  if (isA<Content>(node)) return "";
  if (isA<PI>(node)) return NRef<PI>(node).name();
  return "";
}

static std::string nodeLocalName(const Node &node)
{
  const std::string nm = nodeName(node);
  const auto pos = nm.find(':');
  return (pos != std::string::npos) ? nm.substr(pos + 1) : nm;
}


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
static std::string resultToString(const XPathResult &r);
static double resultToNumber(const XPathResult &r);
static bool resultToBool(const XPathResult &r);

static std::string resultToString(const XPathResult &r)
{
  switch (r.type) {
  case XPathResultType::String:
    return r.stringValue;
  case XPathResultType::Number: {
    if (std::isnan(r.numberValue)) return "NaN";
    if (std::isinf(r.numberValue)) return (r.numberValue > 0) ? "Infinity" : "-Infinity";
    // Remove trailing zeros from decimal representation
    std::ostringstream os;
    os << r.numberValue;
    return os.str();
  }
  case XPathResultType::Boolean:
    return r.boolValue ? "true" : "false";
  case XPathResultType::NodeSet:
    if (r.nodeSet.empty()) return "";
    // Check if the first node is an attribute proxy
    if (const auto it = r.attrValues.find(r.nodeSet.front()); it != r.attrValues.end()) return it->second;
    return nodeStringValue(*r.nodeSet.front());
  }
  return "";
}

static double resultToNumber(const XPathResult &r)
{
  switch (r.type) {
  case XPathResultType::Number:
    return r.numberValue;
  case XPathResultType::Boolean:
    return r.boolValue ? 1.0 : 0.0;
  case XPathResultType::String: {
    const std::string s = r.stringValue;
    if (s.empty()) return std::numeric_limits<double>::quiet_NaN();
    try {
      size_t idx = 0;
      double v = std::stod(s, &idx);
      // consume trailing whitespace
      while (idx < s.size() && std::isspace(static_cast<unsigned char>(s[idx]))) ++idx;
      if (idx != s.size()) return std::numeric_limits<double>::quiet_NaN();
      return v;
    } catch (...) {
      return std::numeric_limits<double>::quiet_NaN();
    }
  }
  case XPathResultType::NodeSet: {
    if (r.nodeSet.empty()) return std::numeric_limits<double>::quiet_NaN();
    // Use attribute value if present
    std::string sv;
    if (const auto it = r.attrValues.find(r.nodeSet.front()); it != r.attrValues.end()) {
      sv = it->second;
    } else {
      sv = nodeStringValue(*r.nodeSet.front());
    }
    XPathResult s;
    s.type = XPathResultType::String;
    s.stringValue = sv;
    return resultToNumber(s);
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
// Collect all descendants (depth-first, not including self)
// ========================================================================
static void collectDescendants(const Node &node, std::vector<const Node *> &out)
{
  for (const auto &child : node.getChildren()) {
    out.push_back(&child);
    collectDescendants(child, out);
  }
}

// ========================================================================
// Is this node an element-type node (matches element/Root/Self)?
// ========================================================================
static bool isElement(const Node &n) { return isA<Element>(n) || isA<Root>(n) || isA<Self>(n); }

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
    if (!isElement(node)) return false;
    if (test.name == "*") return true;
    // Compare local-name or full name
    const std::string name = nodeName(node);
    const std::string local = nodeLocalName(node);
    return name == test.name || local == test.name;
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
    collectDescendants(contextNode, tmp);
    for (const auto *n : tmp) result.push_back({ n, "", false });
    break;
  }

  case XPathAxis::DescendantOrSelf: {
    result.push_back({ &contextNode, "", false });
    std::vector<const Node *> tmp;
    collectDescendants(contextNode, tmp);
    for (const auto *n : tmp) result.push_back({ n, "", false });
    break;
  }

  case XPathAxis::Attribute:
    if (isElement(contextNode)) {
      const auto &attrs = [&]() -> const std::vector<XMLAttribute> & {
        if (isA<Element>(contextNode)) return NRef<Element>(contextNode).getAttributes();
        if (isA<Root>(contextNode)) return NRef<Root>(contextNode).getAttributes();
        return NRef<Self>(contextNode).getAttributes();
      }();
      for (const auto &attr : attrs) {
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
  std::unordered_map<const Node *, std::string> outAttrValues;

  for (const auto *inputNode : inputNodeSet) {
    // For attribute proxies, the "real" context is still the element
    const auto candidates = axisNodes(step.axis, *inputNode, docRoot, ancestorsOfContext);

    // Filter by node-test
    std::vector<CandidateNode> passing;
    for (const auto &c : candidates) {
      if (matchNodeTest(*c.node, step.nodeTest, step.axis, c.attrName, c.isAttr)) { passing.push_back(c); }
    }

    // Apply predicates
    for (const auto &pred : step.predicates) {
      std::vector<CandidateNode> surviving;
      const size_t total = passing.size();
      size_t pos = 1;
      for (const auto &c : passing) {
        std::vector<const Node *> candAncestors = ancestorsOfContext;
        candAncestors.push_back(inputNode);
        if (evalPredicate(pred, *c.node, pos, total, docRoot, candAncestors)) { surviving.push_back(c); }
        ++pos;
      }
      passing = std::move(surviving);
    }

    for (const auto &c : passing) {
      if (!std::ranges::contains(output, c.node)) {
        output.push_back(c.node);
        if (c.isAttr) {
          // Retrieve attribute value from the element
          const auto getAttrVal = [&]() -> std::string {
            if (isA<Element>(*c.node)) {
              for (const auto &a : NRef<Element>(*c.node).getAttributes()) {
                if (a.getName() == c.attrName) return a.getParsed();
              }
            } else if (isA<Root>(*c.node)) {
              for (const auto &a : NRef<Root>(*c.node).getAttributes()) {
                if (a.getName() == c.attrName) return a.getParsed();
              }
            } else if (isA<Self>(*c.node)) {
              for (const auto &a : NRef<Self>(*c.node).getAttributes()) {
                if (a.getName() == c.attrName) return a.getParsed();
              }
            }
            return "";
          };
          outAttrValues[c.node] = getAttrVal();
        }
      }
    }
  }

  XPathResult r;
  r.type = XPathResultType::NodeSet;
  r.nodeSet = output;
  r.attrValues = outAttrValues;
  return r;
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
      XPathResult r;
      r.type = XPathResultType::NodeSet;
      r.nodeSet = { &docRoot };
      return r;
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
          if (matchNodeTest(docRoot, step.nodeTest, step.axis) && !std::ranges::contains(nextSet, &docRoot)) {
            nextSet.insert(nextSet.begin(), &docRoot);
          }
        }
        current = std::move(nextSet);
        currentAttrs = std::move(sr.attrValues);
      }
    }

    XPathResult r;
    r.type = XPathResultType::NodeSet;
    r.nodeSet = current;
    r.attrValues = currentAttrs;
    return r;
  }

  // Relative path
  current.push_back(&contextNode);
  for (const auto &step : pathExpr.steps) {
    auto sr = evalStepResult(step, current, currentAttrs, docRoot, ancestorStack);
    current = std::move(sr.nodeSet);
    currentAttrs = std::move(sr.attrValues);
  }

  XPathResult r;
  r.type = XPathResultType::NodeSet;
  r.nodeSet = current;
  r.attrValues = currentAttrs;
  return r;
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
    for (const auto &a : argExprs) {
      res.push_back(evalExpr(*a, contextNode, contextPosition, contextSize, docRoot, ancestorStack));
    }
    return res;
  };

  // --- Node-set functions ---
  if (name == "position") {
    XPathResult r;
    r.type = XPathResultType::Number;
    r.numberValue = static_cast<double>(contextPosition);
    return r;
  }
  if (name == "last") {
    XPathResult r;
    r.type = XPathResultType::Number;
    r.numberValue = static_cast<double>(contextSize);
    return r;
  }
  if (name == "count") {
    auto args = evalArgs();
    if (args.empty() || args[0].type != XPathResultType::NodeSet) {
      XPathResult r;
      r.type = XPathResultType::Number;
      r.numberValue = 0;
      return r;
    }
    XPathResult r;
    r.type = XPathResultType::Number;
    r.numberValue = static_cast<double>(args[0].nodeSet.size());
    return r;
  }
  if (name == "name" || name == "local-name") {
    auto args = evalArgs();
    const Node *n = &contextNode;
    if (!args.empty() && args[0].type == XPathResultType::NodeSet && !args[0].nodeSet.empty()) {
      n = args[0].nodeSet.front();
    }
    XPathResult r;
    r.type = XPathResultType::String;
    r.stringValue = (name == "local-name") ? nodeLocalName(*n) : nodeName(*n);
    return r;
  }
  if (name == "namespace-uri") {
    auto args = evalArgs();
    const Node *n = &contextNode;
    if (!args.empty() && args[0].type == XPathResultType::NodeSet && !args[0].nodeSet.empty()) {
      n = args[0].nodeSet.front();
    }
    XPathResult r;
    r.type = XPathResultType::String;
    r.stringValue = nodeNamespaceURI(*n);
    return r;
  }

  // --- Boolean functions ---
  if (name == "true") {
    XPathResult r;
    r.type = XPathResultType::Boolean;
    r.boolValue = true;
    return r;
  }
  if (name == "false") {
    XPathResult r;
    r.type = XPathResultType::Boolean;
    r.boolValue = false;
    return r;
  }
  if (name == "not") {
    auto args = evalArgs();
    XPathResult r;
    r.type = XPathResultType::Boolean;
    r.boolValue = args.empty() ? true : !resultToBool(args[0]);
    return r;
  }
  if (name == "boolean") {
    auto args = evalArgs();
    XPathResult r;
    r.type = XPathResultType::Boolean;
    r.boolValue = args.empty() ? false : resultToBool(args[0]);
    return r;
  }
  if (name == "lang") {
    // Simplified: always return false
    XPathResult r;
    r.type = XPathResultType::Boolean;
    r.boolValue = false;
    return r;
  }

  // --- Number functions ---
  if (name == "number") {
    auto args = evalArgs();
    XPathResult r;
    r.type = XPathResultType::Number;
    r.numberValue = args.empty() ? std::numeric_limits<double>::quiet_NaN() : resultToNumber(args[0]);
    return r;
  }
  if (name == "sum") {
    auto args = evalArgs();
    double total = 0.0;
    if (!args.empty() && args[0].type == XPathResultType::NodeSet) {
      for (const auto *n : args[0].nodeSet) {
        const std::string sv = nodeStringValue(*n);
        try {
          total += std::stod(sv);
        } catch (...) {
          total = std::numeric_limits<double>::quiet_NaN();
          break;
        }
      }
    }
    XPathResult r;
    r.type = XPathResultType::Number;
    r.numberValue = total;
    return r;
  }
  if (name == "floor") {
    auto args = evalArgs();
    XPathResult r;
    r.type = XPathResultType::Number;
    r.numberValue = args.empty() ? std::numeric_limits<double>::quiet_NaN() : std::floor(resultToNumber(args[0]));
    return r;
  }
  if (name == "ceiling") {
    auto args = evalArgs();
    XPathResult r;
    r.type = XPathResultType::Number;
    r.numberValue = args.empty() ? std::numeric_limits<double>::quiet_NaN() : std::ceil(resultToNumber(args[0]));
    return r;
  }
  if (name == "round") {
    auto args = evalArgs();
    XPathResult r;
    r.type = XPathResultType::Number;
    if (args.empty()) {
      r.numberValue = std::numeric_limits<double>::quiet_NaN();
      return r;
    }
    const double v = resultToNumber(args[0]);
    r.numberValue = std::floor(v + 0.5);
    return r;
  }

  // --- String functions ---
  if (name == "string") {
    auto args = evalArgs();
    XPathResult r;
    r.type = XPathResultType::String;
    r.stringValue = args.empty() ? nodeStringValue(contextNode) : resultToString(args[0]);
    return r;
  }
  if (name == "concat") {
    auto args = evalArgs();
    std::string s;
    for (const auto &a : args) s += resultToString(a);
    XPathResult r;
    r.type = XPathResultType::String;
    r.stringValue = s;
    return r;
  }
  if (name == "starts-with") {
    auto args = evalArgs();
    XPathResult r;
    r.type = XPathResultType::Boolean;
    if (args.size() < 2) {
      r.boolValue = false;
      return r;
    }
    r.boolValue = resultToString(args[0]).starts_with(resultToString(args[1]));
    return r;
  }
  if (name == "contains") {
    auto args = evalArgs();
    XPathResult r;
    r.type = XPathResultType::Boolean;
    if (args.size() < 2) {
      r.boolValue = false;
      return r;
    }
    r.boolValue = resultToString(args[0]).find(resultToString(args[1])) != std::string::npos;
    return r;
  }
  if (name == "string-length") {
    auto args = evalArgs();
    XPathResult r;
    r.type = XPathResultType::Number;
    const std::string s = args.empty() ? nodeStringValue(contextNode) : resultToString(args[0]);
    r.numberValue = static_cast<double>(s.size());
    return r;
  }
  if (name == "normalize-space") {
    auto args = evalArgs();
    const std::string s = args.empty() ? nodeStringValue(contextNode) : resultToString(args[0]);
    XPathResult r;
    r.type = XPathResultType::String;
    r.stringValue = fnNormalizeSpace(s);
    return r;
  }
  if (name == "translate") {
    auto args = evalArgs();
    XPathResult r;
    r.type = XPathResultType::String;
    if (args.size() < 3) {
      r.stringValue = args.empty() ? "" : resultToString(args[0]);
      return r;
    }
    r.stringValue = fnTranslate(resultToString(args[0]), resultToString(args[1]), resultToString(args[2]));
    return r;
  }
  if (name == "substring") {
    auto args = evalArgs();
    XPathResult r;
    r.type = XPathResultType::String;
    if (args.empty()) { return r; }
    const std::string s = resultToString(args[0]);
    const double startD = (args.size() >= 2) ? std::round(resultToNumber(args[1])) : 1.0;
    // XPath's substring is 1-based
    const long start = static_cast<long>(startD) - 1;
    if (args.size() >= 3) {
      const long len = static_cast<long>(std::round(resultToNumber(args[2])));
      const long begin = std::max(0L, start);
      const long end = std::min(static_cast<long>(s.size()), start + len);
      if (end > begin) r.stringValue = s.substr(static_cast<size_t>(begin), static_cast<size_t>(end - begin));
    } else {
      const long begin = std::max(0L, start);
      if (begin < static_cast<long>(s.size())) r.stringValue = s.substr(static_cast<size_t>(begin));
    }
    return r;
  }
  if (name == "substring-before") {
    auto args = evalArgs();
    XPathResult r;
    r.type = XPathResultType::String;
    if (args.size() < 2) return r;
    const std::string haystack = resultToString(args[0]);
    const std::string needle = resultToString(args[1]);
    const auto pos = haystack.find(needle);
    if (pos != std::string::npos) r.stringValue = haystack.substr(0, pos);
    return r;
  }
  if (name == "substring-after") {
    auto args = evalArgs();
    XPathResult r;
    r.type = XPathResultType::String;
    if (args.size() < 2) return r;
    const std::string haystack = resultToString(args[0]);
    const std::string needle = resultToString(args[1]);
    const auto pos = haystack.find(needle);
    if (pos != std::string::npos) r.stringValue = haystack.substr(pos + needle.size());
    return r;
  }

  // --- Internal synthetic: path continuation (filter then path) ---
  if (name == "__pathcont__") {
    // args[0] = filter expression,  args[1] = path expression
    if (argExprs.size() < 2) {
      XPathResult r;
      r.type = XPathResultType::NodeSet;
      return r;
    }
    XPathResult filterResult =
      evalExpr(*argExprs[0], contextNode, contextPosition, contextSize, docRoot, ancestorStack);
    if (filterResult.type != XPathResultType::NodeSet) {
      XPathResult r;
      r.type = XPathResultType::NodeSet;
      return r;
    }
    // Apply the path expression to each node in the filter result
    // The path is a PathExpr (already has steps)
    const auto *pathExprPtr = dynamic_cast<const XPathPathExpr *>(argExprs[1].get());
    if (!pathExprPtr) { return filterResult; }
    std::vector<const Node *> combined;
    for (const auto *n : filterResult.nodeSet) {
      auto sub = evalPathExpr(*pathExprPtr, *n, docRoot, ancestorStack);
      for (const auto *r : sub.nodeSet) {
        if (!std::ranges::contains(combined, r)) combined.push_back(r);
      }
    }
    XPathResult result;
    result.type = XPathResultType::NodeSet;
    result.nodeSet = combined;
    return result;
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
    XPathResult r;
    r.type = XPathResultType::NodeSet;
    r.nodeSet = left.type == XPathResultType::NodeSet ? left.nodeSet : std::vector<const Node *>{};
    if (right.type == XPathResultType::NodeSet) {
      for (const auto *n : right.nodeSet) {
        if (!std::ranges::contains(r.nodeSet, n)) r.nodeSet.push_back(n);
      }
    }
    return r;
  }

  // Binary expression
  if (const auto *b = dynamic_cast<const XPathBinaryExpr *>(&expr)) {
    // Short-circuit for and / or
    if (b->op == XPathBinaryExpr::Op::And) {
      auto left = evalExpr(*b->left, contextNode, contextPosition, contextSize, documentRoot, ancestorStack);
      if (!resultToBool(left)) {
        XPathResult r;
        r.type = XPathResultType::Boolean;
        r.boolValue = false;
        return r;
      }
      auto right = evalExpr(*b->right, contextNode, contextPosition, contextSize, documentRoot, ancestorStack);
      XPathResult r;
      r.type = XPathResultType::Boolean;
      r.boolValue = resultToBool(right);
      return r;
    }
    if (b->op == XPathBinaryExpr::Op::Or) {
      auto left = evalExpr(*b->left, contextNode, contextPosition, contextSize, documentRoot, ancestorStack);
      if (resultToBool(left)) {
        XPathResult r;
        r.type = XPathResultType::Boolean;
        r.boolValue = true;
        return r;
      }
      auto right = evalExpr(*b->right, contextNode, contextPosition, contextSize, documentRoot, ancestorStack);
      XPathResult r;
      r.type = XPathResultType::Boolean;
      r.boolValue = resultToBool(right);
      return r;
    }

    auto left = evalExpr(*b->left, contextNode, contextPosition, contextSize, documentRoot, ancestorStack);
    auto right = evalExpr(*b->right, contextNode, contextPosition, contextSize, documentRoot, ancestorStack);

    // Equality / relational use special node-set comparison rules
    auto nodeSetContains = [&](const XPathResult &nodeSetRes, const XPathResult &other) -> bool {
      if (nodeSetRes.type != XPathResultType::NodeSet) return false;
      for (const auto *n : nodeSetRes.nodeSet) {
        // Use attribute value if this is an attribute-proxy node
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
        // node-set vs node-set: compare string values
        if (other.type == XPathResultType::NodeSet) {
          for (const auto *m : other.nodeSet) {
            if (sv == nodeStringValue(*m)) return true;
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
      XPathResult r;
      r.type = XPathResultType::Boolean;
      r.boolValue = (b->op == XPathBinaryExpr::Op::Eq) ? eq : !eq;
      return r;
    }

    // Relational and arithmetic: coerce to numbers
    const double lv = resultToNumber(left);
    const double rv = resultToNumber(right);
    XPathResult r;
    switch (b->op) {
    case XPathBinaryExpr::Op::Lt:
      r.type = XPathResultType::Boolean;
      r.boolValue = lv < rv;
      break;
    case XPathBinaryExpr::Op::Gt:
      r.type = XPathResultType::Boolean;
      r.boolValue = lv > rv;
      break;
    case XPathBinaryExpr::Op::LtEq:
      r.type = XPathResultType::Boolean;
      r.boolValue = lv <= rv;
      break;
    case XPathBinaryExpr::Op::GtEq:
      r.type = XPathResultType::Boolean;
      r.boolValue = lv >= rv;
      break;
    case XPathBinaryExpr::Op::Add:
      r.type = XPathResultType::Number;
      r.numberValue = lv + rv;
      break;
    case XPathBinaryExpr::Op::Sub:
      r.type = XPathResultType::Number;
      r.numberValue = lv - rv;
      break;
    case XPathBinaryExpr::Op::Mul:
      r.type = XPathResultType::Number;
      r.numberValue = lv * rv;
      break;
    case XPathBinaryExpr::Op::Div:
      r.type = XPathResultType::Number;
      r.numberValue = (rv == 0.0) ? std::numeric_limits<double>::infinity() : lv / rv;
      break;
    case XPathBinaryExpr::Op::Mod:
      r.type = XPathResultType::Number;
      r.numberValue = std::fmod(lv, rv);
      break;
    default:
      break;
    }
    return r;
  }

  // Unary minus
  if (const auto *u = dynamic_cast<const XPathUnaryExpr *>(&expr)) {
    auto val = evalExpr(*u->operand, contextNode, contextPosition, contextSize, documentRoot, ancestorStack);
    XPathResult r;
    r.type = XPathResultType::Number;
    r.numberValue = -resultToNumber(val);
    return r;
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
    XPathResult r;
    r.type = XPathResultType::String;
    r.stringValue = sl->value;
    return r;
  }

  // Number literal
  if (const auto *nl = dynamic_cast<const XPathNumberLiteral *>(&expr)) {
    XPathResult r;
    r.type = XPathResultType::Number;
    r.numberValue = nl->value;
    return r;
  }

  throw XPath::Error("Internal evaluator error: unknown AST node type.");
}

// ========================================================================
// XPath_Impl public methods (called from XPath::evaluate etc.)
// ========================================================================
XPath_Impl::XPath_Impl(const Node &root) : xmlRoot(root) {}

std::vector<const Node *> XPath_Impl::evaluate(const std::string_view expression) const
{
  try {
    if (expression.empty()) { throw XPath::Error("Empty expression."); }
    const auto tokens = xpathTokenize(expression);
    const auto ast = xpathParse(tokens);
    const std::vector<const Node *> emptyAncestors;
    XPathResult result = evalExpr(*ast, xmlRoot, 1, 1, xmlRoot, emptyAncestors);
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
    if (expression.empty()) { throw XPath::Error("Empty expression."); }
    const auto tokens = xpathTokenize(expression);
    const auto ast = xpathParse(tokens);
    const std::vector<const Node *> emptyAncestors;
    XPathResult result = evalExpr(*ast, xmlRoot, 1, 1, xmlRoot, emptyAncestors);
    return resultToString(result);
  } catch (const XPath::Error &) {
    throw;
  } catch (const std::exception &e) {
    throw XPath::Error(e.what());
  }
}

bool XPath_Impl::evaluateBool(const std::string_view expression) const
{
  try {
    if (expression.empty()) { throw XPath::Error("Empty expression."); }
    const auto tokens = xpathTokenize(expression);
    const auto ast = xpathParse(tokens);
    const std::vector<const Node *> emptyAncestors;
    XPathResult result = evalExpr(*ast, xmlRoot, 1, 1, xmlRoot, emptyAncestors);
    return resultToBool(result);
  } catch (const XPath::Error &) {
    throw;
  } catch (const std::exception &e) {
    throw XPath::Error(e.what());
  }
}

double XPath_Impl::evaluateNumber(const std::string_view expression) const
{
  try {
    if (expression.empty()) { throw XPath::Error("Empty expression."); }
    const auto tokens = xpathTokenize(expression);
    const auto ast = xpathParse(tokens);
    const std::vector<const Node *> emptyAncestors;
    XPathResult result = evalExpr(*ast, xmlRoot, 1, 1, xmlRoot, emptyAncestors);
    return resultToNumber(result);
  } catch (const XPath::Error &) {
    throw;
  } catch (const std::exception &e) {
    throw XPath::Error(e.what());
  }
}

}// namespace XML_Lib
