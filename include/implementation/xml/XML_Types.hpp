#pragma once
// =======
// C++ STL
// =======
#include <utility>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <unordered_map>
#include <stdexcept>
// =================
// LIBRARY NAMESPACE
// =================
namespace XML_Lib {
// ===================================================
// Forward declarations for interfaces/classes/structs
// ===================================================
class IEntityMapper;
// =========
// XML Error
// =========
struct Error : public std::runtime_error
{
  Error(std::string const &message) : std::runtime_error("XML Error: " + message) {}
};
// ================
// XML Syntax Error
// ================
struct SyntaxError : public std::runtime_error
{
  SyntaxError(const std::string &message) : std::runtime_error("XML Syntax Error: " + message) {}
  explicit SyntaxError(const std::pair<long, long> &position, const std::string &message = "")
    : std::runtime_error("XML Syntax Error [Line: " + std::to_string(position.first)
                         + " Column: " + std::to_string(position.second) + "] " + message)
  {}
};
// ====================
// XML Validation Error
// ====================
struct ValidationError : public std::runtime_error
{
  explicit ValidationError(long lineNumber, const std::string &message = "")
    : std::runtime_error("XML Validation Error [Line: " + std::to_string(lineNumber) + "] " + message)
  {}
};
// =========
// XML value
// =========
struct XMLValue
{
  explicit XMLValue(std::string unparsed) : unparsed(std::move(unparsed)) {}
  XMLValue(std::string unparsed, std::string parsed) : unparsed(std::move(unparsed)), parsed(std::move(parsed)) {}
  [[nodiscard]] bool isReference() const { return (unparsed[0] == '&' && unparsed.back() == ';'); }
  [[nodiscard]] bool isEntityReference() const
  {
    return (unparsed[0] == '&' && unparsed[1] != '#' && unparsed.back() == ';');
  }
  [[nodiscard]] bool isCharacterReference() const
  {
    return (unparsed[0] == '&' && unparsed[1] == '#' && unparsed.back() == ';');
  }
  std::string unparsed{};
  std::string parsed{};
};
// =============
// XML Attribute
// =============
struct XMLAttribute
{
  using List = std::vector<XMLAttribute>;
  XMLAttribute(std::string name, XMLValue value) : name(std::move(name)), value(std::move(value)) {}
  std::string name;
  XMLValue value;
};
// ======================
// XML External reference
// ======================
struct XMLExternalReference
{
  explicit XMLExternalReference(std::string type) : type(std::move(type)) {}
  XMLExternalReference(std::string type, std::string systemID, std::string publicID)
    : type(std::move(type)), systemID(std::move(systemID)), publicID(std::move(publicID))
  {}
  std::string type{};
  std::string systemID{};
  std::string publicID{};
};
// ==================
// XML Entity mapping
// ==================
struct XMLEntityMapping
{
  explicit XMLEntityMapping(std::string internal) : internal(std::move(internal)) {}
  XMLEntityMapping(std::string internal, XMLExternalReference external, std::string notation)
    : internal(std::move(internal)), external(std::move(external)), notation(std::move(notation))
  {}
  std::string internal{};
  XMLExternalReference external{ "" };
  std::string notation{};
};
// Base
// ====
struct XNode
{
  enum class Type { base = 0, prolog, declaration, root, self, element, content, entity, comment, cdata, pi, dtd };
  struct Error : public std::runtime_error
  {
    Error(const std::string &message) : std::runtime_error("XMLNode Error: " + message) {}
  };
  explicit XNode(Type nodeType = Type::base) : xNodeType(nodeType) {}
  [[nodiscard]] Type getNodeType() const { return (xNodeType); }
  void setNodeType(Type nodeType) { xNodeType = nodeType; }
  [[nodiscard]] std::string getContents() const;
  XNode &operator[](int index) const;
  XNode &operator[](const std::string &name) const;
  std::vector<std::unique_ptr<XNode>> children;

private:
  Type xNodeType;
};
// ======
// Prolog
// ======
struct XNodeProlog : XNode
{
  explicit XNodeProlog(XNode::Type nodeType = XNode::Type::prolog) : XNode(nodeType) {}
};
// ===========
// Declaration
// ===========
struct XNodeDeclaration : XNode
{
  explicit XNodeDeclaration(XNode::Type nodeType = XNode::Type::declaration) : XNode(nodeType) {}
  [[nodiscard]] bool isValidVersion() const { return (m_version == "1.0" || m_version == "1.1"); }
  [[nodiscard]] bool isValidEncoding() const { return (m_encoding == "UTF-8" || m_encoding == "UTF-16"); }
  [[nodiscard]] bool isValidStandalone() const { return (m_standalone == "yes" || m_standalone == "no"); }
  [[nodiscard]] std::string version() const { return (m_version); }
  void setVersion(const std::string &version) { m_version = version; }
  [[nodiscard]] std::string encoding() const { return (m_encoding); }
  void setEncoding(const std::string &encoding) { m_encoding = encoding; }
  [[nodiscard]] std::string standalone() const { return (m_standalone); }
  void setStandalone(const std::string &standalone) { m_standalone = standalone; }

private:
  std::string m_version{ "1.0" };
  std::string m_encoding{ "UTF-8" };
  std::string m_standalone{ "no" };
};
// =======
// Content
// =======
struct XNodeContent : XNode
{
  explicit XNodeContent(bool isWhiteSpace = true, XNode::Type nodeType = XNode::Type::content)
    : XNode(nodeType), m_isWhiteSpace(isWhiteSpace)
  {}
  [[nodiscard]] std::string content() const { return (m_content); }
  void addContent(const std::string &content) { m_content += content; }
  [[nodiscard]] bool isWhiteSpace() const { return (m_isWhiteSpace); }
  void setIsWhiteSpace(bool isWhiteSpace) { m_isWhiteSpace = isWhiteSpace; }

private:
  std::string m_content;
  bool m_isWhiteSpace;
};
// =====
// CDATA
// =====
struct XNodeCDATA : XNode
{
  explicit XNodeCDATA(XNode::Type nodeType = XNode::Type::cdata) : XNode(nodeType) {}
  [[nodiscard]] std::string CDATA() const { return (m_cdata); }
  void setCDATA(const std::string &cdata) { m_cdata = cdata; }

private:
  std::string m_cdata;
};
// ===============
// EntityReference
// ===============
struct XNodeEntityReference : XNode
{
  explicit XNodeEntityReference(XMLValue value, XNode::Type nodeType = XNode::Type::entity)
    : XNode(nodeType), m_value(std::move(value))
  {}
  [[nodiscard]] XMLValue value() const { return (m_value); }
  void setValue(const XMLValue &value) { m_value = value; }

private:
  XMLValue m_value;
};
// =======
// Element
// =======
struct XNodeElement : XNode
{
  explicit XNodeElement(XNode::Type nodeType = XNode::Type::element) : XNode(nodeType) {}
  explicit XNodeElement(const std::string &name) : XNodeElement() { m_name = name; }
  [[nodiscard]] bool isAttributePresent(const std::string &name) const
  {
    return (std::find_if(m_attributes.rbegin(), m_attributes.rend(), [&name](const XMLAttribute &attr) {
      return (attr.name == name);
    }) != m_attributes.rend());
  }
  void addAttribute(const std::string &name, const XMLValue &value) const { m_attributes.emplace_back(name, value); }
  [[nodiscard]] XMLAttribute getAttribute(const std::string &name) const
  {
    return (*std::find_if(
      m_attributes.rbegin(), m_attributes.rend(), [&name](const XMLAttribute &attr) { return (attr.name == name); }));
  }
  [[nodiscard]] const XMLAttribute::List &getAttributeList() const { return (m_attributes); }
  [[nodiscard]] bool isNameSpacePresent(const std::string &name) const
  {
    return (std::find_if(m_namespaces.rbegin(), m_namespaces.rend(), [&name](const XMLAttribute &attr) {
      return (attr.name == name);
    }) != m_namespaces.rend());
  }
  void addNameSpace(const std::string &name, const XMLValue &value) { m_namespaces.emplace_back(name, value); }
  [[nodiscard]] XMLAttribute getNameSpace(const std::string &name) const
  {
    return (*std::find_if(
      m_namespaces.rbegin(), m_namespaces.rend(), [&name](const XMLAttribute &ns) { return (ns.name == name); }));
  }
  [[nodiscard]] const XMLAttribute::List &getNameSpaceList() const { return (m_namespaces); }
  [[nodiscard]] std::string name() const { return (m_name); }
  void setName(const std::string &name) { m_name = name; }
  XNodeElement &operator[](int index) const;
  XNodeElement &operator[](const std::string &name) const;

private:
  std::string m_name;
  XMLAttribute::List m_namespaces;
  mutable XMLAttribute::List m_attributes;
};
// =======
// Comment
// =======
struct XNodeComment : XNode
{
  explicit XNodeComment(std::string comment = "", XNode::Type nodeType = XNode::Type::comment)
    : XNode(nodeType), m_comment(std::move(comment))
  {}
  [[nodiscard]] std::string comment() const { return (m_comment); }
  void setComment(const std::string &comment) { m_comment = comment; }

private:
  std::string m_comment;
};
// ==
// PI
// ==
struct XNodePI : XNode
{
  explicit XNodePI(XNode::Type nodeType = XNode::Type::pi) : XNode(nodeType) {}
  [[nodiscard]] std::string name() const { return (m_name); }
  void setName(const std::string &name) { m_name = name; }
  [[nodiscard]] std::string parameters() const { return (m_parameters); }
  void setParameters(const std::string &parameters) { m_parameters = parameters; }

private:
  std::string m_name;
  std::string m_parameters;
};
// ===
// DTD
// ===
struct XNodeDTD : XNode
{
  //
  // DTD Type
  //
  enum Type : uint8_t { internal = (0x1 << 0), external = (0x1 << 1) };
  //
  // XML DTD attribute types. Note only one type bit and
  // value bit should be set for each attribute and that
  // some combinations like id and fixed are illegal and
  // are reported as so by the parser.
  //
  enum AttributeType : uint16_t {
    // Types bits 0 - 9
    cdata = (0x1 << 0),
    enumeration = (0x1 << 1),
    id = (0x1 << 2),
    idref = (0x1 << 3),
    idrefs = (0x1 << 4),
    nmtoken = (0x1 << 5),
    nmtokens = (0x1 << 6),
    entity = (0x1 << 7),
    entities = (0x1 << 8),
    notation = (0x1 << 9),
    // Value bits 10 - 13
    normal = (0x1 << 10),
    required = (0x1 << 11),
    implied = (0x1 << 12),
    fixed = (0x1 << 13)
  };
  //
  // XML DTD attribute definition
  //
  struct Attribute
  {
    std::string name;
    uint16_t type;
    std::string enumeration;
    XMLValue value{ "", "" };
  };
  //
  // XML DTD element definition
  //
  struct Element
  {
    Element() = default;
    Element(std::string name, XMLValue content) : name(std::move(name)), content(std::move(content)) {}
    std::string name;
    bool idAttributePresent = false;
    XMLValue content{ "", "" };
    std::vector<Attribute> attributes;
  };
  explicit XNodeDTD(IEntityMapper &entityMapper, XNode::Type nodeType = XNode::Type::dtd)
    : XNode(nodeType), m_entityMapper(entityMapper)
  {}
  [[nodiscard]] std::string unparsed() const { return (m_unparsed); }
  void setUnparsed(const std::string &unparsed) { m_unparsed = unparsed; }
  [[nodiscard]] uint16_t getType() const { return (m_type); }
  void setType(uint16_t type) { m_type = type; }
  [[nodiscard]] std::string getRootName() const { return (m_name); }
  void setRootName(const std::string &name) { m_name = name; }
  [[nodiscard]] XMLExternalReference getExternalReference() const { return (m_external); }
  void setExternalReference(const XMLExternalReference &reference) { m_external = reference; }
  [[nodiscard]] bool isElementPresent(const std::string &elementName) const
  {
    return (m_elements.find(elementName) != m_elements.end());
  }
  XNodeDTD::Element &getElement(const std::string &elementName)
  {
    auto element = m_elements.find(elementName);
    if (element != m_elements.end()) { return (element->second); }
    throw XNode::Error("Could not find notation name.");
  }
  void addElement(const std::string &elementName, const XNodeDTD::Element &element)
  {
    m_elements.emplace(elementName, element);
  }
  [[nodiscard]] long getElementCount() const { return (static_cast<long>(m_elements.size())); }
  XMLExternalReference &getNotation(const std::string &notationName)
  {
    auto notation = m_notations.find(notationName);
    if (notation != m_notations.end()) { return (notation->second); }
    throw XNode::Error("Could not find notation name.");
  }
  void addNotation(const std::string &notationName, const XMLExternalReference &notation)
  {
    m_notations.emplace(notationName, notation);
  }
  [[nodiscard]] long getNotationCount(const std::string &notationName) const
  {
    return (static_cast<long>(m_notations.count(notationName)));
  }
  [[nodiscard]] long getLineCount() const { return (m_lineCount); }
  void setLineCount(long lineCount) { m_lineCount = lineCount; }
  IEntityMapper &m_entityMapper;

private:
  uint16_t m_type{};
  long m_lineCount{};
  std::string m_name;
  XMLExternalReference m_external{ "" };
  std::unordered_map<std::string, XNodeDTD::Element> m_elements;
  std::unordered_map<std::string, XMLExternalReference> m_notations;
  std::string m_unparsed;
};
// ===========================
// XMLNode reference converter
// ===========================
template<typename T> void CheckXNodeType(const XNode &xNode)
{
  if constexpr (std::is_same_v<T, XNodeProlog>) {
    if (xNode.getNodeType() != XNode::Type::prolog) { throw XNode::Error("Node not a prolog."); }
  } else if constexpr (std::is_same_v<T, XNodeDeclaration>) {
    if (xNode.getNodeType() != XNode::Type::declaration) { throw XNode::Error("Node not a declaration."); }
  } else if constexpr (std::is_same_v<T, XNodeElement>) {
    if ((xNode.getNodeType() != XNode::Type::root) && (xNode.getNodeType() != XNode::Type::self)
        && (xNode.getNodeType() != XNode::Type::element)) {
      throw XNode::Error("Node not a element.");
    }
  } else if constexpr (std::is_same_v<T, XNodeContent>) {
    if (xNode.getNodeType() != XNode::Type::content) { throw XNode::Error("Node not content."); }
  } else if constexpr (std::is_same_v<T, XNodeEntityReference>) {
    if (xNode.getNodeType() != XNode::Type::entity) { throw XNode::Error("Node not an entity."); }
  } else if constexpr (std::is_same_v<T, XNodeComment>) {
    if (xNode.getNodeType() != XNode::Type::comment) { throw XNode::Error("Node not a comment."); }
  } else if constexpr (std::is_same_v<T, XNodeCDATA>) {
    if (xNode.getNodeType() != XNode::Type::cdata) { throw XNode::Error("Node not CDATA."); }
  } else if constexpr (std::is_same_v<T, XNodePI>) {
    if (xNode.getNodeType() != XNode::Type::pi) { throw XNode::Error("Node not a PI."); }
  } else if constexpr (std::is_same_v<T, XNodeDTD>) {
    if (xNode.getNodeType() != XNode::Type::dtd) { throw XNode::Error("Node not DTD."); }
  }
}
template<typename T> T &XNodeRef(XNode &xNode)
{
  CheckXNodeType<T>(xNode);
  return (static_cast<T &>(xNode));
}
template<typename T> const T &XMLNodeRef(const XNode &xNode)
{
  CheckXNodeType<T>(xNode);
  return (static_cast<const T &>(xNode));
}
// ====================
// XMLNode index access
// ====================
inline XNode &XNode::operator[](int index) const// Array
{
  if ((index >= 0) && (index < (static_cast<int>(XMLNodeRef<XNode>(*this).children.size())))) {
    return (*((XMLNodeRef<XNode>(*this).children[index])));
  }
  throw XNode::Error("Invalid index used to access array.");
}
// ===================
// XMLNode name access
// ===================
inline XNode &XNode::operator[](const std::string &name) const// Array
{
  if (xNodeType <= XNode::Type::element) {
    for (const auto &element : XMLNodeRef<XNodeElement>(*this).children) {
      if (XNodeRef<XNodeElement>(*element).name() == name) { return (*element); }
    }
  }
  throw XNode::Error("Invalid index used to access array.");
}
// ===========================
// XMLNodeElement index access
// ===========================
inline XNodeElement &XNodeElement::operator[](int index) const// Array
{
  int number = 0;
  if ((index >= 0) && (index < (static_cast<int>(XMLNodeRef<XNodeElement>(*this).children.size())))) {
    for (const auto &child : XMLNodeRef<XNode>(*this).children) {
      if (XNodeRef<XNode>(*child).getNodeType() <= XNode::Type::element) {
        if (number == index) { return (XNodeRef<XNodeElement>(*child)); }
        number++;
      }
    }
  }
  throw XNode::Error("Invalid index used to access array.");
}
// ==========================
// XMLNodeElement name access
// ==========================
inline XNodeElement &XNodeElement::operator[](const std::string &name) const// Array
{
  if (getNodeType() <= XNode::Type::element) {
    for (const auto &element : XMLNodeRef<XNodeElement>(*this).children) {
      if (XNodeRef<XNodeElement>(*element).m_name == name) { return (XNodeRef<XNodeElement>(*element)); }
    }
  }
  throw XNode::Error("Invalid index used to access array.");
}
// ====================
// XMLNode get contents
// ====================
inline std::string XNode::getContents() const
{
  std::string result;
  for (const auto &node : children) {
    if (node->getNodeType() == XNode::Type::content) {
      result += XNodeRef<XNodeContent>(*node).content();
    } else if (node->getNodeType() == XNode::Type::entity) {
      if (!XNodeRef<XNodeEntityReference>(*node).children.empty()) {
        result += XNodeRef<XNodeEntityReference>(*node).getContents();
      } else {
        result += XNodeRef<XNodeEntityReference>(*node).value().parsed;
      }
    } else if (node->getNodeType() == XNode::Type::cdata) {
      result += XNodeRef<XNodeCDATA>(*node).CDATA();
    }
  }
  return (result);
}
}// namespace XML_Lib
