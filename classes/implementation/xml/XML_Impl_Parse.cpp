//
// Class: XML_Impl
//
// Description: XML parser code. All parsing of characters takes place having
// converted the characters to UTF-32 to make the process easier (any data once
// parsed is stored in UTF-8 strings).
//
// Dependencies:   C20++ - Language standard features used.
//
// =================
// CLASS DEFINITIONS
// =================
#include "XML_Impl.hpp"
// ====================
// CLASS IMPLEMENTATION
// ====================
// =================
// LIBRARY NAMESPACE
// =================
namespace XML_Lib {
// ===========================
// PRIVATE TYPES AND CONSTANTS
// ===========================
// ==========================
// PUBLIC TYPES AND CONSTANTS
// ==========================
// ========================
// PRIVATE STATIC VARIABLES
// ========================
// =======================
// PUBLIC STATIC VARIABLES
// =======================

// ===============
// PRIVATE METHODS
// ===============
/// <summary>
/// Parse a element tag name and set its value in current XElement.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <returns>Element tag name.</returns>
std::string XML_Impl::parseTagName(ISource &source) { return (parseName(source)); }
/// <summary>
/// Parse declaration attribute and validate its value.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <param name="name">Attribute name string.</param>
/// <param name="values">Set of valid attribute values.</param>
/// <param name="toUpper">==true then convert attribute value all to uppercase.</param>
/// <returns>Valid attribute value.</returns>
std::string XML_Impl::parseDeclarationAttribute(ISource &source,
  const std::string &name,
  const std::set<std::string> &values,
  bool toUpper = false)
{
  std::string result;
  source.ignoreWS();
  if (!source.match(U"=")) { throw SyntaxError(source.getPosition(), "Missing '=' after " + name + "."); }
  source.ignoreWS();
  result = parseValue(source).parsed;
  if (toUpper) { result = toUpperString(result); }
  if (!values.contains(result)) { throw SyntaxError("Unsupported XML " + name + " value '" + result + "' specified."); }
  return (result);
}
/// <summary>
/// Parse a XML comment, create a XComment for it and add to list
/// of elements for the current XElement.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <returns>Pointer to comment XNode.</returns>
std::unique_ptr<XNode> XML_Impl::parseComment(ISource &source)
{
  std::string comment;
  while (source.more() && !source.match(U"--")) {
    comment += source.current_to_bytes();
    source.next();
  }
  if (!source.match(U">")) { throw SyntaxError(source.getPosition(), "Missing closing '>' for comment line."); }
  return (XNode::make<XComment>(comment));
}
/// <summary>
/// Parse a XML process instruction, create an XPI for it and add it to
/// the list of elements under the current XElement.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <returns>Pointer to PI XNode.</returns>
std::unique_ptr<XNode> XML_Impl::parsePI(ISource &source)
{
  std::string name{ parseName(source) };
  std::string parameters;
  while (source.more() && !source.match(U"?>")) {
    parameters += source.current_to_bytes();
    source.next();
  }
  return (XNode::make<XPI>(name, parameters));
}
/// <summary>
/// Parse an XML CDATA section, create an XCDATA for it and add it to
/// the list of elements under the current XElement.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <returns>Pointer to CDATA XNode.</returns>
std::unique_ptr<XNode> XML_Impl::parseCDATA(ISource &source)
{
  std::string cdata;
  while (source.more() && !source.match(U"]]>")) {
    if (source.match(U"<![CDATA[")) {
      throw SyntaxError(source.getPosition(), "Nesting of CDATA sections is not allowed.");
    }
    cdata += source.current_to_bytes();
    source.next();
  }
  return (XNode::make<XCDATA>(cdata));
}
/// <summary>
/// Parse list of attributes (name/value pairs) that exist in a tag and add them to
/// the list of attributes associated with the current XElement.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <returns>XML element attribute list.</returns>
std::vector<XMLAttribute> XML_Impl::parseAttributes(ISource &source)
{
  std::vector<XMLAttribute> attributes;
  std::set<std::string> attributeNames;
  while (source.more() && source.current() != '?' && source.current() != '/' && source.current() != '>') {
    std::string attributeName = parseName(source);
    if (!source.match(U"=")) {
      throw SyntaxError(source.getPosition(), "Missing '=' between attribute name and value.");
    }
    source.ignoreWS();
    XMLValue attributeValue = parseValue(source, *m_entityMapper);
    if (!validAttributeValue(attributeValue)) {
      throw SyntaxError(source.getPosition(), "Attribute value contains invalid character '<', '\"', ''' or '&'.");
    }
    if (!attributeNames.contains(attributeName)) {
      attributes.emplace_back(attributeName, attributeValue);
      attributeNames.insert(attributeName);
    } else {
      throw SyntaxError(source.getPosition(), "Attribute defined more than once within start tag.");
    }
  }
  return (attributes);
}
/// <summary>
/// Parse white space add to current XNodes child list.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <param name="xNode">Current XNode.</param>
void XML_Impl::parseWhiteSpaceToContent(ISource &source, XNode &xNode)
{
  std::string whiteSpace;
  while (source.more() && source.isWS()) {
    whiteSpace += source.current_to_bytes();
    source.next();
  }
  addContentToElementChildList(xNode, whiteSpace);
}
/// <summary>
/// Parse any element content that is found.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <param name="xNode">Current element XNode.</param>
void XML_Impl::parseElementContent(ISource &source, XNode &xNode)
{
  XMLValue content{ parseCharacter(source) };
  if (content.isReference()) {
    if (content.isEntityReference()) { content = m_entityMapper->map(content); }
    auto xEntityReference = XNode::make<XEntityReference>(content);
    if (content.isEntityReference()) {
      // Does entity contain start tag ?
      // YES then XML into current element list
      if (content.parsed.starts_with("<")) {
        processEntityReferenceXML(xNode, content);
        return;
      }
      // NO XML into entity elements list.
      else {
        processEntityReferenceXML(*xEntityReference, content);
        resetWhiteSpace(xNode);
      }
    }
    xNode.addChild(std::move(xEntityReference));
  } else {
    addContentToElementChildList(xNode, content.parsed);
  }
}
/// <summary>
/// Parse element content area, generating any XNode(s) and adding them
/// to the list of the current XElement.
/// </summary>
/// <param name="source">XMl source stream.</param>
/// <param name="xNode">Current element XNode.</param>
void XML_Impl::parseElementContents(ISource &source, XNode &xNode)
{
  if (source.match(U"<!--")) {
    xNode.addChild(parseComment(source));
  } else if (source.match(U"<?")) {
    xNode.addChild(parsePI(source));
  } else if (source.match(U"<![CDATA[")) {
    resetWhiteSpace(xNode);
    xNode.addChild(parseCDATA(source));
  } else if (source.match(U"<")) {
    xNode.addChild(parseElement(source, XRef<XElement>(xNode).getNamespaceList()));
    XElement &xNodeChildElement = XRef<XElement>(*xNode.getChildren().back());
    if (auto pos = xNodeChildElement.name().find(':'); pos != std::string::npos) {
      if (!xNodeChildElement.isNameSpacePresent(xNodeChildElement.name().substr(0, pos))) {
        throw SyntaxError(source.getPosition(), "Namespace used but not defined.");
      }
    }
  } else {
    if (source.match(U"</")) {
      throw SyntaxError(source.getPosition(), "Missing closing tag.");
    } else if (source.match(U"]]>")) {
      throw SyntaxError(source.getPosition(), "']]>' invalid in element content area.");
    }
    parseElementContent(source, xNode);
  }
}
/// <summary>
/// Parse current XML element found.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <param name="outerNamespaces">Current list of outerNamespaces.</param>
/// <returns>Pointer to element XNode.</returns>
std::unique_ptr<XNode>
  XML_Impl::parseElement(ISource &source, const std::vector<XMLAttribute> &outerNamespaces, XNode::Type xNodeType)
{
  // Parse tag and attributes
  const std::string name{ parseTagName(source) };
  const std::vector<XMLAttribute> attributes{ parseAttributes(source) };
  // Add any new namespaces
  std::vector<XMLAttribute> namespaces{ outerNamespaces };
  for (const auto &attribute : attributes) {
    if (attribute.name.starts_with("xmlns")) {
      namespaces.emplace_back((attribute.name.size() > 5) ? attribute.name.substr(6) : ":", attribute.value);
    }
  }
  // Create element XNode
  if (source.match(U">")) {
    // Normal element tag
    auto xNode = XNode::make<XElement>(name, attributes, namespaces, xNodeType);
    while (source.more() && !source.match(U"</")) { parseElementContents(source, *xNode); }
    if (source.match(source.from_bytes(xNode->name()) + U">")) { return (xNode); }
  } else if (source.match(U"/>")) {
    // Self closing element tag
    return (XNode::make<XElement>(name, attributes, namespaces, XNode::Type::self));
  }
  throw SyntaxError(source.getPosition(), "Missing closing tag.");
}

/// <summary>
/// Parse XML declaration and return XNode for it.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <returns>Pointer to declaration XNode.</returns>
std::unique_ptr<XNode> XML_Impl::parseDeclaration(ISource &source)
{
  std::string version{ "1.0" };
  std::string encoding{ "UTF-8" };
  std::string standalone{ "no" };
  source.ignoreWS();
  if (source.match(U"<?xml")) {
    source.ignoreWS();
    if (source.match(U"version")) {
      version = parseDeclarationAttribute(source, "version", { "1.0", "2.0" });
    } else {
      throw SyntaxError(source.getPosition(), "Version missing from declaration.");
    }
    if (source.match(U"encoding")) {
      encoding = parseDeclarationAttribute(source, "encoding", { "UTF-8", "UTF-16" }, true);
    }
    if (source.match(U"standalone")) { standalone = parseDeclarationAttribute(source, "standalone", { "yes", "no" }); }
    if (source.match(U"encoding")) {
      throw SyntaxError(source.getPosition(), "Incorrect order for version, encoding and standalone attributes.");
    }
    if (!source.match(U"?>")) { throw SyntaxError(source.getPosition(), "Declaration end tag not found."); }
  }
  return (XNode::make<XDeclaration>(version, encoding, standalone));
}
/// <summary>
/// Parse any XML tail that is present. This can include comments, PI and white space.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <param name="xProlog">Prolog XNode.</param>
void XML_Impl::parseTail(ISource &source, XNode &xProlog)
{
  while (source.more()) {
    if (source.match(U"<!--")) {
      xProlog.addChild(parseComment(source));
    } else if (source.match(U"<?")) {
      xProlog.addChild(parsePI(source));
    } else if (source.isWS()) {
      parseWhiteSpaceToContent(source, xProlog);
    } else {
      throw SyntaxError(source.getPosition(), "Extra content at the end of document.");
    }
  }
}
/// <summary>
/// Parse XML DTD and return any XNode created for it.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <returns>Pointer to DTD XNode.</returns>
std::unique_ptr<XNode> XML_Impl::parseDTD(ISource &source)
{
  auto xNode = XNode::make<XDTD>(*m_entityMapper);
  DTD dtd{ *xNode };
  dtd.parse(source);
  m_validator = std::make_unique<DTD_Validator>(*xNode);
  return (xNode);
}
/// <summary>
/// Parse XML prolog and create the necessary element XNodes for it. Valid
/// parts of the prolog include declaration (first line if present),
/// processing instructions, comments, whitespace and a Document Type Declaration (DTD).
/// </summary>
/// <param name="source">XML source stream.</param>
/// <returns>Pointer to prolog XNode.</returns>
std::unique_ptr<XNode> XML_Impl::parseProlog(ISource &source)
{
  auto xProlog = XNode::make<XProlog>();
  xProlog->addChild(parseDeclaration(source));
  while (source.more()) {
    if (source.match(U"<!--")) {
      xProlog->addChild(parseComment(source));
    } else if (source.match(U"<?")) {
      xProlog->addChild(parsePI(source));
    } else if (source.isWS()) {
      parseWhiteSpaceToContent(source, *xProlog);
    } else if (source.match(U"<!DOCTYPE")) {
      for (auto &element : xProlog->getChildren()) {
        if (element->getType() == XNode::Type::dtd) {
          throw SyntaxError(source.getPosition(), "More than one DOCTYPE declaration.");
        }
      }
      xProlog->addChild(parseDTD(source));
    } else if (source.current() == '<') {
      break;// --- Break out as potential root element detected ---
    } else {
      throw SyntaxError(source.getPosition(), "Content detected before root element.");
    }
  }
  if (!source.match(U"<")) { throw SyntaxError(source.getPosition(), "Missing root element."); }
  return (xProlog);
}
/// <summary>
/// Parse XML from source stream.
/// </summary>
std::unique_ptr<XNode> XML_Impl::parseXML(ISource &source)
{
  auto xProlog = parseProlog(source);
  xProlog->addChild(parseElement(source, {}, XNode::Type::root));
  parseTail(source, *xProlog);
  return (xProlog);
}
}// namespace XML_Lib