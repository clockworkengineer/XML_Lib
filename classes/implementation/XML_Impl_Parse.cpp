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
#include "XML.hpp"
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
/// Parse a element tag name and set its value in current XMLNodeElement.
/// </summary>
/// <param name="source">XML source stream.</param>
std::string XML_Impl::parseTagName(ISource &source) { return (parseName(source)); }
/// <summary>
/// Parse a XML comment, create a XMLNodeComment for it and add to list
/// of elements for the current XMLNodeElement.
/// </summary>
/// <param name="source">XML source stream.</param>
std::unique_ptr<XNode> XML_Impl::parseComment(ISource &source)
{
  std::string comment;
  while (source.more() && !source.match(U"--")) {
    comment += source.current_to_bytes();
    source.next();
  }
  if (!source.match(U">")) { throw SyntaxError(source.getPosition(), "Missing closing '>' for comment line."); }
  return (std::make_unique<XComment>(XComment{ comment }));
}
/// <summary>
/// Parse a XML process instruction, create an XMLNodePI for it and add it to
/// the list of elements under the current XMLNodeElement.
/// </summary>
/// <param name="source">XML source stream.</param>
std::unique_ptr<XNode> XML_Impl::parsePI(ISource &source)
{
  XPI xNodePI;
  xNodePI.setName(parseName(source));
  std::string parameters;
  while (source.more() && !source.match(U"?>")) {
    parameters += source.current_to_bytes();
    source.next();
  }
  xNodePI.setParameters(parameters);
  return (std::make_unique<XPI>(std::move(xNodePI)));
}
/// <summary>
/// Parse an XML CDATA section, create an XNodeCDATA for it and add it to
/// the list of elements under the current XMLNodeElement.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <param name="xNode">Current element node.</param>
std::unique_ptr<XNode> XML_Impl::parseCDATA(ISource &source)
{
  XCDATA xNodeCDATA;
  std::string cdata;
  while (source.more() && !source.match(U"]]>")) {
    if (source.match(U"<![CDATA[")) {
      throw SyntaxError(source.getPosition(), "Nesting of CDATA sections is not allowed.");
    }
    cdata += source.current_to_bytes();
    source.next();
  }
  xNodeCDATA.setCDATA(cdata);
  return (std::make_unique<XCDATA>(std::move(xNodeCDATA)));
}
/// <summary>
/// Parse list of attributes (name/value pairs) that exist in a tag and add them to
/// the list of attributes associated with the current XMLNodeElement.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <param name="xNode">Current element node.</param>
XMLAttribute::List XML_Impl::parseAttributes(ISource &source)
{
  XMLAttribute::List attributes;
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
/// <param name="xNode">Current element node.</param>
void XML_Impl::parseElementContent(ISource &source, XNode &xNode)
{
  XMLValue content{ parseCharacter(source) };
  if (content.isReference()) {
    if (content.isEntityReference()) { content = m_entityMapper->map(content); }
    XEntityReference xNodeEntityReference(content);
    if (content.isEntityReference()) {
      // Does entity contain start tag ?
      // YES then XML into current element list
      if (content.parsed.starts_with("<")) {
        processEntityReferenceXML(xNode, content);
        return;
      }
      // NO XML into entity elements list.
      else {
        processEntityReferenceXML(xNodeEntityReference, content);
        resetWhiteSpace(xNode);
      }
    }
    xNode.getChildren().emplace_back(std::make_unique<XEntityReference>(std::move(xNodeEntityReference)));
  } else {
    addContentToElementChildList(xNode, content.parsed);
  }
}
/// <summary>
/// Parse element content area, generating any XNodes and adding them
/// to the list of the current XMLNodeElement.
/// </summary>
/// <param name="source">XMl source stream.</param>
/// <param name="xNode">Current element node.</param>
void XML_Impl::parseElementContents(ISource &source, XNode &xNode)
{
  if (source.match(U"<!--")) {
    xNode.getChildren().emplace_back(parseComment(source));
  } else if (source.match(U"<?")) {
    xNode.getChildren().emplace_back(parsePI(source));
  } else if (source.match(U"<![CDATA[")) {
    resetWhiteSpace(xNode);
    xNode.getChildren().emplace_back(parseCDATA(source));
  } else if (source.match(U"<")) {
    xNode.getChildren().emplace_back(parseElement(source, XRef<XElement>(xNode).getNameSpaceList()));
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
/// <param name="namespaces">Current list of namespaces.</param>
std::unique_ptr<XNode>
  XML_Impl::parseElement(ISource &source, const XMLAttribute::List &namespaces, XNode::Type xNodeType)
{
  XElement xNodeElement{ xNodeType };
  for (const auto &ns : namespaces) { xNodeElement.addNameSpace(ns.name, ns.value); }
  xNodeElement.setName(parseTagName(source));
  for (const auto &attribute : parseAttributes(source)) {
    xNodeElement.addAttribute(attribute.name, attribute.value);
    if (attribute.name.starts_with("xmlns")) {
      xNodeElement.addNameSpace((attribute.name.size() > 5) ? attribute.name.substr(6) : ":", attribute.value);
    }
  }
  if (source.match(U">")) {
    while (source.more() && !source.match(U"</")) { parseElementContents(source, xNodeElement); }
    if (!source.match(source.from_bytes(xNodeElement.name()) + U">")) {
      throw SyntaxError(source.getPosition(), "Missing closing tag.");
    }
  } else if (source.match(U"/>")) {
    // Self closing element tag
    xNodeElement.setType(XNode::Type::self);
  } else {
    throw SyntaxError(source.getPosition(), "Missing closing tag.");
  }
  return (std::make_unique<XElement>(std::move(xNodeElement)));
}
/// <summary>
/// </summary>
/// <param name="source">XML source stream.</param>
std::unique_ptr<XNode> XML_Impl::parseDeclaration(ISource &source)
{
  XDeclaration declaration;
  if (source.match(U"<?xml")) {
    source.ignoreWS();
    if (source.match(U"version")) {
      source.ignoreWS();
      if (!source.match(U"=")) { throw SyntaxError(source.getPosition(), "Missing '=' after version."); }
      source.ignoreWS();
      declaration.setVersion(parseValue(source).parsed);
      if (!declaration.isValidVersion()) {
        throw SyntaxError(source.getPosition(), "Unsupported version " + declaration.version() + ".");
      }
    } else {
      throw SyntaxError(source.getPosition(), "Version missing from declaration.");
    }
    if (source.match(U"encoding")) {
      source.ignoreWS();
      if (!source.match(U"=")) { throw SyntaxError(source.getPosition(), "Missing '=' after encoding."); }
      source.ignoreWS();
      declaration.setEncoding(toUpperString(parseValue(source).parsed));
      if (!declaration.isValidEncoding()) {
        throw SyntaxError(source.getPosition(), "Unsupported encoding " + declaration.encoding() + " specified.");
      }
    }
    if (source.match(U"standalone")) {
      source.ignoreWS();
      if (!source.match(U"=")) { throw SyntaxError(source.getPosition(), "Missing '=' after standalone."); }
      source.ignoreWS();
      declaration.setStandalone(parseValue(source).parsed);
      if (!declaration.isValidStandalone()) {
        throw SyntaxError(source.getPosition(), "Invalid standalone value of '" + declaration.standalone() + "'.");
      }
    }
    if (source.match(U"encoding")) {
      throw SyntaxError(source.getPosition(), "Incorrect order for version, encoding and standalone attributes.");
    }
    if (!source.match(U"?>")) { throw SyntaxError(source.getPosition(), "Declaration end tag not found."); }
  }
  return (std::make_unique<XDeclaration>(std::move(declaration)));
}
/// <summary>
/// </summary>
/// <param name="source">XML source stream.</param>
void XML_Impl::parseXMLTail(ISource &source)
{
  while (source.more()) {
    if (source.match(U"<!--")) {
      prolog().getChildren().emplace_back(parseComment(source));
    } else if (source.match(U"<?")) {
      prolog().getChildren().emplace_back(parsePI(source));
    } else if (source.isWS()) {
      parseWhiteSpaceToContent(source, prolog());
    } else {
      throw SyntaxError(source.getPosition(), "Extra content at the end of document.");
    }
  }
}
std::unique_ptr<XNode> XML_Impl::parseDTD(ISource &source)
{
  if (m_dtd == nullptr) {
    std::unique_ptr<XNode> xNodeDTD = std::make_unique<XDTD>(*m_entityMapper);
    m_dtd = std::make_unique<DTD>(XRef<XDTD>(*xNodeDTD));
    m_dtd->parse(source);
    m_validator = std::make_unique<XML_Validator>(XRef<XDTD>(*xNodeDTD));
    return (xNodeDTD);
  }

  throw SyntaxError(source.getPosition(), "More than one DOCTYPE declaration.");
}
/// <summary>
/// Parse XML prolog and create the necessary XMLNodeElements for it. Valid
/// parts of the prolog include declaration (first line if present),
/// processing instructions, comments, whitespace content and XML
/// Document Type Declaration (DTD).
/// </summary>
/// <param name="source">XML source stream.</param>
std::unique_ptr<XNode> XML_Impl::parseProlog(ISource &source)
{
  XProlog xNodeProlog;
  source.ignoreWS();
  xNodeProlog.getChildren().emplace_back(parseDeclaration(source));
  while (source.more()) {
    if (source.match(U"<!--")) {
      xNodeProlog.getChildren().emplace_back(parseComment(source));
    } else if (source.match(U"<?")) {
      xNodeProlog.getChildren().emplace_back(parsePI(source));
    } else if (source.isWS()) {
      parseWhiteSpaceToContent(source, xNodeProlog);
    } else if (source.match(U"<!DOCTYPE")) {
      xNodeProlog.getChildren().emplace_back(parseDTD(source));
    } else if (source.current() != '<') {
      throw SyntaxError(source.getPosition(), "Content detected before root element.");
    } else {
      // Break out as root element detected
      break;
    }
  }
  return (std::make_unique<XProlog>(std::move(xNodeProlog)));
}
/// <summary>
/// Parse XML from source stream.
/// </summary>
void XML_Impl::parseXML(ISource &source)
{
  m_prolog = parseProlog(source);
  if (source.match(U"<")) {
    prolog().getChildren().emplace_back(parseElement(source, {}, XNode::Type::root));
    parseXMLTail(source);
  } else {
    throw SyntaxError(source.getPosition(), "Missing root element.");
  }
}
}// namespace XML_Lib
