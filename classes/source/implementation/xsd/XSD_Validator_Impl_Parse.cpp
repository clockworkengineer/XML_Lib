//
// Class: XSD_Impl_Parse
//
// Description: Parse an XSD schema (which is itself XML) into the data model.
//
// Dependencies: C++20 - Language standard features used.
//

#include "XSD_Impl.hpp"
#include "xsd/XSD_NodeHelpers.hpp"
#include <algorithm>
#include <array>
#include <charconv>
#include <unordered_map>

namespace XML_Lib {

namespace {

/// @brief
/// Implementation of parseUint.

uint32_t parseUint(const std::string_view &value, uint32_t defaultValue = 1u)
{
  if (value.empty()) { return defaultValue; }
  uint32_t result = 0;
  const auto [ptr, ec] = std::from_chars(value.data(), value.data() + value.size(), result);
  return (ec == std::errc()) ? result : defaultValue;
}

/// @brief
/// Implementation of parseOccurrenceBounds.

void parseOccurrenceBounds(const Node &node, uint32_t &minOccurs, uint32_t &maxOccurs)
{
  const auto minStr = attrValueView(node, "minOccurs");
  const auto maxStr = attrValueView(node, "maxOccurs");
  minOccurs = parseUint(minStr, 1u);
  maxOccurs = maxStr == "unbounded" ? 0u : maxStr.empty() ? 1u : parseUint(maxStr, 1u);
}

} // namespace

// ----------------------------------------------------------------
// Utility helpers
// ----------------------------------------------------------------

/// @brief
/// Return the local tag name of an XML element node (strips any namespace prefix).
/// e.g. "xs:element" -> "element", "element" -> "element"

std::string_view XSD_Impl::localTag(const Node &node)
{
  return localTagView(node);
}

/// @brief
/// Return the value of a named attribute on a node, or empty string view if absent.

std::string_view XSD_Impl::attrValue(const Node &node, const std::string_view &attrName)
{
  return attrValueView(node, attrName);
}

/// @brief
/// Normalise a type reference, removing a known xs: prefix so all stored
/// type names use the "xs:xxx" canonical form regardless of the prefix used
/// in the schema document (xsd:, xs:, etc.)

std::string XSD_Impl::resolveType(const std::string_view &typeStr)
{
  // Already fully qualified with xs:
  if (typeStr.starts_with("xs:") || typeStr.starts_with("xsd:")) {
    // Normalise to xs:
    const auto pos = typeStr.find(':');
    return "xs:" + std::string(typeStr.substr(pos + 1));
  }
  return std::string(typeStr);
}

/// @brief
/// Return a list of child Nodes that are XML element nodes
/// (skips Content, Comment, PI etc.).

std::vector<std::reference_wrapper<const Node>> XSD_Impl::childElements(const Node &node)
{
  return childElementRefs(node);
}

// ----------------------------------------------------------------
// Restriction parsing
// ----------------------------------------------------------------

/// @brief
/// Parse xs:restriction and its facet children into a SimpleType.

void XSD_Impl::parseRestriction(const Node &restrictNode, XSD_SimpleType &st)
{
  static const std::unordered_map<std::string_view, XSD_Restriction::Facet> facetMap{
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
    if (const auto it = facetMap.find(tag); it != facetMap.end()) {
      st.restrictions.emplace_back(XSD_Restriction{ it->second, std::string(val) });
    }
  }
}

// ----------------------------------------------------------------
// SimpleType parsing
// ----------------------------------------------------------------

/// @brief
/// Parse xs:simpleType node into a SimpleType struct.

void XSD_Impl::parseSimpleType(const Node &stNode, XSD_SimpleType &st)
{
  st.name = attrValue(stNode, "name");
  for (const auto &child : childElementViews(stNode)) {
    if (child.tag == "restriction") { parseRestriction(child.node, st); }
  }
}

// ----------------------------------------------------------------
// Attribute declaration parsing
// ----------------------------------------------------------------

/// @brief
/// Parse xs:attribute node into an AttributeDecl.

void XSD_Impl::parseAttributeDecl(const Node &attrNode, XSD_AttributeDecl &attr)
{
  attr.name = attrValue(attrNode, "name");
  attr.typeRef = resolveType(attrValue(attrNode, "type"));
  const auto useStr = attrValue(attrNode, "use");
  if (useStr == "required") {
    attr.use = XSD_AttributeDecl::Use::required;
  } else if (useStr == "prohibited") {
    attr.use = XSD_AttributeDecl::Use::prohibited;
  } else {
    attr.use = XSD_AttributeDecl::Use::optional;
  }
  attr.defaultValue = attrValue(attrNode, "default");
  attr.fixedValue = attrValue(attrNode, "fixed");
}

void XSD_Impl::parseIdentityConstraint(const Node &constraintNode, XSD_ElementDecl &decl)
{
  XSD_IdentityConstraint constraint;
  const auto tag = localTag(constraintNode);
  if (tag == "key") {
    constraint.kind = XSD_IdentityConstraint::Kind::key;
  } else if (tag == "unique") {
    constraint.kind = XSD_IdentityConstraint::Kind::unique;
  } else if (tag == "keyref") {
    constraint.kind = XSD_IdentityConstraint::Kind::keyref;
    constraint.refer = std::string(attrValue(constraintNode, "refer"));
  }
  constraint.name = std::string(attrValue(constraintNode, "name"));

  for (const auto &child : childElements(constraintNode)) {
    const auto &childNode = child.get();
    const auto childTag = localTag(childNode);
    if (childTag == "selector") {
      constraint.selector = std::string(attrValue(childNode, "xpath"));
    } else if (childTag == "field") {
      constraint.fields.emplace_back(attrValue(childNode, "xpath"));
    }
  }

  if (constraint.name.empty()) {
    XML_LIB_THROW(IValidator::Error("XSD identity constraint missing name."));
  }
  if (constraint.selector.empty()) {
    XML_LIB_THROW(IValidator::Error("XSD identity constraint '" + constraint.name + "' missing selector xpath."));
  }
  if (constraint.fields.empty()) {
    XML_LIB_THROW(IValidator::Error("XSD identity constraint '" + constraint.name + "' must declare at least one field."));
  }

  decl.identityConstraints.push_back(std::move(constraint));
}

// ----------------------------------------------------------------
// Particle (element child in content model) parsing
// ----------------------------------------------------------------

/// @brief
/// Parse a single xs:element particle node.

void XSD_Impl::parseParticle(const Node &particleNode, XSD_Particle &particle)
{
  const auto tag = localTag(particleNode);
  if (tag == "any") {
    particle.elementName = "*";
  } else {
    particle.elementName = std::string(attrValue(particleNode, "name"));
    particle.nillable = (attrValue(particleNode, "nillable") == "true");
  }
  parseOccurrenceBounds(particleNode, particle.minOccurs, particle.maxOccurs);
  if (tag != "any") { particle.typeRef = resolveType(attrValue(particleNode, "type")); }

  // Check for inline type declarations
  for (const auto &child : childElementViews(particleNode)) {
    const auto &childNode = child.node.get();
    const auto childTag = child.tag;
    if (childTag == "complexType") {
      particle.inlineComplexType = std::make_unique<XSD_ComplexType>();
      parseComplexType(childNode, *particle.inlineComplexType);
    } else if (childTag == "simpleType") {
      particle.inlineSimpleType = std::make_unique<XSD_SimpleType>();
      parseSimpleType(childNode, *particle.inlineSimpleType);
    }
  }
}

static constexpr std::size_t kMaxXsdContentChildren{ 4096 };

/// @brief
/// Implementation of XSD_Impl::parseChildParticleList.

void XSD_Impl::parseChildParticleList(const Node &parentNode, XSD_ComplexType &ct)
{
  const auto children = childElementViews(parentNode);
  if (children.size() > kMaxXsdContentChildren) {
    XML_LIB_THROW(std::runtime_error("XSD Error: Schema complexity exceeds maximum allowed."));
  }
  for (const auto &child : children) {
    if (child.tag == "element" || child.tag == "any") {
      XSD_Particle particle;
      parseParticle(child.node, particle);
      ct.particles.push_back(std::move(particle));
    }
  }
}

/// @brief
/// Implementation of XSD_Impl::parseChildAttributes.

void XSD_Impl::parseChildAttributes(const Node &parentNode, XSD_ComplexType &ct)
{
  const auto children = childElementViews(parentNode);
  if (children.size() > kMaxXsdContentChildren) {
    XML_LIB_THROW(std::runtime_error("XSD Error: Schema complexity exceeds maximum allowed."));
  }
  for (const auto &child : children) {
    if (child.tag == "attribute") {
      XSD_AttributeDecl attr;
      parseAttributeDecl(child.node, attr);
      ct.attributes.push_back(std::move(attr));
    }
  }
}

// ----------------------------------------------------------------
// ComplexType parsing
// ----------------------------------------------------------------

/// @brief
/// Parse xs:complexType node into a ComplexType struct.

void XSD_Impl::parseComplexType(const Node &ctNode, XSD_ComplexType &ct)
{
  ct.name = attrValue(ctNode, "name");
  const auto mixedStr = attrValue(ctNode, "mixed");
  ct.mixed = (mixedStr == "true" || mixedStr == "1");

  for (const auto &child : childElementViews(ctNode)) {
    const auto &childNode = child.node.get();
    const auto tag = child.tag;
    if (tag == "sequence") {
      ct.compositor = XSD_ComplexType::Compositor::sequence;
      parseChildParticleList(childNode, ct);
    } else if (tag == "choice") {
      ct.compositor = XSD_ComplexType::Compositor::choice;
      parseChildParticleList(childNode, ct);
    } else if (tag == "all") {
      ct.compositor = XSD_ComplexType::Compositor::all;
      parseChildParticleList(childNode, ct);
    } else if (tag == "attribute") {
      XSD_AttributeDecl attr;
      parseAttributeDecl(childNode, attr);
      ct.attributes.push_back(std::move(attr));
    } else if (tag == "anyAttribute") {
      ct.hasAnyAttribute = true;
    } else if (tag == "simpleContent" || tag == "complexContent") {
      for (const auto &contentChild : childElementViews(childNode)) {
        const auto &contentChildNode = contentChild.node.get();
        const auto contentTag = contentChild.tag;
        if (contentTag == "extension" || contentTag == "restriction") {
          ct.baseType = resolveType(attrValue(contentChildNode, "base"));
          for (const auto &contentGrandChild : childElementViews(contentChildNode)) {
            const auto &grandChildNode = contentGrandChild.node.get();
            const auto grandTag = contentGrandChild.tag;
            if (grandTag == "sequence" || grandTag == "choice" || grandTag == "all") {
              if (grandTag == "sequence") {
                ct.compositor = XSD_ComplexType::Compositor::sequence;
              } else if (grandTag == "choice") {
                ct.compositor = XSD_ComplexType::Compositor::choice;
              } else if (grandTag == "all") {
                ct.compositor = XSD_ComplexType::Compositor::all;
              }
              parseChildParticleList(grandChildNode, ct);
            } else if (grandTag == "attribute") {
              XSD_AttributeDecl attr;
              parseAttributeDecl(grandChildNode, attr);
              ct.attributes.push_back(std::move(attr));
            } else if (grandTag == "anyAttribute") {
              ct.hasAnyAttribute = true;
            }
          }
        }
      }
    }
  }
}

// ----------------------------------------------------------------
// Top-level element declaration parsing
// ----------------------------------------------------------------

/// @brief
/// Parse a top-level xs:element declaration.

void XSD_Impl::parseTopLevelElement(const Node &elemNode)
{
  XSD_ElementDecl decl;
  decl.name = std::string(attrValue(elemNode, "name"));
  decl.typeRef = resolveType(attrValue(elemNode, "type"));
  decl.nillable = (attrValue(elemNode, "nillable") == "true");
  parseOccurrenceBounds(elemNode, decl.minOccurs, decl.maxOccurs);
  decl.fixedValue = attrValue(elemNode, "fixed");
  decl.defaultValue = attrValue(elemNode, "default");

  // Inline type definitions
  for (const auto &childRef : childElements(elemNode)) {
    const auto &child = childRef.get();
    const auto tag = localTag(child);
    if (tag == "complexType") {
      // Inline anonymous complex type — name it after the element
      XSD_ComplexType ct;
      parseComplexType(child, ct);
      ct.name = decl.name + "#inline";
      decl.typeRef = ct.name;
      complexTypes.emplace(ct.name, std::move(ct));
    } else if (tag == "simpleType") {
      XSD_SimpleType st;
      parseSimpleType(child, st);
      st.name = decl.name + "#inline";
      decl.typeRef = st.name;
      simpleTypes.emplace(st.name, std::move(st));
    } else if (tag == "key" || tag == "unique" || tag == "keyref") {
      parseIdentityConstraint(child, decl);
    }
  }

  rootElements.push_back(std::move(decl));
}

// ----------------------------------------------------------------
// Schema root parsing
// ----------------------------------------------------------------

/// @brief
/// Walk the xs:schema element and dispatch to type/element parsers.

void XSD_Impl::parseSchema(const Node &schemaNode)
{
  const auto tag = localTag(schemaNode);
  if (tag != "schema") {
    XML_LIB_THROW(IValidator::Error("XSD root element must be 'xs:schema', found: '" + NRef<Element>(schemaNode).name() + "'."));
  }

  targetNamespace = attrValue(schemaNode, "targetNamespace");
  elementFormDefault = attrValue(schemaNode, "elementFormDefault");
  if (elementFormDefault.empty()) { elementFormDefault = "unqualified"; }

  // Detect xs prefix from xmlns declarations
  const auto &elem = NRef<Element>(schemaNode);
  for (const auto &ns : elem.getNameSpaces()) {
    if (ns.getParsed() == "http://www.w3.org/2001/XMLSchema") {
      // Key is the prefix (or ":" for default)
      const auto &key = ns.getName();
      if (key != ":") { xsPrefix = key; }
    }
  }

  for (const auto &childRef : childElements(schemaNode)) {
    const auto &child = childRef.get();
    const auto childTag = localTag(child);
    if (childTag == "element") {
      parseTopLevelElement(child);
    } else if (childTag == "complexType") {
      XSD_ComplexType ct;
      parseComplexType(child, ct);
      if (!ct.name.empty()) { complexTypes.emplace(ct.name, std::move(ct)); }
    } else if (childTag == "simpleType") {
      XSD_SimpleType st;
      parseSimpleType(child, st);
      if (!st.name.empty()) { simpleTypes.emplace(st.name, std::move(st)); }
    } else if (childTag == "include" || childTag == "import") {
      parseExternalSchema(child);
    }
  }

  resolveDerivedTypes();

  // Validate all typeRefs can be resolved
  for (const auto &decl : rootElements) {
    if (!decl.typeRef.empty()) {
      const bool found =
        complexTypes.contains(decl.typeRef) || simpleTypes.contains(decl.typeRef) || isBuiltinType(decl.typeRef);
      if (!found) {
        XML_LIB_THROW(IValidator::Error("Unresolvable type '" + decl.typeRef + "' referenced by element '" + decl.name + "'."));
      }
    }
  }
}

// ----------------------------------------------------------------
// Lookup helpers
// ----------------------------------------------------------------

const XSD_ComplexType *XSD_Impl::findComplexType(const std::string &name) const
{
  const auto it = complexTypes.find(name);
  return it != complexTypes.end() ? &it->second : nullptr;
}

const XSD_SimpleType *XSD_Impl::findSimpleType(const std::string &name) const
{
  const auto it = simpleTypes.find(name);
  return it != simpleTypes.end() ? &it->second : nullptr;
}

void XSD_Impl::resolveDerivedTypes()
{
  std::unordered_set<std::string> resolutionStack;
  for (auto &pair : complexTypes) {
    resolveBaseTypes(pair.second, resolutionStack);
  }
}

void XSD_Impl::resolveBaseTypes(XSD_ComplexType &ct, std::unordered_set<std::string> &resolutionStack)
{
  if (ct.baseType.empty() || ct.baseTypeResolved) { return; }
  if (resolutionStack.contains(ct.name)) {
    XML_LIB_THROW(IValidator::Error("Circular complexType derivation detected for '" + ct.name + "'."));
  }
  resolutionStack.insert(ct.name);

  const auto baseName = ct.baseType;
  const auto baseIt = complexTypes.find(baseName);
  if (baseIt != complexTypes.end()) {
    resolveBaseTypes(baseIt->second, resolutionStack);
    if (!ct.baseTypeResolved) {
      if (ct.compositor == XSD_ComplexType::Compositor::none && baseIt->second.compositor != XSD_ComplexType::Compositor::none) {
        ct.compositor = baseIt->second.compositor;
      }
      if (!baseIt->second.particles.empty()) {
        ct.particles.insert(ct.particles.begin(), baseIt->second.particles.begin(), baseIt->second.particles.end());
      }
      if (!baseIt->second.attributes.empty()) {
        ct.attributes.insert(ct.attributes.begin(), baseIt->second.attributes.begin(), baseIt->second.attributes.end());
      }
      ct.hasAnyAttribute = ct.hasAnyAttribute || baseIt->second.hasAnyAttribute;
      ct.mixed = ct.mixed || baseIt->second.mixed;
      ct.baseTypeResolved = true;
    }
  } else if (findSimpleType(baseName) || isBuiltinType(baseName)) {
    ct.baseTypeResolved = true;
  } else {
    XML_LIB_THROW(IValidator::Error("Unresolvable base type '" + baseName + "' referenced by complexType '" + ct.name + "'."));
  }

  resolutionStack.erase(ct.name);
}

const XSD_ElementDecl *XSD_Impl::findTopLevelElement(const std::string &name) const
{
  for (const auto &decl : rootElements) {
    if (decl.name == name) { return &decl; }
  }
  return nullptr;
}

/// @brief
/// Implementation of XSD_Impl::isBuiltinType.

bool XSD_Impl::isBuiltinType(const std::string_view &typeName) const
{
  static constexpr std::array<std::string_view, 34> builtins{ "xs:ID",
    "xs:IDREF",
    "xs:Name",
    "xs:NCName",
    "xs:NMTOKEN",
    "xs:QName",
    "xs:anySimpleType",
    "xs:anyURI",
    "xs:base64Binary",
    "xs:boolean",
    "xs:byte",
    "xs:date",
    "xs:dateTime",
    "xs:decimal",
    "xs:double",
    "xs:float",
    "xs:hexBinary",
    "xs:int",
    "xs:integer",
    "xs:language",
    "xs:long",
    "xs:negativeInteger",
    "xs:nonNegativeInteger",
    "xs:nonPositiveInteger",
    "xs:normalizedString",
    "xs:positiveInteger",
    "xs:short",
    "xs:string",
    "xs:time",
    "xs:token",
    "xs:unsignedByte",
    "xs:unsignedInt",
    "xs:unsignedLong",
    "xs:unsignedShort" };
  return std::find(builtins.begin(), builtins.end(), typeName) != builtins.end();
}

}// namespace XML_Lib