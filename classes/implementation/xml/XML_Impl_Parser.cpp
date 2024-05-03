//
// Class: XML_Impl_Parser
//
// Description: XML parser code. All parsing of characters takes place having
// converted the characters to UTF-16 to make the process easier (any data once
// parsed is stored in UTF-8 strings).
//
// Dependencies: C++20 - Language standard features used..
//

#include "XML_Impl.hpp"

namespace XML_Lib {

/// <summary>
/// Reset content node whitespace flag if set.
/// </summary>
/// <param name="xNode">Current element XNode.</param>
void resetWhiteSpace(XNode &xNode)
{
  if (!xNode.getChildren().empty()) {
    if (xNode.getChildren().back().isContent()) { XRef<XContent>(xNode.getChildren().back()).setIsWhiteSpace(false); }
  }
}

/// <summary>
/// Add content XNode to elements child list.
/// </summary>
/// <param name="xNode">Current element XNode.</param>
/// <param name="content">Content to add to new content XNode (XMLNodeContent).</param>
void addContentToElementChildList(XNode &xNode, const std::string &content)
{
  // Make sure there is a content XNode to receive characters
  if (xNode.getChildren().empty() || !xNode.getChildren().back().isContent()) {
    bool isWhiteSpace = true;
    if (!xNode.getChildren().empty()) {
      if ((xNode.getChildren().back().isCDATA()) || (xNode.getChildren().back().isEntity())) { isWhiteSpace = false; }
    }
    xNode.addChild(XNode::make<XContent>(isWhiteSpace));
  }
  XContent &xmlContent = XRef<XContent>(xNode.getChildren().back());
  if (xmlContent.isWhiteSpace()) {
    for (const auto ch : content) {
      if (!std::iswspace(ch)) {
        xmlContent.setIsWhiteSpace(false);
        break;
      }
    }
  }
  xmlContent.addContent(content);
}

/// <summary>
/// Add new namespaces to current global list.
/// </summary>
/// <param name="attributes">Current element attrutes..</param>
/// <param name="namespaces">Current global namespace list.</param>
void addNewNameSpaces(const std::vector<XMLAttribute> &attributes, std::vector<XMLAttribute> &namespaces)
{
  for (const auto &attribute : attributes) {
    if (attribute.getName().starts_with("xmlns")) {
      namespaces.emplace_back((attribute.getName().size() > 5) ? attribute.getName().substr(6) : ":",
        XMLValue{ attribute.getUnparsed(), attribute.getParsed() });
    }
  }
}

/// <summary>
/// Parse entity reference as XML and add XNodes produced to the current XNode.
/// </summary>
/// <param name="xNode">Current element XNode.</param>
/// <param name="entityReference">Entity reference to be parsed for XML.</param>
void XML_Impl::parseEntityReferenceXML(XNode &xNode, const XMLValue &entityReference)
{
  auto xElement = XNode::make<XElement>();
  BufferSource entitySource(entityReference.getParsed());
  // Parse entity XML
  while (entitySource.more()) { parseElementInternal(entitySource, xNode); }
  // Place into XNode (element) child list
  for (auto &xNodeChild : xElement.getChildren()) { xNode.addChild(xNodeChild); }
}

/// <summary>
/// Parse any comments, PI or whitespace in prolog/epilog of XML file.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <param name="xProlog">XMLprolog XNode.</param>
/// <returns>True then items parsed.</returns>
bool XML_Impl::parseCommentsPIAndWhiteSpace(ISource &source, XNode &xProlog)
{
  if (source.match("<!--")) {
    xProlog.addChild(parseComment(source));
    return (true);
  } else if (source.match("<?")) {
    xProlog.addChild(parsePI(source));
    return (true);
  } else if (source.isWS()) {
    parseWhiteSpaceToContent(source, xProlog);
    return (true);
  }
  return (false);
}

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
std::string
  XML_Impl::parseDeclarationAttribute(ISource &source, const std::string &name, const std::set<std::string> &values)
{
  std::string value;
  source.ignoreWS();
  if (source.match("=")) {
    source.ignoreWS();
    value = parseValue(source).getParsed();
    if (name == "encoding") { value = toUpperString(value); }
    if (!values.contains(value)) { throw SyntaxError("Unsupported XML " + name + " value '" + value + "' specified."); }
  } else {
    throw SyntaxError(source.getPosition(), "Missing '=' after " + name + ".");
  }
  return (value);
}

/// <summary>
/// Parse a XML comment, create a XComment for it and add to list
/// of elements for the current XElement.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <returns>Pointer to comment XNode.</returns>
XNode XML_Impl::parseComment(ISource &source)
{
  std::string comment;
  while (source.more() && !source.match("--")) {
    comment += toUtf8(source.current());
    source.next();
  }
  if (!source.match(">")) { throw SyntaxError(source.getPosition(), "Missing closing '>' for comment line."); }
  return (XNode::make<XComment>(comment));
}

/// <summary>
/// Parse a XML process instruction, create an XPI for it and add it to
/// the list of elements under the current XElement.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <returns>Pointer to PI XNode.</returns>
XNode XML_Impl::parsePI(ISource &source)
{
  std::string name{ parseName(source) };
  // Check not a declartion
  if (name == "xml") {
    throw SyntaxError(source.getPosition(), "Declaration allowed only at the start of the document.");
  }
  std::string parameters;
  while (source.more() && !source.match("?>")) {
    parameters += toUtf8(source.current());
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
XNode XML_Impl::parseCDATA(ISource &source)
{
  std::string cdata;
  while (source.more() && !source.match("]]>")) {
    if (source.match("<![CDATA[")) {
      throw SyntaxError(source.getPosition(), "Nesting of CDATA sections is not allowed.");
    }
    cdata += toUtf8(source.current());
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
  while (source.more() && source.current() != '/' && source.current() != '>') {
    std::string attributeName{ parseName(source) };
    if (!source.match("=")) {
      throw SyntaxError(source.getPosition(), "Missing '=' between attribute name and value.");
    }
    source.ignoreWS();
    XMLValue attributeValue = parseValue(source, *entityMapper);
    if (!validAttributeValue(attributeValue)) {
      throw SyntaxError(source.getPosition(), "Attribute value contains invalid character '<', '\"', ''' or '&'.");
    }
    if (XMLAttribute::isAttrubutePresent(attributes, attributeName)) {
      throw SyntaxError("Attribute '" + attributeName + "' defined more than once within start tag.");
    }
    attributes.emplace_back(attributeName, attributeValue);
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
    whiteSpace += toUtf8(source.current());
    source.next();
  }
  addContentToElementChildList(xNode, whiteSpace);
}

/// <summary>
/// Parse any content that is found inside an element.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <param name="xNode">Current element XNode.</param>
void XML_Impl::parseContent(ISource &source, XNode &xNode)
{
  XMLValue content{ parseCharacter(source) };
  if (content.isReference()) {
    if (content.isEntityReference()) { content = entityMapper->map(content); }
    auto xEntityReference = XNode::make<XEntityReference>(content);
    if (content.isEntityReference()) {
      // Does entity contain start tag ?
      // YES then XML into current element list
      if (content.getParsed().starts_with("<")) {
        parseEntityReferenceXML(xNode, content);
        return;
      }
      // NO XML into entity elements list.
      else {
        parseEntityReferenceXML(xEntityReference, content);
        resetWhiteSpace(xNode);
      }
    }
    xNode.addChild(std::move(xEntityReference));
  } else {
    addContentToElementChildList(xNode, content.getParsed());
  }
}

/// <summary>
/// Parse element internal area, generating any XNode(s) and adding them
/// to the child list of the current XElement. This can be anything from
/// comments, program instructions, CDATA, nested elements or even content.
/// </summary>
/// <param name="source">XMl source stream.</param>
/// <param name="xNode">Current element XNode.</param>
void XML_Impl::parseElementInternal(ISource &source, XNode &xNode)
{
  if (source.match("<!--")) {
    xNode.addChild(parseComment(source));
  } else if (source.match("<?")) {
    xNode.addChild(parsePI(source));
  } else if (source.match("<![CDATA[")) {
    resetWhiteSpace(xNode);
    xNode.addChild(parseCDATA(source));
  } else if (source.match("<")) {
    xNode.addChild(parseElement(source, XRef<XElement>(xNode).getNamespaceList()));
    XElement &xNodeChildElement = XRef<XElement>(xNode.getChildren().back());
    if (auto pos = xNodeChildElement.name().find(':'); pos != std::string::npos) {
      if (!xNodeChildElement.isNameSpacePresent(xNodeChildElement.name().substr(0, pos))) {
        throw SyntaxError(source.getPosition(), "Namespace used but not defined.");
      }
    }
  } else {
    if (source.match("</")) {
      throw SyntaxError(source.getPosition(), "Missing closing tag.");
    } else if (source.match("]]>")) {
      throw SyntaxError(source.getPosition(), "']]>' invalid in element content area.");
    }
    parseContent(source, xNode);
  }
}

/// <summary>
/// Parse current XML element found.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <param name="outerNamespaces">Current list of outerNamespaces.</param>
/// <returns>Pointer to element XNode.</returns>
XNode XML_Impl::parseElement(ISource &source, const std::vector<XMLAttribute> &outerNamespaces)
{
  // Parse tag and attributes
  const std::string name{ parseTagName(source) };
  const std::vector<XMLAttribute> attributes{ parseAttributes(source) };
  // Add any new namespaces
  std::vector<XMLAttribute> namespaces{ outerNamespaces };
  addNewNameSpaces(attributes, namespaces);
  // Create element XNode
  if (XNode xNode; source.match(">")) {
    // Normal element tag
    if (!hasRoot) {
      xNode = XNode::make<XRoot>(name, attributes, namespaces);
      hasRoot = true;
    } else {
      xNode = XNode::make<XElement>(name, attributes, namespaces);
    }
    while (source.more() && !source.match("</")) { parseElementInternal(source, xNode); }
    if (source.match(toUtf16(XRef<XElement>(xNode).name()) + u">")) { return (xNode); }
  } else if (source.match("/>")) {
    // Self closing element tag
    return (XNode::make<XSelf>(name, attributes, namespaces));
  }
  throw SyntaxError(source.getPosition(), "Missing closing tag.");
}

/// <summary>
/// Parse XML declaration and return XNode for it.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <returns>Pointer to declaration XNode.</returns>
XNode XML_Impl::parseDeclaration(ISource &source)
{
  std::string version{ "1.0" };
  std::string encoding{ "UTF-8" };
  std::string standalone{ "no" };
  source.ignoreWS();
  if (source.match("<?xml")) {
    source.ignoreWS();
    if (source.match("version")) {
      version = parseDeclarationAttribute(source, "version", { "1.0", "1.1" });
    } else {
      throw SyntaxError(source.getPosition(), "Version missing from declaration.");
    }
    if (source.match("encoding")) { encoding = parseDeclarationAttribute(source, "encoding", { "UTF-8", "UTF-16" }); }
    if (source.match("standalone")) { standalone = parseDeclarationAttribute(source, "standalone", { "yes", "no" }); }
    if (source.match("encoding")) {
      throw SyntaxError(source.getPosition(), "Incorrect order for version, encoding and standalone attributes.");
    }
    if (!source.match("?>")) { throw SyntaxError(source.getPosition(), "Declaration end tag not found."); }
  }
  return (XNode::make<XDeclaration>(version, encoding, standalone));
}

/// <summary>
/// Parse any XML tail that is present. This can include comments, PI and white space.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <param name="xProlog">Prolog XNode.</param>
void XML_Impl::parseEpilog(ISource &source, XNode &xProlog)
{
  while (source.more()) {
    if (!parseCommentsPIAndWhiteSpace(source, xProlog)) {
      throw SyntaxError(source.getPosition(), "Extra content at the end of document.");
    }
  }
}

/// <summary>
/// Parse XML prolog and create the necessary element XNodes for it. Valid
/// parts of the prolog include declaration (first line if present),
/// processing instructions, comments, whitespace and a Document Type Declaration (DTD).
/// </summary>
/// <param name="source">XML source stream.</param>
/// <returns>Pointer to prolog XNode.</returns>
XNode XML_Impl::parseProlog(ISource &source)
{
  auto xProlog = XNode::make<XProlog>();
  xProlog.addChild(parseDeclaration(source));
  while (source.more()) {
    if (source.match("<!DOCTYPE")) {
      xProlog.addChild(parseDTD(source));
    } else if (parseCommentsPIAndWhiteSpace(source, xProlog)) {
      continue;
    } else if (source.current() == '<') {
      break;// --- Break out as potential root element detected ---
    } else {
      throw SyntaxError(source.getPosition(), "Content detected before root element.");
    }
  }
  return (xProlog);
}

/// <summary>
/// Parse XML read from source stream into internal object generating an exception
/// if a syntax error in the XML is found (not well formed).
/// </summary>
void XML_Impl::parse(ISource &source)
{
  // Reset XML before next parse
  entityMapper->resetToDefault();
  hasRoot = false;
  validator.reset();
  // Handle prolog
  xmlRoot = parseProlog(source);
  // Handle main body
  if (source.match("<")) {
    xmlRoot.addChild(parseElement(source, {}));
  } else {
    throw SyntaxError(source.getPosition(), "Missing root element.");
  }
  // Handle any epilog
  parseEpilog(source, xmlRoot);
}
}// namespace XML_Lib
