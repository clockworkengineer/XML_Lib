//
// Class: XSD_Impl_Validate
//
// Description: Validate a parsed XML tree against a loaded XSD schema.
//
// Dependencies: C++20 - Language standard features used.
//

#include "XSD_Impl.hpp"

#include <charconv>
#include <regex>

namespace XML_Lib {

// ----------------------------------------------------------------
// Error helper
// ----------------------------------------------------------------

static void xsdError(const std::string &elementName, const std::string &message)
{
  throw IValidator::Error("XSD Validation Error [Element: " + elementName + "] " + message);
}

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
      if (value.size() < static_cast<std::size_t>(std::stoul(r.value))) {
        xsdError(context, "value length " + std::to_string(value.size()) + " is less than minLength " + r.value + ".");
      }
      break;
    case XSD_Restriction::Facet::maxLength:
      if (value.size() > static_cast<std::size_t>(std::stoul(r.value))) {
        xsdError(context, "value length " + std::to_string(value.size()) + " exceeds maxLength " + r.value + ".");
      }
      break;
    case XSD_Restriction::Facet::length:
      if (value.size() != static_cast<std::size_t>(std::stoul(r.value))) {
        xsdError(
          context, "value length " + std::to_string(value.size()) + " does not equal required length " + r.value + ".");
      }
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
      if (dval <= dmin) {
        xsdError(context, "value '" + value + "' is not greater than minExclusive '" + r.value + "'.");
      }
      break;
    }
    case XSD_Restriction::Facet::maxExclusive: {
      const double dval = std::stod(value);
      const double dmax = std::stod(r.value);
      if (dval >= dmax) { xsdError(context, "value '" + value + "' is not less than maxExclusive '" + r.value + "'."); }
      break;
    }
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

void XSD_Impl::validateAttributes(const Node &xNode, const XSD_ComplexType &type)
{
  const auto &elem = NRef<Element>(xNode);
  const std::string &elemName = elem.name();

  // Check declared attributes
  for (const auto &declAttr : type.attributes) {
    const bool present = elem.hasAttribute(declAttr.name);
    if (declAttr.use == XSD_AttributeDecl::Use::required && !present) {
      xsdError(elemName, "missing required attribute '" + declAttr.name + "'.");
    }
    if (declAttr.use == XSD_AttributeDecl::Use::prohibited && present) {
      xsdError(elemName, "attribute '" + declAttr.name + "' is prohibited.");
    }
    if (present) {
      const auto attrVal = elem[declAttr.name].getParsed();
      if (!declAttr.fixedValue.empty() && attrVal != declAttr.fixedValue) {
        xsdError(elemName,
          "attribute '" + declAttr.name + "' must have fixed value '" + declAttr.fixedValue + "' but got '" + attrVal
            + "'.");
      }
      if (!declAttr.typeRef.empty()) {
        validateSimpleValue(attrVal, declAttr.typeRef, elemName + "/@" + declAttr.name);
      }
    }
  }

  // Check for undeclared attributes (skip xmlns* always)
  if (!type.hasAnyAttribute) {
    for (const auto &attr : elem.getAttributes()) {
      const auto &attrName = attr.getName();
      if (attrName.starts_with("xmlns")) { continue; }
      const bool declared =
        std::ranges::any_of(type.attributes, [&](const XSD_AttributeDecl &d) { return d.name == attrName; });
      if (!declared) { xsdError(elemName, "undeclared attribute '" + attrName + "'."); }
    }
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

// ----------------------------------------------------------------
// Element validation
// ----------------------------------------------------------------

void XSD_Impl::validateElement(const Node &xNode, const XSD_ComplexType &type)
{
  const auto &elem = NRef<Element>(xNode);
  const std::string &elemName = elem.name();

  // Validate attributes
  validateAttributes(xNode, type);

  if (type.compositor == XSD_ComplexType::Compositor::none) {
    // Simple content — validate text
    const auto text = getTextContent(xNode);
    if (!text.empty()) { validateSimpleValue(text, "", elemName); }
    return;
  }

  // Build a map of actual child element counts
  std::unordered_map<std::string, uint32_t> childCounts;
  std::vector<std::string> childOrder;// for sequence validation

  for (const auto &child : xNode.getChildren()) {
    if (isA<Element>(child) || isA<Root>(child) || isA<Self>(child)) {
      const auto &childElem = NRef<Element>(child);
      const auto &childName = childElem.name();
      if (childCounts[childName]++ == 0) { childOrder.push_back(childName); }
    }
  }

  if (type.compositor == XSD_ComplexType::Compositor::sequence || type.compositor == XSD_ComplexType::Compositor::all) {

    // Check each declared particle
    for (const auto &particle : type.particles) {
      const auto count = childCounts.contains(particle.elementName) ? childCounts.at(particle.elementName) : 0u;

      // Missing required element
      if (count < particle.minOccurs) {
        xsdError(elemName,
          "required child element <" + particle.elementName + "> is missing (expected at least "
            + std::to_string(particle.minOccurs) + ", found " + std::to_string(count) + ").");
      }
      // Too many occurrences
      if (particle.maxOccurs != 0 && count > particle.maxOccurs) {
        xsdError(elemName,
          "child element <" + particle.elementName + "> appears " + std::to_string(count)
            + " times but maxOccurs=" + std::to_string(particle.maxOccurs) + ".");
      }
    }

    // Check for unexpected children
    for (const auto &[childName, count] : childCounts) {
      const bool declared =
        std::ranges::any_of(type.particles, [&](const XSD_Particle &p) { return p.elementName == childName; });
      if (!declared) { xsdError(elemName, "unexpected child element <" + childName + ">."); }
    }

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
    for (const auto &[childName, count] : childCounts) {
      const bool declared =
        std::ranges::any_of(type.particles, [&](const XSD_Particle &p) { return p.elementName == childName; });
      if (!declared) { xsdError(elemName, "unexpected child element <" + childName + ">."); }
    }
  }

  // Recursively validate child elements
  for (const auto &child : xNode.getChildren()) {
    if (!isA<Element>(child) && !isA<Root>(child) && !isA<Self>(child)) { continue; }
    const auto &childElem = NRef<Element>(child);
    const auto &childName = childElem.name();

    // Find this child's declared particle
    const XSD_Particle *particle = nullptr;
    for (const auto &p : type.particles) {
      if (p.elementName == childName) {
        particle = &p;
        break;
      }
    }
    if (!particle) { continue; }

    // Resolve type for child
    const auto &typeRef = particle->inlineComplexType  ? particle->inlineComplexType->name
                          : particle->inlineSimpleType ? particle->inlineSimpleType->name
                                                       : particle->typeRef;

    if (particle->inlineComplexType) {
      validateElement(child, *particle->inlineComplexType);
    } else if (particle->inlineSimpleType) {
      const auto text = getTextContent(child);
      validateRestrictions(text, *particle->inlineSimpleType, childName);
    } else if (!typeRef.empty()) {
      if (const auto *ct = findComplexType(typeRef)) {
        validateElement(child, *ct);
      } else if (const auto *st = findSimpleType(typeRef)) {
        const auto text = getTextContent(child);
        validateRestrictions(text, *st, childName);
      } else if (isBuiltinType(typeRef)) {
        const auto text = getTextContent(child);
        validateBuiltinType(text, typeRef, childName);
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
    const auto localName = rootName.find(':') != std::string::npos ? rootName.substr(rootName.find(':') + 1) : rootName;
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
    validateElement(xNode, *ct);
  } else if (const auto *st = findSimpleType(decl->typeRef)) {
    const auto text = getTextContent(xNode);
    validateRestrictions(text, *st, rootName);
  } else if (isBuiltinType(decl->typeRef)) {
    const auto text = getTextContent(xNode);
    validateBuiltinType(text, decl->typeRef, rootName);
  }
}

}// namespace XML_Lib
