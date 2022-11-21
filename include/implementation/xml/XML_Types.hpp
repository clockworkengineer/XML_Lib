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
// ====
// Base
// ====
struct XNode
{
  // XNode Error
  struct Error : public std::runtime_error
  {
    Error(const std::string &message) : std::runtime_error("XNode Error: " + message) {}
  };
  // XNode Types
  enum class Type { base = 0, prolog, declaration, root, self, element, content, entity, comment, cdata, pi, dtd };
  // Constructors/Destructors
  explicit XNode(Type nodeType = Type::base) : m_type(nodeType) {}
  XNode(const XNode &other) = delete;
  XNode &operator=(XNode &other) = delete;
  XNode(XNode &&other) = default;
  XNode &operator=(XNode &&other) = default;
  ~XNode() = default;
  // Get/Set XNode type
  [[nodiscard]] Type getType() const { return (m_type); }
  void setType(Type nodeType) { m_type = nodeType; }
  // Return XNode contents
  [[nodiscard]] std::string getContents() const;
  // XNode Index overloads
  XNode &operator[](int index) const;
  XNode &operator[](const std::string &name) const;
  // Add child
  void addChild(std::unique_ptr<XNode> &child) { m_children.push_back(std::move(child)); }
  void addChild(std::unique_ptr<XNode> &&child) { m_children.push_back(std::move(child)); }
  // Get XNode Children reference
  std::vector<std::unique_ptr<XNode>> &getChildren() { return (m_children); };
  const std::vector<std::unique_ptr<XNode>> &getChildren() const { return (m_children); };

private:
  // XNode Type
  Type m_type;
  // XNode Children
  std::vector<std::unique_ptr<XNode>> m_children;
};
// ======
// Prolog
// ======
struct XProlog : XNode
{
  // Constructors/Destructors
  explicit XProlog(XNode::Type nodeType = XNode::Type::prolog) : XNode(nodeType) {}
  XProlog(const XProlog &other) = delete;
  XProlog &operator=(XProlog &other) = delete;
  XProlog(XProlog &&other) = default;
  XProlog &operator=(XProlog &&other) = default;
  ~XProlog() = default;
};
// ===========
// Declaration
// ===========
struct XDeclaration : XNode
{
  // Constructors/Destructors
  explicit XDeclaration(XNode::Type nodeType = XNode::Type::declaration) : XNode(nodeType) {}
  XDeclaration(const XDeclaration &other) = delete;
  XDeclaration &operator=(XDeclaration &other) = delete;
  XDeclaration(XDeclaration &&other) = default;
  XDeclaration &operator=(XDeclaration &&other) = default;
  ~XDeclaration() = default;
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
struct XContent : XNode
{
  // Constructors/Destructors
  explicit XContent(bool isWhiteSpace = true, XNode::Type nodeType = XNode::Type::content)
    : XNode(nodeType), m_isWhiteSpace(isWhiteSpace)
  {}
  XContent(const XContent &other) = delete;
  XContent &operator=(XContent &other) = delete;
  XContent(XContent &&other) = default;
  XContent &operator=(XContent &&other) = default;
  ~XContent() = default;
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
struct XCDATA : XNode
{
  // Constructors/Destructors
  explicit XCDATA(XNode::Type nodeType = XNode::Type::cdata) : XNode(nodeType) {}
  XCDATA(const XCDATA &other) = delete;
  XCDATA &operator=(XCDATA &other) = delete;
  XCDATA(XCDATA &&other) = default;
  XCDATA &operator=(XCDATA &&other) = default;
  ~XCDATA() = default;
  [[nodiscard]] std::string CDATA() const { return (m_cdata); }
  void setCDATA(const std::string &cdata) { m_cdata = cdata; }

private:
  std::string m_cdata;
};
// ===============
// EntityReference
// ===============
struct XEntityReference : XNode
{
  // Constructors/Destructors
  explicit XEntityReference(XMLValue value, XNode::Type nodeType = XNode::Type::entity)
    : XNode(nodeType), m_value(std::move(value))
  {}
  XEntityReference(const XEntityReference &other) = delete;
  XEntityReference &operator=(XEntityReference &other) = delete;
  XEntityReference(XEntityReference &&other) = default;
  XEntityReference &operator=(XEntityReference &&other) = default;
  ~XEntityReference() = default;
  [[nodiscard]] XMLValue value() const { return (m_value); }
  void setValue(const XMLValue &value) { m_value = value; }

private:
  XMLValue m_value;
};
// =======
// Element
// =======
struct XElement : XNode
{
  // Constructors/Destructors
  explicit XElement(XNode::Type nodeType = XNode::Type::element) : XNode(nodeType) {}
  explicit XElement(const std::string &name) : XElement() { m_name = name; }
  XElement(const XElement &other) = delete;
  XElement &operator=(XElement &other) = delete;
  XElement(XElement &&other) = default;
  XElement &operator=(XElement &&other) = default;
  ~XElement() = default;
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
  XElement &operator[](int index) const;
  XElement &operator[](const std::string &name) const;

private:
  std::string m_name;
  XMLAttribute::List m_namespaces;
  mutable XMLAttribute::List m_attributes;
};
// =======
// Comment
// =======
struct XComment : XNode
{
  // Constructors/Destructors
  explicit XComment(std::string comment = "", XNode::Type nodeType = XNode::Type::comment)
    : XNode(nodeType), m_comment(std::move(comment))
  {}
  XComment(const XComment &other) = delete;
  XComment &operator=(XComment &other) = delete;
  XComment(XComment &&other) = default;
  XComment &operator=(XComment &&other) = default;
  ~XComment() = default;
  [[nodiscard]] std::string comment() const { return (m_comment); }
  void setComment(const std::string &comment) { m_comment = comment; }

private:
  std::string m_comment;
};
// ==
// PI
// ==
struct XPI : XNode
{
  // Constructors/Destructors
  explicit XPI(XNode::Type nodeType = XNode::Type::pi) : XNode(nodeType) {}
  XPI(const XPI &other) = delete;
  XPI &operator=(XPI &other) = delete;
  XPI(XPI &&other) = default;
  XPI &operator=(XPI &&other) = default;
  ~XPI() = default;
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
struct XDTD : XNode
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
  // Constructors/Destructors
  explicit XDTD(IEntityMapper &entityMapper, XNode::Type nodeType = XNode::Type::dtd)
    : XNode(nodeType), m_entityMapper(entityMapper)
  {}
  XDTD(const XDTD &other) = delete;
  XDTD &operator=(XDTD &other) = delete;
  XDTD(XDTD &&other) = default;
  XDTD &operator=(XDTD &&other) = default;
  ~XDTD() = default;
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
  XDTD::Element &getElement(const std::string &elementName)
  {
    auto element = m_elements.find(elementName);
    if (element != m_elements.end()) { return (element->second); }
    throw XNode::Error("Could not find notation name.");
  }
  void addElement(const std::string &elementName, const XDTD::Element &element)
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
  std::unordered_map<std::string, XDTD::Element> m_elements;
  std::unordered_map<std::string, XMLExternalReference> m_notations;
  std::string m_unparsed;
};
// ===========================
// XMLNode reference converter
// ===========================
template<typename T> void CheckXNodeType(const XNode &xNode)
{
  if constexpr (std::is_same_v<T, XProlog>) {
    if (xNode.getType() != XNode::Type::prolog) { throw XNode::Error("Node not a prolog."); }
  } else if constexpr (std::is_same_v<T, XDeclaration>) {
    if (xNode.getType() != XNode::Type::declaration) { throw XNode::Error("Node not a declaration."); }
  } else if constexpr (std::is_same_v<T, XElement>) {
    if ((xNode.getType() != XNode::Type::root) && (xNode.getType() != XNode::Type::self)
        && (xNode.getType() != XNode::Type::element)) {
      throw XNode::Error("Node not a element.");
    }
  } else if constexpr (std::is_same_v<T, XContent>) {
    if (xNode.getType() != XNode::Type::content) { throw XNode::Error("Node not content."); }
  } else if constexpr (std::is_same_v<T, XEntityReference>) {
    if (xNode.getType() != XNode::Type::entity) { throw XNode::Error("Node not an entity."); }
  } else if constexpr (std::is_same_v<T, XComment>) {
    if (xNode.getType() != XNode::Type::comment) { throw XNode::Error("Node not a comment."); }
  } else if constexpr (std::is_same_v<T, XCDATA>) {
    if (xNode.getType() != XNode::Type::cdata) { throw XNode::Error("Node not CDATA."); }
  } else if constexpr (std::is_same_v<T, XPI>) {
    if (xNode.getType() != XNode::Type::pi) { throw XNode::Error("Node not a PI."); }
  } else if constexpr (std::is_same_v<T, XDTD>) {
    if (xNode.getType() != XNode::Type::dtd) { throw XNode::Error("Node not DTD."); }
  }
}
template<typename T> T &XRef(XNode &xNode)
{
  CheckXNodeType<T>(xNode);
  return (static_cast<T &>(xNode));
}
template<typename T> const T &XRef(const XNode &xNode)
{
  CheckXNodeType<T>(xNode);
  return (static_cast<const T &>(xNode));
}
// ====================
// XMLNode index access
// ====================
inline XNode &XNode::operator[](int index) const// Array
{
  if ((index >= 0) && (index < (static_cast<int>(XRef<XNode>(*this).getChildren().size())))) {
    return (*((XRef<XNode>(*this).getChildren()[index])));
  }
  throw XNode::Error("Invalid index used to access array.");
}
// ===================
// XMLNode name access
// ===================
inline XNode &XNode::operator[](const std::string &name) const// Array
{
  if (m_type <= XNode::Type::element) {
    for (const auto &element : XRef<XElement>(*this).getChildren()) {
      if (XRef<XElement>(*element).name() == name) { return (*element); }
    }
  }
  throw XNode::Error("Invalid index used to access array.");
}
// ===========================
// XMLNodeElement index access
// ===========================
inline XElement &XElement::operator[](int index) const// Array
{
  int number = 0;
  if ((index >= 0) && (index < (static_cast<int>(XRef<XElement>(*this).getChildren().size())))) {
    for (const auto &child : XRef<XNode>(*this).getChildren()) {
      if (XRef<XNode>(*child).getType() <= XNode::Type::element) {
        if (number == index) { return (XRef<XElement>(*child)); }
        number++;
      }
    }
  }
  throw XNode::Error("Invalid index used to access array.");
}
// ==========================
// XMLNodeElement name access
// ==========================
inline XElement &XElement::operator[](const std::string &name) const// Array
{
  if (getType() <= XNode::Type::element) {
    for (const auto &element : XRef<XElement>(*this).getChildren()) {
      if (XRef<XElement>(*element).m_name == name) { return (XRef<XElement>(*element)); }
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
  for (const auto &node : m_children) {
    if (node->getType() == XNode::Type::content) {
      result += XRef<XContent>(*node).content();
    } else if (node->getType() == XNode::Type::entity) {
      if (!XRef<XEntityReference>(*node).getChildren().empty()) {
        result += XRef<XEntityReference>(*node).getContents();
      } else {
        result += XRef<XEntityReference>(*node).value().parsed;
      }
    } else if (node->getType() == XNode::Type::cdata) {
      result += XRef<XCDATA>(*node).CDATA();
    }
  }
  return (result);
}
}// namespace XML_Lib
