//
// Class: XML_Parser
//
// Description: XML parser code. All parsings of characters take place having
// converted the characters to UTF-16 to make the process easier (any data once
// parsed is stored in UTF-8 strings).
//
// Dependencies: C++20 - Language standard features used.
//

#include "Default_Parser.hpp"
#include "DTD_Validator.hpp"

namespace XML_Lib {

/// <summary>
/// Add content XNode to element's child list.
/// </summary>
/// <param name="xNode">Current element XNode.</param>
/// <param name="content">Content to add to new content XNode (XMLNodeContent).</param>
void addContentToElementChildList(XNode &xNode, const std::string &content)
{
  // Make sure there is a content XNode to receive characters
  if (xNode.getChildren().empty() || !isA<Content>(xNode.getChildren().back())) {
    bool isWhiteSpace = true;
    if (!xNode.getChildren().empty()) {
      if (isA<CDATA>(xNode.getChildren().back()) || isA<EntityReference>(xNode.getChildren().back())) { isWhiteSpace = false; }
    }
    xNode.addChild(XNode::make<Content>("", isWhiteSpace));
  }
  auto &xmlContent = XRef<Content>(xNode.getChildren().back());
  if (xmlContent.isWhiteSpace()) {
    if (std::ranges::all_of(content, [](const char ch) { return std::iswspace(ch); })) {
      xmlContent.setIsWhiteSpace(true);
    } else {
      xmlContent.setIsWhiteSpace(false);
    }
  }
  xmlContent.addContent(content);
}

/// <summary>
/// Parse entity reference as XML and add XNodes produced to the current XNode.
/// </summary>
/// <param name="xNode">Current element XNode.</param>
/// <param name="entityReference">Entity reference to be parsed for XML.</param>
/// <param name="entityMapper">Entity mapper interface object.</param>
void Default_Parser::parseEntityReferenceXML(XNode &xNode, const XMLValue &entityReference, IEntityMapper & entityMapper)
{
  auto xElement = XNode::make<Element>();
  BufferSource entitySource(entityReference.getParsed());
  // Parse entity XML
  while (entitySource.more()) { parseElementInternal(entitySource, xNode, entityMapper); }
  // Place into XNode (element) child list
  for (auto &child : xElement.getChildren()) { xNode.addChild(child); }
}

/// <summary>
/// Parse any comments, PI or whitespace in prolog/epilog of the XML file.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <param name="xProlog">XML prolog XNode.</param>
/// <returns>True then items parsed.</returns>
bool Default_Parser::parseCommentsPIAndWhiteSpace(ISource &source, XNode &xProlog)
{
  if (source.match("<!--")) {
    xProlog.addChild(parseComment(source));
    return true;
  }
  if (source.match("<?")) {
    xProlog.addChild(parsePI(source));
    return true;
  }
  if (source.isWS()) {
    parseWhiteSpaceToContent(source, xProlog);
    return true;
  }
  return false;
}

/// <summary>
/// Parse a element tag name and set its value in current XElement.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <returns>Element tag name.</returns>
std::string Default_Parser::parseTagName(ISource &source) { return parseName(source); }

/// <summary>
/// Parse declaration attribute and validate its value.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <param name="name">Attribute name string.</param>
/// <param name="values">Set of valid attribute values.</param>
/// <returns>Valid attribute value.</returns>
std::string
  Default_Parser::parseDeclarationAttribute(ISource &source, const std::string &name, const std::set<std::string> &values)
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
  return value;
}

/// <summary>
/// Parse a XML comment, create a XComment for it and add to the list
/// of elements for the current XElement.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <returns>Pointer to comment XNode.</returns>
XNode Default_Parser::parseComment(ISource &source)
{
  std::string comment;
  while (source.more() && !source.match("--")) {
    comment += toUtf8(source.current());
    source.next();
  }
  if (!source.match(">")) { throw SyntaxError(source.getPosition(), "Missing closing '>' for comment line."); }
  return XNode::make<Comment>(comment);
}

/// <summary>
/// Parse an XML process instruction, create an XPI for it and add it to
/// the list of elements under the current XElement.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <returns>Pointer to PI XNode.</returns>
XNode Default_Parser::parsePI(ISource &source)
{
  std::string name{ parseName(source) };
  // Check not a declaration
  if (name == "xml") {
    throw SyntaxError(source.getPosition(), "Declaration allowed only at the start of the document.");
  }
  std::string parameters;
  while (source.more() && !source.match("?>")) {
    parameters += toUtf8(source.current());
    source.next();
  }
  return XNode::make<PI>(name, parameters);
}

/// <summary>
/// Parse an XML CDATA section, create an XCDATA for it and add it to
/// the list of elements under the current XElement.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <returns>Pointer to CDATA XNode.</returns>
XNode Default_Parser::parseCDATA(ISource &source)
{
  std::string cdata;
  while (source.more() && !source.match("]]>")) {
    if (source.match("<![CDATA[")) {
      throw SyntaxError(source.getPosition(), "Nesting of CDATA sections is not allowed.");
    }
    cdata += toUtf8(source.current());
    source.next();
  }
  return XNode::make<CDATA>(cdata);
}

/// <summary>
/// Parse the list of attributes (name/value pairs) that exist in a tag and add them to
/// the list of attributes associated with the current XElement.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <param name="entityMapper">Entity mapper interface object.</param>
/// <returns>XML element attribute list.</returns>
std::vector<XMLAttribute> Default_Parser::parseAttributes(ISource &source, IEntityMapper &entityMapper)
{
  std::vector<XMLAttribute> attributes {};
  while (source.more() && source.current() != '/' && source.current() != '>') {
    std::string attributeName{ parseName(source) };
    if (!source.match("=")) {
      throw SyntaxError(source.getPosition(), "Missing '=' between attribute name and value.");
    }
    source.ignoreWS();
    XMLValue attributeValue = parseValue(source, entityMapper);
    if (!validAttributeValue(attributeValue.getParsed(), attributeValue.getQuote())) {
      throw SyntaxError(source.getPosition(), "Attribute value contains invalid character '<', '\"', ''' or '&'.");
    }
    if (XMLAttribute::contains(attributes, attributeName)) {
      throw SyntaxError("Attribute '" + attributeName + "' defined more than once within start tag.");
    }
    attributes.emplace_back(attributeName, attributeValue);
  }
  return attributes;
}

/// <summary>
/// Parse white spaces and add to the current XNodes child list.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <param name="xNode">Current XNode.</param>
void Default_Parser::parseWhiteSpaceToContent(ISource &source, XNode &xNode)
{
  std::string whiteSpace;
  while (source.more() && source.isWS()) {
    whiteSpace += toUtf8(source.current());
    source.next();
  }
  addContentToElementChildList(xNode, whiteSpace);
}

/// <summary>
/// Parse any content found inside an element.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <param name="xNode">Current element XNode.</param>
/// <param name="entityMapper">Entity mapper interface object.</param>
void Default_Parser::parseContent(ISource &source, XNode &xNode, IEntityMapper &entityMapper)
{
  if (XMLValue content{ parseCharacter(source) }; content.isReference()) {
    if (content.isEntityReference()) { content = entityMapper.map(content); }
    auto xEntityReference = XNode::make<EntityReference>(content);
    if (content.isEntityReference()) {
      // Does entity contain start tag ?
      // YES then XML into current element list
      if (content.getParsed().starts_with("<")) {
        parseEntityReferenceXML(xNode, content,entityMapper);
        return;
      }
      // NO XML into entity elements list.
      parseEntityReferenceXML(xEntityReference, content, entityMapper);
      if (!xNode.getChildren().empty()) {
        if (isA<Content>(xNode.getChildren().back())) { XRef<Content>(xNode.getChildren().back()).setIsWhiteSpace(false); }
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
/// <param name="source">XML source stream.</param>
/// <param name="xNode">Current element XNode.</param>
/// <param name="entityMapper">Entity mapper interface object.</param>
void Default_Parser::parseElementInternal(ISource &source, XNode &xNode, IEntityMapper & entityMapper)
{
  if (source.match("<!--")) {
    xNode.addChild(parseComment(source));
  } else if (source.match("<?")) {
    xNode.addChild(parsePI(source));
  } else if (source.match("<![CDATA[")) {
    if (!xNode.getChildren().empty()) {
      if (isA<Content>(xNode.getChildren().back())) { XRef<Content>(xNode.getChildren().back()).setIsWhiteSpace(false); }
    }
    xNode.addChild(parseCDATA(source));
  } else if (source.match("<")) {
    xNode.addChild(parseElement(source, XRef<Element>(xNode).getNameSpaces(), entityMapper));
    const Element &xNodeChildElement = XRef<Element>(xNode.getChildren().back());
    if (const auto pos = xNodeChildElement.name().find(':'); pos != std::string::npos) {
      if (!xNodeChildElement.hasNameSpace(xNodeChildElement.name().substr(0, pos))) {
        throw SyntaxError(source.getPosition(), "Namespace used but not defined.");
      }
    }
  } else {
    if (source.match("</")) {
      throw SyntaxError(source.getPosition(), "Missing closing tag.");
    }
    if (source.match("]]>")) {
      throw SyntaxError(source.getPosition(), "']]>' invalid in element content area.");
    }
    parseContent(source, xNode, entityMapper);
  }
}

/// <summary>
/// Parse the current XML element found.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <param name="namespaces">Current list of outer namespaces.</param>
/// <param name="entityMapper">Entity mapper interface object.</param>
/// <returns>Pointer to element XNode.</returns>
XNode Default_Parser::parseElement(ISource &source, const std::vector<XMLAttribute> &namespaces, IEntityMapper & entityMapper)
{
  // Parse tag and attributes
  const std::string name{ parseTagName(source) };
  const std::vector attributes{ parseAttributes(source, entityMapper) };
  // Create element XNode
  if (XNode xNode; source.match(">")) {
    // Normal element tag
    if (!hasRoot) {
      xNode = XNode::make<Root>(name, attributes, namespaces);
      hasRoot = true;
    } else {
      xNode = XNode::make<Element>(name, attributes, namespaces);
    }
    while (source.more() && !source.match("</")) { parseElementInternal(source, xNode, entityMapper); }
    if (source.match(toUtf16(XRef<Element>(xNode).name()) + u">")) { return xNode; }
  } else if (source.match("/>")) {
    // Self-closing element tag
    return XNode::make<Self>(name, attributes, namespaces);
  }
  throw SyntaxError(source.getPosition(), "Missing closing tag.");
}

/// <summary>
/// Parse XML declaration and return XNode for it.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <returns>Pointer to declaration XNode.</returns>
XNode Default_Parser::parseDeclaration(ISource &source)
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
  return XNode::make<Declaration>(version, encoding, standalone);
}

/// <summary>
/// Parse any XML tail that is present. This can include comments, PI and white space.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <param name="xProlog">Prolog XNode.</param>
void Default_Parser::parseEpilog(ISource &source, XNode &xProlog)
{
  while (source.more()) {
    if (!parseCommentsPIAndWhiteSpace(source, xProlog)) {
      throw SyntaxError(source.getPosition(), "Extra content at the end of document.");
    }
  }
}

/// <summary>
/// Parse XML DTD and return any XNode created for it.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <param name="entityMapper">Entity mapper interface object.</param>
/// <returns>Pointer to DTD XNode.</returns>
XNode Default_Parser::parseDTD(ISource &source, IEntityMapper &entityMapper)
{
  if (validator != nullptr) { throw SyntaxError(source.getPosition(), "More than one DOCTYPE declaration."); }
  auto xNode = XNode::make<DTD>(entityMapper);
  validator = std::make_unique<DTD_Validator>(xNode);
  validator->parse(source);
  return xNode;
}
/// <summary>
/// Parse XML prolog and create the necessary element XNodes for it. Valid
/// parts of the prolog include declaration (first line if present),
/// processing instructions, comments, whitespace and a Document Type Declaration (DTD).
/// </summary>
/// <param name="source">XML source stream.</param>
/// <param name="entityMapper">Entity mapper interface object.</param>
/// <returns>Pointer to prolog XNode.</returns>
XNode Default_Parser::parseProlog(ISource &source, IEntityMapper &entityMapper)
{
  auto xProlog = XNode::make<Prolog>();
  xProlog.addChild(parseDeclaration(source));
  while (source.more()) {
    if (source.match("<!DOCTYPE")) {
      xProlog.addChild(parseDTD(source, entityMapper));
    } else if (parseCommentsPIAndWhiteSpace(source, xProlog)) {
    } else if (source.current() == '<') {
      break;// --- Break out as potential root element detected ---
    } else {
      throw SyntaxError(source.getPosition(), "Content detected before root element.");
    }
  }
  return xProlog;
}

/// <summary>
/// Parse XML read from source stream into internal object generating an exception
/// if a syntax error in the XML is found (not well-formed).
/// </summary>
/// <param name="source">XML source stream.</param>
/// <returns>Prolog XNode.</returns>
XNode Default_Parser::parse(ISource &source)
{
  // Reset XML before next parse
  entityMapper.reset();
  hasRoot = false;
  validator.reset();
  // Handle prolog
  XNode xmlRoot = parseProlog(source, entityMapper);
  // Handle main body
  if (source.match("<")) {
    xmlRoot.addChild(parseElement(source, {}, entityMapper));
  } else {
    throw SyntaxError(source.getPosition(), "Missing root element.");
  }
  // Handle any epilog
  parseEpilog(source, xmlRoot);
  return xmlRoot;
}
/// <summary>
/// Validate XML against parsed DTD.
/// </summary>
/// <param name="xProlog">Prolog XNode</param>
void Default_Parser::validate(XNode &xProlog)
{
  if (validator != nullptr) {
    validator->validate(xProlog);
  } else {
    throw Error("No DTD specified for validation.");
  }
}
/// <summary>
/// Parser can validate XML.
/// </summary>
/// <returns>Returns true if parser can validate XML.</returns>
bool Default_Parser::canValidate() { return validator != nullptr; }
}// namespace XML_Lib
