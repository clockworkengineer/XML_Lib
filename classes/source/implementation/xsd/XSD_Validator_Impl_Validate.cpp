//
// Class: XSD_Impl_Validate
//
// Description: Validate a parsed XML tree against a loaded XSD schema.
//
// Dependencies: C++20 - Language standard features used.
//

#include "XSD_Impl.hpp"
#include "XML_NodeKindHelpers.hpp"
#include "XPath.hpp"
#include "xsd/XSD_ValidateHelpers.hpp"
#include "common/XML_QName.hpp"

#include <charconv>
#include <functional>
#include <regex>
#include <set>

namespace XML_Lib {

namespace {
template<typename Cmp>
void checkNumericFacet(const std::string &value,
  const std::string &bound,
  const std::string &context,
  const std::string &message,
  Cmp cmp)
{
  const double dval  = std::stod(value);
  const double dbound = std::stod(bound);
  if (cmp(dval, dbound)) { xsdError(context, message); }
}

template<typename Cmp>
void checkLengthFacet(const std::string &value,
  const std::string &bound,
  const std::string &context,
  const std::string &message,
  Cmp cmp)
{
  if (cmp(value.size(), static_cast<std::size_t>(std::stoul(bound)))) { xsdError(context, message); }
}
} // namespace

// ----------------------------------------------------------------
// Built-in type checking
// ----------------------------------------------------------------

void XSD_Impl::validateBuiltinType(const std::string &value, const std::string &typeName, const std::string &context)
{
  if (typeName == "xs:boolean") {
    if (value != "true" && value != "false" && value != "1" && value != "0") {
      xsdError(context, "value '" + value + "' is not a valid xs:boolean.");
    }
  } else if (typeName == "xs:integer" || typeName == "xs:int" || typeName == "xs:long" || typeName == "xs:short"
             || typeName == "xs:byte" || typeName == "xs:positiveInteger" || typeName == "xs:negativeInteger"
             || typeName == "xs:nonNegativeInteger" || typeName == "xs:nonPositiveInteger"
             || typeName == "xs:unsignedInt" || typeName == "xs:unsignedLong" || typeName == "xs:unsignedShort"
             || typeName == "xs:unsignedByte") {
    // Must parse as integer
    long long parsed{};
    const auto *end = value.data() + value.size();
    auto [ptr, ec] = std::from_chars(value.data(), end, parsed);
    if (ec != std::errc{} || ptr != end) {
      xsdError(context, "value '" + value + "' is not a valid " + typeName + ".");
    }
    // Additional range checks
    if (typeName == "xs:positiveInteger" && parsed <= 0) {
      xsdError(context, "value '" + value + "' must be positive for xs:positiveInteger.");
    }
    if (typeName == "xs:negativeInteger" && parsed >= 0) {
      xsdError(context, "value '" + value + "' must be negative for xs:negativeInteger.");
    }
    if (typeName == "xs:nonNegativeInteger" && parsed < 0) {
      xsdError(context, "value '" + value + "' must be >= 0 for xs:nonNegativeInteger.");
    }
    if (typeName == "xs:nonPositiveInteger" && parsed > 0) {
      xsdError(context, "value '" + value + "' must be <= 0 for xs:nonPositiveInteger.");
    }
  } else if (typeName == "xs:decimal" || typeName == "xs:float" || typeName == "xs:double") {
    try {
      std::size_t idx{};
      [[maybe_unused]] const auto dval = std::stod(value, &idx);
      if (idx != value.size()) { xsdError(context, "value '" + value + "' is not a valid " + typeName + "."); }
    } catch (...) {
      xsdError(context, "value '" + value + "' is not a valid " + typeName + ".");
    }
  }
  // xs:string, xs:anyURI, xs:date, xs:dateTime etc. accept any text — no further check in Phase 1
}

// ----------------------------------------------------------------
// Restriction facet checking
// ----------------------------------------------------------------

void XSD_Impl::validateRestrictions(const std::string &value, const XSD_SimpleType &st, const std::string &context)
{
  // First validate against the base type
  if (!st.baseType.empty() && isBuiltinType(st.baseType)) { validateBuiltinType(value, st.baseType, context); }

  std::vector<std::string> allowedEnums;
  bool hasEnum = false;

  for (const auto &r : st.restrictions) {
    switch (r.facet) {
    case XSD_Restriction::Facet::minLength:
      checkLengthFacet(value, r.value, context,
        "value length " + std::to_string(value.size()) + " is less than minLength " + r.value + ".",
        std::less<std::size_t>{});
      break;
    case XSD_Restriction::Facet::maxLength:
      checkLengthFacet(value, r.value, context,
        "value length " + std::to_string(value.size()) + " exceeds maxLength " + r.value + ".",
        std::greater<std::size_t>{});
      break;
    case XSD_Restriction::Facet::length:
      checkLengthFacet(value, r.value, context,
        "value length " + std::to_string(value.size()) + " does not equal required length " + r.value + ".",
        std::not_equal_to<std::size_t>{});
      break;
    case XSD_Restriction::Facet::pattern: {
      std::regex re(r.value);
      if (!std::regex_match(value, re)) {
        xsdError(context, "value '" + value + "' does not match pattern '" + r.value + "'.");
      }
      break;
    }
    case XSD_Restriction::Facet::enumeration:
      hasEnum = true;
      allowedEnums.push_back(r.value);
      break;
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
    case XSD_Restriction::Facet::totalDigits:
    case XSD_Restriction::Facet::fractionDigits:
    case XSD_Restriction::Facet::whiteSpace:
      // Phase 2
      break;
    }
  }

  if (hasEnum && std::ranges::find(allowedEnums, value) == allowedEnums.end()) {
    xsdError(context, "value '" + value + "' is not in the allowed enumeration set.");
  }
}

// ----------------------------------------------------------------
// Simple value validation
// ----------------------------------------------------------------

void XSD_Impl::validateSimpleValue(const std::string &value, const std::string &typeRef, const std::string &context)
{
  if (typeRef.empty()) { return; }
  if (isBuiltinType(typeRef)) {
    validateBuiltinType(value, typeRef, context);
    return;
  }
  if (const auto *st = findSimpleType(typeRef)) {
    validateRestrictions(value, *st, context);
    return;
  }
  // Unknown type — skip (could be a complex type used for element content)
}

// ----------------------------------------------------------------
// Attribute validation
// ----------------------------------------------------------------

void XSD_Impl::validateAttributes(const Node &xNode, const XSD_ComplexType &type, bool nillable)
{
  const auto &elem = NRef<Element>(xNode);
  const std::string &elemName = elem.name();

  validateDeclaredAttributes(elem, type, elemName);
  validateUndeclaredAttributes(elem, type, elemName, nillable);
}

void XSD_Impl::validateDeclaredAttributes(const Element &elem,
  const XSD_ComplexType &type,
  const std::string &elemName)
{
  for (const auto &declAttr : type.attributes) { validateDeclaredAttribute(elem, declAttr, elemName); }
}

void XSD_Impl::validateDeclaredAttribute(const Element &elem,
  const XSD_AttributeDecl &declAttr,
  const std::string &elemName)
{
  const bool present = elem.hasAttribute(declAttr.name);
  if (declAttr.use == XSD_AttributeDecl::Use::required && !present) {
    xsdError(elemName, "missing required attribute '" + declAttr.name + "'.");
  }
  if (declAttr.use == XSD_AttributeDecl::Use::prohibited && present) {
    xsdError(elemName, "attribute '" + declAttr.name + "' is prohibited.");
  }

  const bool hasDefaultOrFixed = !declAttr.defaultValue.empty() || !declAttr.fixedValue.empty();
  if (!present && !hasDefaultOrFixed) { return; }

  const std::string attrValue = present ? elem[declAttr.name].getParsed()
                                        : (!declAttr.fixedValue.empty() ? declAttr.fixedValue : declAttr.defaultValue);

  if (!declAttr.fixedValue.empty() && attrValue != declAttr.fixedValue) {
    xsdError(elemName,
      "attribute '" + declAttr.name + "' must have fixed value '" + declAttr.fixedValue + "' but got '" + attrValue
        + "'.");
  }

  if (!declAttr.typeRef.empty()) {
    validateSimpleValue(attrValue, declAttr.typeRef, elemName + "/@" + declAttr.name);
  }
}

void XSD_Impl::validateUndeclaredAttributes(const Element &elem,
  const XSD_ComplexType &type,
  const std::string &elemName,
  bool nillable)
{
  if (type.hasAnyAttribute) { return; }

  for (const auto &attr : elem.getAttributes()) {
    const auto &attrName = attr.getName();
    if (attrName.starts_with("xmlns")) { continue; }
    if (nillable && attrName == "xsi:nil") { continue; }
    const bool declared =
      std::ranges::any_of(type.attributes, [&](const XSD_AttributeDecl &d) { return d.name == attrName; });
    if (!declared) { xsdError(elemName, "undeclared attribute '" + attrName + "'."); }
  }
}

// ----------------------------------------------------------------
// Content model validation helpers
// ----------------------------------------------------------------

/// Return the text content of a node (concatenated content children, trimmed).
static std::string getTextContent(const Node &xNode)
{
  std::string result;
  for (const auto &child : xNode.getChildren()) {
    if (isA<Content>(child)) { result += child.getContents(); }
  }
  // Trim leading/trailing whitespace
  const auto start = result.find_first_not_of(" \t\r\n");
  if (start == std::string::npos) { return ""; }
  return result.substr(start, result.find_last_not_of(" \t\r\n") - start + 1);
}

/// Extract text content from xNode, then validate it against a named type
/// (simple type lookup first, then builtin type fallback).
void XSD_Impl::validateNodeText(const Node &xNode, const std::string &typeRef, const std::string &context)
{
  const auto text = getTextContent(xNode);
  if (const auto *st = findSimpleType(typeRef)) {
    validateRestrictions(text, *st, context);
  } else if (isBuiltinType(typeRef)) {
    validateBuiltinType(text, typeRef, context);
  }
}

// ----------------------------------------------------------------
// Element validation
// ----------------------------------------------------------------

static bool hasXsiNilTrue(const Element &elem)
{
  if (!elem.hasAttribute("xsi:nil")) { return false; }
  const auto value = elem["xsi:nil"].getParsed();
  return value == "true" || value == "1";
}

void XSD_Impl::validateElement(const Node &xNode, const XSD_ComplexType &type, bool nillable)
{
  const auto &elem = NRef<Element>(xNode);
  const std::string &elemName = elem.name();
  const bool nilled = hasXsiNilTrue(elem);

  if (nilled && !nillable) {
    xsdError(elemName, "xsi:nil='true' is not allowed for non-nillable element.");
  }

  // Validate attributes
  validateAttributes(xNode, type, nillable);

  if (nilled) {
    if (!getTextContent(xNode).empty()) {
      xsdError(elemName, "xsi:nil='true' element must be empty.");
    }
    for (const auto &child : xNode.getChildren()) {
      if (isElementLikeNode(child)) {
        xsdError(elemName, "xsi:nil='true' element must not contain child elements.");
      }
    }
    return;
  }

  if (type.compositor == XSD_ComplexType::Compositor::none) {
    // Simple content — validate text against any declared simple content base type
    const auto text = getTextContent(xNode);
    if (!text.empty()) { validateSimpleValue(text, type.baseType, elemName); }
    return;
  }

  // Collect actual child element occurrence counts
  auto childCounts = collectElementChildCounts(xNode);

  if (type.compositor == XSD_ComplexType::Compositor::sequence || type.compositor == XSD_ComplexType::Compositor::all) {

    validateParticleOccurrenceBounds(xNode, childCounts, type, elemName);
    validateUnexpectedChildren(childCounts, type, elemName);

  } else if (type.compositor == XSD_ComplexType::Compositor::choice) {
    // Exactly one branch must be present
    bool branchMatched = false;
    for (const auto &particle : type.particles) {
      const auto count = childCounts.contains(particle.elementName) ? childCounts.at(particle.elementName) : 0u;
      if (count > 0) {
        if (branchMatched) {
          xsdError(elemName, "xs:choice allows only one branch, but multiple child elements are present.");
        }
        branchMatched = true;
        if (particle.maxOccurs != 0 && count > particle.maxOccurs) {
          xsdError(elemName,
            "child element <" + particle.elementName + "> appears " + std::to_string(count)
              + " times but maxOccurs=" + std::to_string(particle.maxOccurs) + ".");
        }
      }
    }
    if (!branchMatched && !type.particles.empty()) {
      // Build list of expected names
      std::string options;
      for (const auto &p : type.particles) {
        if (!options.empty()) { options += ", "; }
        options += "<" + p.elementName + ">";
      }
      xsdError(elemName, "xs:choice requires exactly one of: " + options + ".");
    }
    // Check for unexpected children not in any branch
    validateUnexpectedChildren(childCounts, type, elemName);
  }

  // Recursively validate child elements
  for (const auto &child : xNode.getChildren()) {
    if (!isElementLikeNode(child)) { continue; }
    const auto &childElem = NRef<Element>(child);
    const auto &childName = childElem.name();

    // Find this child's declared particle
    const XSD_Particle *particle = findDeclaredParticle(type, childName);
    if (!particle) { continue; }

    // Resolve type for child
    const auto &typeRef = particle->inlineComplexType  ? particle->inlineComplexType->name
                          : particle->inlineSimpleType ? particle->inlineSimpleType->name
                                                       : particle->typeRef;

    if (particle->inlineComplexType) {
      validateElement(child, *particle->inlineComplexType, particle->nillable);
    } else if (particle->inlineSimpleType) {
      if (hasXsiNilTrue(childElem)) {
        if (!particle->nillable) {
          xsdError(childName, "xsi:nil='true' is not allowed for non-nillable element.");
        }
      } else {
        validateRestrictions(getTextContent(child), *particle->inlineSimpleType, childName);
      }
    } else if (!typeRef.empty()) {
      if (hasXsiNilTrue(childElem)) {
        if (!particle->nillable) {
          xsdError(childName, "xsi:nil='true' is not allowed for non-nillable element.");
        }
      } else if (const auto *ct = findComplexType(typeRef)) {
        validateElement(child, *ct, particle->nillable);
      } else {
        validateNodeText(child, typeRef, childName);
      }
    }
  }
}

// ----------------------------------------------------------------
// Top-level validate entry point
// ----------------------------------------------------------------

void XSD_Impl::validate(const Node &xNode)
{
  // xNode is the root element of the XML
  const auto &rootElem = NRef<Element>(xNode);
  const auto &rootName = rootElem.name();

  // Find matching top-level element declaration
  const auto *decl = findTopLevelElement(rootName);
  if (!decl) {
    // Check local name only (in case of namespace prefix)
    const auto localName = getLocalName(rootName);
    for (const auto &d : rootElements) {
      if (d.name == localName) {
        decl = &d;
        break;
      }
    }
  }
  if (!decl) { xsdError(rootName, "element <" + rootName + "> is not declared in the schema."); }

  // Check fixed value on root element content
  if (!decl->fixedValue.empty()) {
    const auto text = getTextContent(xNode);
    if (text != decl->fixedValue) {
      xsdError(rootName, "content '" + text + "' does not match fixed value '" + decl->fixedValue + "'.");
    }
  }

  if (decl->typeRef.empty()) {
    // No type declared — well-formed only
    return;
  }

  if (const auto *ct = findComplexType(decl->typeRef)) {
    validateElement(xNode, *ct, decl->nillable);
  } else {
    if (hasXsiNilTrue(rootElem)) {
      if (!decl->nillable) {
        xsdError(rootName, "xsi:nil='true' is not allowed for non-nillable element.");
      }
    } else {
      validateNodeText(xNode, decl->typeRef, rootName);
    }
  }

  validateIdentityConstraints(xNode, *decl);
}

std::vector<std::string> XSD_Impl::evaluateIdentityConstraintFields(const Node &contextNode,
  const XSD_IdentityConstraint &constraint) const
{
  XPath xpath(contextNode);
  std::vector<std::string> values;
  for (const auto &fieldExpr : constraint.fields) {
    values.push_back(xpath.evaluateString(fieldExpr));
  }
  return values;
}

std::vector<std::vector<std::string>> XSD_Impl::collectIdentityConstraintValues(const Node &xNode,
  const XSD_IdentityConstraint &constraint) const
{
  XPath xpath(xNode);
  std::vector<std::vector<std::string>> tuples;
  const auto selected = xpath.evaluate(constraint.selector);
  for (const auto *node : selected) {
    tuples.push_back(evaluateIdentityConstraintFields(*node, constraint));
  }
  return tuples;
}

void XSD_Impl::validateIdentityConstraints(const Node &xNode, const XSD_ElementDecl &decl)
{
  if (decl.identityConstraints.empty()) { return; }

  std::map<std::string, std::set<std::vector<std::string>>> identityValues;

  for (const auto &constraint : decl.identityConstraints) {
    if (constraint.kind == XSD_IdentityConstraint::Kind::key
      || constraint.kind == XSD_IdentityConstraint::Kind::unique) {
      const auto tuples = collectIdentityConstraintValues(xNode, constraint);
      for (const auto &tuple : tuples) {
        auto &values = identityValues[constraint.name];
        if (values.contains(tuple)) {
          xsdError(NRef<Element>(xNode).name(),
            "identity constraint '" + constraint.name + "' requires unique values, but duplicate value was found.");
        }
        values.insert(tuple);
      }
    }
  }

  for (const auto &constraint : decl.identityConstraints) {
    if (constraint.kind != XSD_IdentityConstraint::Kind::keyref) { continue; }
    const auto it = identityValues.find(constraint.refer);
    if (it == identityValues.end()) {
      xsdError(NRef<Element>(xNode).name(),
        "keyref '" + constraint.name + "' references undefined key '" + constraint.refer + "'.");
    }
    const auto &referencedValues = it->second;
    const auto tuples = collectIdentityConstraintValues(xNode, constraint);
    for (const auto &tuple : tuples) {
      if (!referencedValues.contains(tuple)) {
        xsdError(NRef<Element>(xNode).name(),
          "keyref '" + constraint.name + "' refers to a missing key value.");
      }
    }
  }
}

}// namespace XML_Lib
