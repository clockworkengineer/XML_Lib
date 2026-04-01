#pragma once

#include "XML.hpp"
#include "XML_Core.hpp"
#include "XSD_Validator.hpp"

namespace XML_Lib {

// ============================================================
// XSD data model types
// ============================================================

struct XSD_Restriction
{
  enum class Facet : uint8_t {
    minLength,
    maxLength,
    length,
    pattern,
    enumeration,
    minInclusive,
    maxInclusive,
    minExclusive,
    maxExclusive,
    totalDigits,
    fractionDigits,
    whiteSpace
  };
  Facet facet;
  std::string value;
};

struct XSD_SimpleType
{
  std::string name;
  std::string baseType;// e.g. "xs:string", "xs:integer"
  std::vector<XSD_Restriction> restrictions;
};

struct XSD_AttributeDecl
{
  enum class Use : uint8_t { optional, required, prohibited };
  std::string name;
  std::string typeRef;// resolved type name
  Use use{ Use::optional };
  std::string defaultValue;
  std::string fixedValue;
};

struct XSD_Particle;// forward

struct XSD_ComplexType
{
  enum class Compositor : uint8_t { sequence, choice, all, none };
  std::string name;
  Compositor compositor{ Compositor::none };
  std::vector<XSD_Particle> particles;
  std::vector<XSD_AttributeDecl> attributes;
  bool mixed{ false };
  bool hasAnyAttribute{ false };
};

struct XSD_Particle
{
  std::string elementName;
  uint32_t minOccurs{ 1 };
  uint32_t maxOccurs{ 1 };// 0 = unbounded
  std::string typeRef;
  // Inline complex type (anonymous)
  std::unique_ptr<XSD_ComplexType> inlineComplexType;
  // Inline simple type (anonymous)
  std::unique_ptr<XSD_SimpleType> inlineSimpleType;
};

struct XSD_ElementDecl
{
  std::string name;
  std::string typeRef;
  uint32_t minOccurs{ 1 };
  uint32_t maxOccurs{ 1 };// 0 = unbounded
  std::string fixedValue;
  std::string defaultValue;
};

// ============================================================
// XSD_Impl — internal implementation
// ============================================================
class XSD_Impl
{
public:
  // Constructors/Destructors
  explicit XSD_Impl(Node &xNode);
  XSD_Impl() = delete;
  XSD_Impl(const XSD_Impl &other) = delete;
  XSD_Impl &operator=(const XSD_Impl &other) = delete;
  XSD_Impl(XSD_Impl &&other) = delete;
  XSD_Impl &operator=(XSD_Impl &&other) = delete;
  ~XSD_Impl();

  // Parse XSD schema from source
  void parse(ISource &source);
  // Stringify XSD schema to text destination
  void stringify(IDestination &destination) const;
  // Validate XML tree against schema
  void validate(const Node &xNode);

private:
  // ----------------------------------------------------------------
  // Schema parse helpers
  // ----------------------------------------------------------------
  void parseSchema(const Node &schemaNode);
  void parseTopLevelElement(const Node &elemNode);
  void parseComplexType(const Node &ctNode, XSD_ComplexType &ct);
  void parseSimpleType(const Node &stNode, XSD_SimpleType &st);
  void parseParticle(const Node &particleNode, XSD_Particle &particle);
  void parseAttributeDecl(const Node &attrNode, XSD_AttributeDecl &attr);
  void parseRestriction(const Node &restrictNode, XSD_SimpleType &st);
  // Retrieve attribute value from a schema node, empty string if absent
  [[nodiscard]] static std::string attrValue(const Node &node, const std::string_view &attrName);
  // Resolve a possibly-prefixed xs: type name to a canonical string
  [[nodiscard]] static std::string resolveType(const std::string_view &typeStr);
  // Return child nodes that are elements (skip text/comment nodes)
  [[nodiscard]] static std::vector<std::reference_wrapper<const Node>> childElements(const Node &node);
  // Return local name of schema element tag (strip xs: prefix)
  [[nodiscard]] static std::string localTag(const Node &node);

  // ----------------------------------------------------------------
  // Validation helpers
  // ----------------------------------------------------------------
  void validateElement(const Node &xNode, const XSD_ComplexType &type);
  void validateAttributes(const Node &xNode, const XSD_ComplexType &type);
  void validateSimpleValue(const std::string &value, const std::string &typeRef, const std::string &context);
  [[nodiscard]] bool isBuiltinType(const std::string_view &typeName) const;
  void validateBuiltinType(const std::string &value, const std::string &typeName, const std::string &context);
  void validateRestrictions(const std::string &value, const XSD_SimpleType &st, const std::string &context);
  [[nodiscard]] const XSD_ComplexType *findComplexType(const std::string &name) const;
  [[nodiscard]] const XSD_SimpleType *findSimpleType(const std::string &name) const;
  [[nodiscard]] const XSD_ElementDecl *findTopLevelElement(const std::string &name) const;

  // ----------------------------------------------------------------
  // Schema data
  // ----------------------------------------------------------------
  std::unordered_map<std::string, XSD_SimpleType> simpleTypes;
  std::unordered_map<std::string, XSD_ComplexType> complexTypes;
  std::vector<XSD_ElementDecl> rootElements;
  std::string targetNamespace;
  std::string elementFormDefault{ "unqualified" };
  std::string xsPrefix{ "xs" };// detected from schema namespace declarations

  // Reference to the XML root node (unused for XSD but required by IValidator constructor pattern)
  Node &xmlRoot;
};

}// namespace XML_Lib
