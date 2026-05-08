//
// Class: XSD_Impl_Parse
//
// Description: Parse an XSD schema (which is itself XML) into the data model.
//
// Dependencies: C++20 - Language standard features used.
//

#include "XSD_Impl.hpp"
#include <algorithm>
#include <charconv>
#include <unordered_map>

namespace XML_Lib {

namespace {

uint32_t parseUint(const std::string_view &value, uint32_t defaultValue = 1u)
{
  if (value.empty()) { return defaultValue; }
  uint32_t result = 0;
  const auto [ptr, ec] = std::from_chars(value.data(), value.data() + value.size(), result);
  return (ec == std::errc()) ? result : defaultValue;
}

} // namespace

// ----------------------------------------------------------------
// Utility helpers
// ----------------------------------------------------------------

/// <summary>
/// Return the local tag name of an XML element node (strips any namespace prefix).
/// e.g. "xs:element" -> "element", "element" -> "element"
/// </summary>
std::string_view XSD_Impl::localTag(const Node &node)
{
  const auto &name = NRef<Element>(node).name();
  const auto pos = name.find(':');
  const auto view = std::string_view(name);
  return pos != std::string::npos ? view.substr(pos + 1) : view;
}

/// <summary>
/// Return the value of a named attribute on a node, or empty string view if absent.
/// </summary>
std::string_view XSD_Impl::attrValue(const Node &node, const std::string_view &attrName)
{
  const auto &elem = NRef<Element>(node);
  if (elem.hasAttribute(attrName)) { return elem[attrName].getParsed(); }
  return std::string_view{};
}

/// <summary>
/// Normalise a type reference, removing a known xs: prefix so all stored
/// type names use the "xs:xxx" canonical form regardless of the prefix used
/// in the schema document (xsd:, xs:, etc.)
/// </summary>
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

/// <summary>
/// Return a list of child Nodes that are XML element nodes
/// (skips Content, Comment, PI etc.).
/// </summary>
std::vector<std::reference_wrapper<const Node>> XSD_Impl::childElements(const Node &node)
{
  const auto &children = node.getChildren();
  size_t count = 0;
  for (const auto &child : children) {
    if (isA<Element>(child) || isA<Root>(child) || isA<Self>(child)) { ++count; }
  }

  std::vector<std::reference_wrapper<const Node>> result;
  result.reserve(count);
  for (const auto &child : children) {
    if (isA<Element>(child) || isA<Root>(child) || isA<Self>(child)) { result.emplace_back(child); }
  }
  return result;
}

// ----------------------------------------------------------------
// Restriction parsing
// ----------------------------------------------------------------

/// <summary>
/// Parse xs:restriction and its facet children into a SimpleType.
/// </summary>
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

/// <summary>
/// Parse xs:simpleType node into a SimpleType struct.
/// </summary>
void XSD_Impl::parseSimpleType(const Node &stNode, XSD_SimpleType &st)
{
  st.name = attrValue(stNode, "name");
  for (const auto &childRef : childElements(stNode)) {
    const auto &child = childRef.get();
    if (localTag(child) == "restriction") { parseRestriction(child, st); }
  }
}

// ----------------------------------------------------------------
// Attribute declaration parsing
// ----------------------------------------------------------------

/// <summary>
/// Parse xs:attribute node into an AttributeDecl.
/// </summary>
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

// ----------------------------------------------------------------
// Particle (element child in content model) parsing
// ----------------------------------------------------------------

/// <summary>
/// Parse a single xs:element particle node.
/// </summary>
void XSD_Impl::parseParticle(const Node &particleNode, XSD_Particle &particle)
{
  particle.elementName = std::string(attrValue(particleNode, "name"));
  const auto minStr = attrValue(particleNode, "minOccurs");
  const auto maxStr = attrValue(particleNode, "maxOccurs");
  particle.minOccurs = parseUint(minStr, 1u);
  particle.maxOccurs = maxStr == "unbounded" ? 0u : maxStr.empty() ? 1u : parseUint(maxStr, 1u);
  particle.typeRef = resolveType(attrValue(particleNode, "type"));

  // Check for inline type declarations
  for (const auto &childRef : childElements(particleNode)) {
    const auto &child = childRef.get();
    const auto tag = localTag(child);
    if (tag == "complexType") {
      particle.inlineComplexType = std::make_unique<XSD_ComplexType>();
      parseComplexType(child, *particle.inlineComplexType);
    } else if (tag == "simpleType") {
      particle.inlineSimpleType = std::make_unique<XSD_SimpleType>();
      parseSimpleType(child, *particle.inlineSimpleType);
    }
  }
}

// ----------------------------------------------------------------
// ComplexType parsing
// ----------------------------------------------------------------

/// <summary>
/// Parse xs:complexType node into a ComplexType struct.
/// </summary>
void XSD_Impl::parseComplexType(const Node &ctNode, XSD_ComplexType &ct)
{
  ct.name = attrValue(ctNode, "name");
  const auto mixedStr = attrValue(ctNode, "mixed");
  ct.mixed = (mixedStr == "true" || mixedStr == "1");

  for (const auto &childRef : childElements(ctNode)) {
    const auto &child = childRef.get();
    const auto tag = localTag(child);
    if (tag == "sequence") {
      ct.compositor = XSD_ComplexType::Compositor::sequence;
      for (const auto &particleChildRef : childElements(child)) {
        const auto &particleChild = particleChildRef.get();
        if (localTag(particleChild) == "element") {
          XSD_Particle particle;
          parseParticle(particleChild, particle);
          ct.particles.push_back(std::move(particle));
        }
      }
    } else if (tag == "choice") {
      ct.compositor = XSD_ComplexType::Compositor::choice;
      for (const auto &particleChildRef : childElements(child)) {
        const auto &particleChild = particleChildRef.get();
        if (localTag(particleChild) == "element") {
          XSD_Particle particle;
          parseParticle(particleChild, particle);
          ct.particles.push_back(std::move(particle));
        }
      }
    } else if (tag == "all") {
      ct.compositor = XSD_ComplexType::Compositor::all;
      for (const auto &particleChildRef : childElements(child)) {
        const auto &particleChild = particleChildRef.get();
        if (localTag(particleChild) == "element") {
          XSD_Particle particle;
          parseParticle(particleChild, particle);
          ct.particles.push_back(std::move(particle));
        }
      }
    } else if (tag == "attribute") {
      XSD_AttributeDecl attr;
      parseAttributeDecl(child, attr);
      ct.attributes.push_back(std::move(attr));
    } else if (tag == "anyAttribute") {
      ct.hasAnyAttribute = true;
    } else if (tag == "simpleContent" || tag == "complexContent") {
      // Handle extension/restriction of content
      for (const auto &contentChildRef : childElements(child)) {
        const auto &contentChild = contentChildRef.get();
        const auto contentTag = localTag(contentChild);
        if (contentTag == "extension" || contentTag == "restriction") {
          // Parse attributes declared inside extension/restriction
          for (const auto &extChildRef : childElements(contentChild)) {
            const auto &extChild = extChildRef.get();
            if (localTag(extChild) == "attribute") {
              XSD_AttributeDecl attr;
              parseAttributeDecl(extChild, attr);
              ct.attributes.push_back(std::move(attr));
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

/// <summary>
/// Parse a top-level xs:element declaration.
/// </summary>
void XSD_Impl::parseTopLevelElement(const Node &elemNode)
{
  XSD_ElementDecl decl;
  decl.name = std::string(attrValue(elemNode, "name"));
  decl.typeRef = resolveType(attrValue(elemNode, "type"));
  const auto minStr = attrValue(elemNode, "minOccurs");
  const auto maxStr = attrValue(elemNode, "maxOccurs");
  decl.minOccurs = parseUint(minStr, 1u);
  decl.maxOccurs = maxStr == "unbounded" ? 0u : maxStr.empty() ? 1u : parseUint(maxStr, 1u);
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
    }
  }

  rootElements.push_back(std::move(decl));
}

// ----------------------------------------------------------------
// Schema root parsing
// ----------------------------------------------------------------

/// <summary>
/// Walk the xs:schema element and dispatch to type/element parsers.
/// </summary>
void XSD_Impl::parseSchema(const Node &schemaNode)
{
  const auto tag = localTag(schemaNode);
  if (tag != "schema") {
    throw IValidator::Error("XSD root element must be 'xs:schema', found: '" + NRef<Element>(schemaNode).name() + "'.");
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
    }
    // xs:include / xs:import: skipped in Phase 1
  }

  // Validate all typeRefs can be resolved
  for (const auto &decl : rootElements) {
    if (!decl.typeRef.empty()) {
      const bool found =
        complexTypes.contains(decl.typeRef) || simpleTypes.contains(decl.typeRef) || isBuiltinType(decl.typeRef);
      if (!found) {
        throw IValidator::Error("Unresolvable type '" + decl.typeRef + "' referenced by element '" + decl.name + "'.");
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

const XSD_ElementDecl *XSD_Impl::findTopLevelElement(const std::string &name) const
{
  for (const auto &decl : rootElements) {
    if (decl.name == name) { return &decl; }
  }
  return nullptr;
}

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
