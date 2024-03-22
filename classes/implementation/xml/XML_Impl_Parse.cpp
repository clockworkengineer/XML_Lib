//
// Class: XML_Impl
//
// Description: XML parser code. All parsing of characters takes place having
// converted the characters to UTF-32 to make the process easier (any data once
// parsed is stored in UTF-8 strings).
//
// Dependencies:   C++20 - Language standard features used.
//

#include "XML_Impl.hpp"

namespace XML_Lib {

/// <summary>
/// Parse any comments, PI or whitespace in prolog or at end of XML file.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <param name="XProlog">XMLprolog XNode.</param>
/// <returns>True then items parsed.</returns>
bool XML_Impl::parseCommentsPIAndWhiteSpace(ISource &source, XNode &xProlog)
{
  if (source.match("<!--")) {
    xProlog.addChildren(parseComment(source));
    return (true);
  } else if (source.match("<?")) {
    xProlog.addChildren(parsePI(source));
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
std::string XML_Impl::parseDeclarationAttribute(ISource &source,
  const std::string &name,
  const std::set<std::string> &values,
  bool toUpper = false)
{
  std::string result;
  source.ignoreWS();
  if (!source.match("=")) { throw XML::SyntaxError(source.getPosition(), "Missing '=' after " + name + "."); }
  source.ignoreWS();
  result = parseValue(source).getParsed();
  if (toUpper) { result = toUpperString(result); }
  if (!values.contains(result)) {
    throw XML::SyntaxError("Unsupported XML " + name + " value '" + result + "' specified.");
  }
  return (result);
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
  if (!source.match(">")) { throw XML::SyntaxError(source.getPosition(), "Missing closing '>' for comment line."); }
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
      throw XML::SyntaxError(source.getPosition(), "Nesting of CDATA sections is not allowed.");
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
  std::set<std::string> attributeNames;
  while (source.more() && source.current() != '?' && source.current() != '/' && source.current() != '>') {
    std::string attributeName = parseName(source);
    if (!source.match("=")) {
      throw XML::SyntaxError(source.getPosition(), "Missing '=' between attribute name and value.");
    }
    if (attributeNames.contains(attributeName)) {
      throw XML::SyntaxError(source.getPosition(), "Attribute defined more than once within start tag.");
    }
    source.ignoreWS();
    XMLValue attributeValue = parseValue(source, *entityMapper);
    if (!validAttributeValue(attributeValue)) {
      throw XML::SyntaxError(source.getPosition(), "Attribute value contains invalid character '<', '\"', ''' or '&'.");
    }
    attributes.emplace_back(attributeName, attributeValue);
    attributeNames.insert(attributeName);
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
/// Parse any element content that is found.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <param name="xNode">Current element XNode.</param>
void XML_Impl::parseElementContent(ISource &source, XNode &xNode)
{
  XMLValue content{ parseCharacter(source) };
  if (content.isReference()) {
    if (content.isEntityReference()) { content = entityMapper->map(content); }
    auto xEntityReference = XNode::make<XEntityReference>(content);
    if (content.isEntityReference()) {
      // Does entity contain start tag ?
      // YES then XML into current element list
      if (content.getParsed().starts_with("<")) {
        processEntityReferenceXML(xNode, content);
        return;
      }
      // NO XML into entity elements list.
      else {
        processEntityReferenceXML(xEntityReference, content);
        resetWhiteSpace(xNode);
      }
    }
    xNode.addChildren(std::move(xEntityReference));
  } else {
    addContentToElementChildList(xNode, content.getParsed());
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
  if (source.match("<!--")) {
    xNode.addChildren(parseComment(source));
  } else if (source.match("<?")) {
    xNode.addChildren(parsePI(source));
  } else if (source.match("<![CDATA[")) {
    resetWhiteSpace(xNode);
    xNode.addChildren(parseCDATA(source));
  } else if (source.match("<")) {
    xNode.addChildren(parseElement(source, XRef<XElement>(xNode).getNamespaceList()));
    XElement &xNodeChildElement = XRef<XElement>(xNode.getChildren().back());
    if (auto pos = xNodeChildElement.name().find(':'); pos != std::string::npos) {
      if (!xNodeChildElement.isNameSpacePresent(xNodeChildElement.name().substr(0, pos))) {
        throw XML::SyntaxError(source.getPosition(), "Namespace used but not defined.");
      }
    }
  } else {
    if (source.match("</")) {
      throw XML::SyntaxError(source.getPosition(), "Missing closing tag.");
    } else if (source.match("]]>")) {
      throw XML::SyntaxError(source.getPosition(), "']]>' invalid in element content area.");
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
XNode XML_Impl::parseElement(ISource &source, const std::vector<XMLAttribute> &outerNamespaces, bool root)
{
  // Parse tag and attributes
  const std::string name{ parseTagName(source) };
  const std::vector<XMLAttribute> attributes{ parseAttributes(source) };
  // Add any new namespaces
  std::vector<XMLAttribute> namespaces{ outerNamespaces };
  for (const auto &attribute : attributes) {
    if (attribute.getName().starts_with("xmlns")) {
      namespaces.emplace_back((attribute.getName().size() > 5) ? attribute.getName().substr(6) : ":",
        XMLValue{ attribute.getUnparsed(), attribute.getValue() });
    }
  }
  // Create element XNode
  if (source.match(">")) {
    XNode xNode;
    // Normal element tag
    if (root) {
      xNode = XNode::make<XRoot>(name, attributes, namespaces);
    } else {
      xNode = XNode::make<XElement>(name, attributes, namespaces);
    }
    while (source.more() && !source.match("</")) { parseElementContents(source, xNode); }
    if (source.match(toUtf16(XRef<XElement>(xNode).name()) + STR(">"))) { return (xNode); }
  } else if (source.match("/>")) {
    // Self closing element tag
    return (XNode::make<XSelf>(name, attributes, namespaces));
  }
  throw XML::SyntaxError(source.getPosition(), "Missing closing tag.");
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
      version = parseDeclarationAttribute(source, "version", { "1.0", "2.0" });
    } else {
      throw XML::SyntaxError(source.getPosition(), "Version missing from declaration.");
    }
    if (source.match("encoding")) {
      encoding = parseDeclarationAttribute(source, "encoding", { "UTF-8", "UTF-16" }, true);
    }
    if (source.match("standalone")) { standalone = parseDeclarationAttribute(source, "standalone", { "yes", "no" }); }
    if (source.match("encoding")) {
      throw XML::SyntaxError(source.getPosition(), "Incorrect order for version, encoding and standalone attributes.");
    }
    if (!source.match("?>")) { throw XML::SyntaxError(source.getPosition(), "Declaration end tag not found."); }
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
    if (!parseCommentsPIAndWhiteSpace(source, xProlog)) {
      throw XML::SyntaxError(source.getPosition(), "Extra content at the end of document.");
    }
  }
}

/// <summary>
/// Parse XML DTD and return any XNode created for it.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <returns>Pointer to DTD XNode.</returns>
XNode XML_Impl::parseDTD(ISource &source)
{
  if (hasDTD) { throw XML::SyntaxError(source.getPosition(), "More than one DOCTYPE declaration."); }
  auto xNode = XNode::make<XDTD>(*entityMapper);
  DTD dtd{ xNode };
  dtd.parse(source);
  validator = std::make_unique<DTD_Validator>(xNode);
  hasDTD = true;
  return (xNode);
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
  xProlog.addChildren(parseDeclaration(source));
  while (source.more()) {
    if (source.match("<!DOCTYPE")) {
      xProlog.addChildren(parseDTD(source));
    } else if (parseCommentsPIAndWhiteSpace(source, xProlog)) {
      continue;
    } else if (source.current() == '<') {
      break;// --- Break out as potential root element detected ---
    } else {
      throw XML::SyntaxError(source.getPosition(), "Content detected before root element.");
    }
  }
  if (!source.match("<")) { throw XML::SyntaxError(source.getPosition(), "Missing root element."); }
  return (xProlog);
}

/// <summary>
/// Parse XML from source stream.
/// </summary>
XNode XML_Impl::parseXML(ISource &source)
{
  auto xProlog = parseProlog(source);
  xProlog.addChildren(parseElement(source, {}, true));
  parseTail(source, xProlog);
  return (xProlog);
}
}// namespace XML_Lib
