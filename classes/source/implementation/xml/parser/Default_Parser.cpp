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
#if defined(XML_LIB_ENABLE_DTD)
#include "DTD_Validator.hpp"
#endif

namespace XML_Lib {

/// <summary>
/// Add content Node to element's child list.
/// </summary>
/// <param name="xNode">Current element Node.</param>
/// <param name="content">Content to add to new content Node (XMLNodeContent).</param>
void addContentToElementChildList(Node &xNode, const std::string_view &content)
{
  // Make sure there is a content Node to receive characters
  if (xNode.getChildren().empty() || !isA<Content>(xNode.getChildren().back())) {
    bool isWhiteSpace = true;
    if (!xNode.getChildren().empty()) {
      if (isA<CDATA>(xNode.getChildren().back()) || isA<EntityReference>(xNode.getChildren().back())) {
        isWhiteSpace = false;
      }
    }
    xNode.addChild(Node::make<Content>("", isWhiteSpace));
  }
  auto &xmlContent = NRef<Content>(xNode.getChildren().back());
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
/// Parse entity reference as XML and add Nodes produced to the current Node.
/// </summary>
/// <param name="xNode">Current element Node.</param>
/// <param name="entityReference">Entity reference to be parsed for XML.</param>
/// <param name="entityMapper">Entity mapper interface object.</param>
void Default_Parser::parseEntityReferenceXML(Node &xNode, const XMLValue &entityReference, IEntityMapper &entityMapper)
{
  auto xElement = Node::make<Element>();
  BufferSource entitySource(entityReference.getParsed());
  // Parse entity XML
  while (entitySource.more()) { parseElementInternal(entitySource, xNode, entityMapper); }
  // Place into Node (element) child list
  for (auto &child : xElement.getChildren()) { xNode.addChild(child); }
}

/// <summary>
/// Parse any comments, PI or whitespace in prolog/epilog of the XML file.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <param name="xProlog">XML prolog Node.</param>
/// <returns>True then items parsed.</returns>
bool Default_Parser::tryParseCommentOrPI(ISource &source, Node &xNode)
{
  if (match(source, "<!--")) { xNode.addChild(parseComment(source)); return true; }
  if (match(source, "<?"))   { xNode.addChild(parsePI(source));      return true; }
  return false;
}

bool Default_Parser::parseCommentsPIAndWhiteSpace(ISource &source, Node &xProlog)
{
  if (tryParseCommentOrPI(source, xProlog)) { return true; }
  if (isWS(source)) {
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
/// Parse the declaration attribute and validate its value.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <param name="name">Attribute name string.</param>
/// <param name="values">Set of valid attribute values.</param>
/// <returns>Valid attribute value.</returns>
std::string Default_Parser::parseDeclarationAttribute(ISource &source,
  const std::string_view &name,
  std::span<const std::string_view> values)
{
  std::string value;
  ignoreWS(source);
  if (match(source, "=")) {
    ignoreWS(source);
    value = parseValue(source).getParsed();
    if (name == "encoding") { value = toUpperString(value); }
    if (!std::ranges::any_of(values, [&](const std::string_view sv) { return sv == value; })) {
      XML_LIB_THROW(SyntaxError("Unsupported XML " + std::string(name) + " value '" + value + "' specified."));
    }
  } else {
    XML_LIB_THROW(SyntaxError(source.getPosition(), "Missing '=' after " + std::string(name) + "."));
  }
  return value;
}

/// <summary>
/// Parse a XML comment, create a XComment for it and add to the list
/// of elements for the current XElement.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <returns>Pointer to comment Node.</returns>
Node Default_Parser::parseComment(ISource &source)
{
  String comment;
  comment.reserve(64);
  while (source.more() && !match(source, "--")) {
    comment += source.current();
    source.next();
  }
  if (!match(source, ">")) { XML_LIB_THROW(SyntaxError(source.getPosition(), "Missing closing '>' for comment line.")); }
  return Node::make<Comment>(toUtf8(comment));
}

/// <summary>
/// Parse an XML process instruction, create an XPI for it and add it to
/// the list of elements under the current XElement.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <returns>Pointer to PI Node.</returns>
Node Default_Parser::parsePI(ISource &source)
{
  std::string name{ parseName(source) };
  // Check not a declaration
  if (name == "xml") {
    XML_LIB_THROW(SyntaxError(source.getPosition(), "Declaration allowed only at the start of the document."));
  }
  String parameters;
  parameters.reserve(64);
  while (source.more() && !match(source, "?>")) {
    parameters += source.current();
    source.next();
  }
  return Node::make<PI>(name, toUtf8(parameters));
}

/// <summary>
/// Parse an XML CDATA section, create an XCDATA for it and add it to
/// the list of elements under the current XElement.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <returns>Pointer to CDATA Node.</returns>
Node Default_Parser::parseCDATA(ISource &source)
{
  String cdata;
  cdata.reserve(128);
  while (source.more() && !match(source, "]]>") ) {
    if (match(source, "<![CDATA[")) {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Nesting of CDATA sections is not allowed."));
    }
    cdata += source.current();
    source.next();
  }
  return Node::make<CDATA>(toUtf8(cdata));
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
  std::vector<XMLAttribute> attributes{};
  attributes.reserve(8);
  while (source.more() && source.current() != '/' && source.current() != '>') {
    std::string attributeName{ parseName(source) };
    if (!match(source, "=")) {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Missing '=' between attribute name and value."));
    }
    ignoreWS(source);
    XMLValue attributeValue = parseValue(source, entityMapper);
    if (!validAttributeValue(attributeValue.getParsed(), attributeValue.getQuote())) {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Attribute value contains invalid character '<', '\"', ''' or '&'."));
    }
    if (XMLAttribute::contains(attributes, attributeName)) {
      XML_LIB_THROW(SyntaxError("Attribute '" + attributeName + "' defined more than once within start tag."));
    }
    attributes.emplace_back(attributeName, attributeValue);
  }
  return attributes;
}

/// <summary>
/// Parse white spaces and add to the current Nodes child list.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <param name="xNode">Current Node.</param>
void Default_Parser::parseWhiteSpaceToContent(ISource &source, Node &xNode)
{
  String whiteSpace;
  whiteSpace.reserve(32);
  while (source.more() && isWS(source)) {
    whiteSpace += source.current();
    source.next();
  }
  addContentToElementChildList(xNode, toUtf8(whiteSpace));
}

/// <summary>
/// Mark the trailing Content child of xNode as non-whitespace, if one exists.
/// </summary>
/// <param name="xNode">Current element Node.</param>
static void markTrailingContentNonWhitespace(Node &xNode)
{
  if (!xNode.getChildren().empty()) {
    if (isA<Content>(xNode.getChildren().back())) {
      NRef<Content>(xNode.getChildren().back()).setIsWhiteSpace(false);
    }
  }
}

/// <summary>
/// Append a parsed character value to xNode as entity reference or plain content.
/// </summary>
/// <param name="xNode">Current element Node.</param>
/// <param name="value">Parsed character value.</param>
/// <param name="entityMapper">Entity mapper interface object.</param>
void Default_Parser::appendEntityOrContent(Node &xNode, const XMLValue &value, IEntityMapper &entityMapper)
{
  if (value.isReference()) {
    XMLValue content = value;
    if (content.isEntityReference()) { content = entityMapper.map(content); }
    auto xEntityReference = Node::make<EntityReference>(content);
    if (content.isEntityReference()) {
      if (entityExpansionDepth >= maxEntityExpansionDepth) {
        XML_LIB_THROW(SyntaxError("Entity expansion depth limit exceeded."));
      }
      ++entityExpansionDepth;
      // Does entity contain start tag ?
      // YES then XML into current element list
      if (content.getParsed().starts_with("<")) {
        parseEntityReferenceXML(xNode, content, entityMapper);
        --entityExpansionDepth;
        return;
      }
      // NO XML into entity elements list.
      parseEntityReferenceXML(xEntityReference, content, entityMapper);
      --entityExpansionDepth;
      markTrailingContentNonWhitespace(xNode);
    }
    xNode.addChild(std::move(xEntityReference));
  } else {
    addContentToElementChildList(xNode, value.getParsed());
  }
}

/// <summary>
/// Parse any content found inside an element.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <param name="xNode">Current element Node.</param>
/// <param name="entityMapper">Entity mapper interface object.</param>
void Default_Parser::parseContent(ISource &source, Node &xNode, IEntityMapper &entityMapper)
{
  appendEntityOrContent(xNode, parseCharacter(source), entityMapper);
}

/// <summary>
/// Parse element internal area, generating any Node(s) and adding them
/// to the child list of the current XElement. This can be anything from
/// comments, program instructions, CDATA, nested elements or even content.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <param name="xNode">Current element Node.</param>
/// <param name="entityMapper">Entity mapper interface object.</param>
void Default_Parser::parseElementInternal(ISource &source, Node &xNode, IEntityMapper &entityMapper)
{
  if (tryParseCommentOrPI(source, xNode)) {
    // comment or PI handled
  } else if (match(source, "<![CDATA[")) {
    markTrailingContentNonWhitespace(xNode);
    xNode.addChild(parseCDATA(source));
  } else if (match(source, "<")) {
    xNode.addChild(parseElement(source, NRef<Element>(xNode).getNameSpaces(), entityMapper));
    const Element &xNodeChildElement = NRef<Element>(xNode.getChildren().back());
    if (const auto pos = xNodeChildElement.name().find(':'); pos != std::string::npos) {
      if (!xNodeChildElement.hasNameSpace(xNodeChildElement.name().substr(0, pos))) {
        XML_LIB_THROW(SyntaxError(source.getPosition(), "Namespace used but not defined."));
      }
    }
    for (const auto &attr : xNodeChildElement.getAttributes()) {
      if (!attr.getName().starts_with("xmlns")) {
        if (const auto attrPos = attr.getName().find(':'); attrPos != std::string::npos) {
          if (!xNodeChildElement.hasNameSpace(attr.getName().substr(0, attrPos))) {
            XML_LIB_THROW(SyntaxError(
              source.getPosition(), "Namespace used but not defined in attribute '" + attr.getName() + "'."));
          }
        }
      }
    }
  } else {
    if (match(source, "</")) { XML_LIB_THROW(SyntaxError(source.getPosition(), "Missing closing tag.")); }
    if (match(source, "]]>")) { XML_LIB_THROW(SyntaxError(source.getPosition(), "']]>' invalid in element content area.")); }
    parseContent(source, xNode, entityMapper);
  }
}

/// <summary>
/// Parse the current XML element found.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <param name="namespaces">Current list of outer namespaces.</param>
/// <param name="entityMapper">Entity mapper interface object.</param>
/// <returns>Pointer to element Node.</returns>
Node Default_Parser::parseElement(ISource &source,
  std::span<const XMLAttribute> namespaces,
  IEntityMapper &entityMapper)
{
  // Parse tag and attributes
  const std::string name{ parseTagName(source) };
  const std::vector attributes{ parseAttributes(source, entityMapper) };
  // Create element Node
  if (Node xNode; match(source, ">")) {
    // Normal element tag
    if (!hasRoot) {
      xNode = Node::make<Root>(name, attributes, namespaces);
      hasRoot = true;
    } else {
      xNode = Node::make<Element>(name, attributes, namespaces);
    }
    xNode.reserveChildren(8);
    if (elementNestingDepth >= maxElementNestingDepth) {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Maximum element nesting depth exceeded."));
    }
    ++elementNestingDepth;
    while (source.more() && !match(source, "</")) { parseElementInternal(source, xNode, entityMapper); }
    --elementNestingDepth;
    if (match(source, toUtf16(NRef<Element>(xNode).name()) + u">")) { return xNode; }
  } else if (match(source, "/>")) {
    // Self-closing element tag
    return Node::make<Self>(name, attributes, namespaces);
  }
  XML_LIB_THROW(SyntaxError(source.getPosition(), "Missing closing tag."));
}

/// <summary>
/// Parse XML declaration and return Node for it.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <returns>Pointer to declaration Node.</returns>
Node Default_Parser::parseDeclaration(ISource &source)
{
  std::string version{ "1.0" };
  std::string encoding{ "UTF-8" };
  std::string standalone{ "no" };
  ignoreWS(source);
  if (match(source, "<?xml")) {
    ignoreWS(source);
    if (match(source, "version")) {
      static constexpr std::array<std::string_view, 2> kVersions{ "1.0", "1.1" };
      version = parseDeclarationAttribute(source, "version", kVersions);
    } else {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Version missing from declaration."));
    }
    if (match(source, "encoding")) {
      // XML 1.0 allows a wide range of encodings, not just UTF-8 and UTF-16
      static constexpr std::array<std::string_view, 22> kEncodings{ "UTF-8",
        "UTF-16",
        "ISO-10646-UCS-2",
        "ISO-10646-UCS-4",
        "ISO-8859-1",
        "ISO-8859-2",
        "ISO-8859-3",
        "ISO-8859-4",
        "ISO-8859-5",
        "ISO-8859-6",
        "ISO-8859-7",
        "ISO-8859-8",
        "ISO-8859-9",
        "ISO-2022-JP",
        "ISO-2022-JP-2",
        "ISO-2022-KR",
        "ISO-2022-CN",
        "EUC-JP",
        "EUC-KR",
        "EUC-CN",
        "SHIFT_JIS",
        "US-ASCII" };
      encoding = parseDeclarationAttribute(source, "encoding", kEncodings);
    }
    static constexpr std::array<std::string_view, 2> kStandalone{ "yes", "no" };
    if (match(source, "standalone")) { standalone = parseDeclarationAttribute(source, "standalone", kStandalone); }
    if (match(source, "encoding")) {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Incorrect order for version, encoding and standalone attributes."));
    }
    if (!match(source, "?>")) { XML_LIB_THROW(SyntaxError(source.getPosition(), "Declaration end tag not found.")); }
  }
  return Node::make<Declaration>(version, encoding, standalone);
}

/// <summary>
/// Parse any XML tail that is present. This can include comments, PI and white space.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <param name="xProlog">Prolog Node.</param>
void Default_Parser::parseEpilog(ISource &source, Node &xProlog)
{
  while (source.more()) {
    if (!parseCommentsPIAndWhiteSpace(source, xProlog)) {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Extra content at the end of document."));
    }
  }
}

/// <summary>
/// Parse XML DTD and return any Node created for it.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <param name="entityMapper">Entity mapper interface object.</param>
/// <returns>Pointer to DTD Node.</returns>
#if defined(XML_LIB_ENABLE_DTD)
Node Default_Parser::parseDTD(ISource &source, IEntityMapper &entityMapper)
{
  if (validator != nullptr) { XML_LIB_THROW(SyntaxError(source.getPosition(), "More than one DOCTYPE declaration.")); }
  auto xNode = Node::make<DTD>(entityMapper);
  validator = std::make_unique<DTD_Validator>(xNode);
  validator->parse(source);
  return xNode;
}
#else
Node Default_Parser::parseDTD([[maybe_unused]] ISource &source, [[maybe_unused]] IEntityMapper &entityMapper)
{
  XML_LIB_THROW(SyntaxError(source.getPosition(), "DTD support disabled in this build."));
}
#endif
/// <summary>
/// Parse XML prolog and create the necessary element Nodes for it. Valid
/// parts of the prolog include declaration (first line if present),
/// processing instructions, comments, whitespace and a Document Type Declaration (DTD).
/// </summary>
/// <param name="source">XML source stream.</param>
/// <param name="entityMapper">Entity mapper interface object.</param>
/// <returns>Pointer to prolog Node.</returns>
Node Default_Parser::parseProlog(ISource &source, IEntityMapper &entityMapper)
{
  auto xProlog = Node::make<Prolog>();
  xProlog.addChild(parseDeclaration(source));
  while (source.more()) {
#if defined(XML_LIB_ENABLE_DTD)
    if (match(source, "<!DOCTYPE")) {
      xProlog.addChild(parseDTD(source, entityMapper));
    } else
#endif
    if (parseCommentsPIAndWhiteSpace(source, xProlog)) {
    } else if (source.current() == '<') {
      break;// --- Break out as potential root element detected ---
    } else {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Content detected before root element."));
    }
  }
  return xProlog;
}

/// <summary>
/// Parse XML read from source stream into internal object generating an exception
/// if a syntax error in the XML is found (not well-formed).
/// </summary>
/// <param name="source">XML source stream.</param>
/// <returns>Prolog Node.</returns>
Node Default_Parser::parse(ISource &source, const ParseOptions &options)
{
  parseOptions = options;
  entityExpansionDepth = 0;
  maxEntityExpansionDepth = options.maxEntityExpansionDepth;
  elementNestingDepth = 0;
  maxElementNestingDepth = options.maxNestingDepth;
  entityMapper.setExternalEntityPolicy(options.allowExternalEntities, options.entityResolver);
  XML_Arena::ScopedCurrentArena scopedCurrentArena(arena);
  XML_Arena::ScopedDefaultResource scopedDefaultResource(arena);
  // Reset XML before next parse
  entityMapper.reset();
  hasRoot = false;
  validator.reset();
  // Handle prolog
  Node xmlRoot = parseProlog(source, entityMapper);
  // Handle main body
  if (match(source, "<")) {
    xmlRoot.addChild(parseElement(source, {}, entityMapper));
  } else {
    XML_LIB_THROW(SyntaxError(source.getPosition(), "Missing root element."));
  }
  // Handle any epilog
  parseEpilog(source, xmlRoot);
  return xmlRoot;
}
/// <summary>
/// Validate XML against parsed DTD.
/// </summary>
/// <param name="xProlog">Prolog Node</param>
void Default_Parser::validate(Node &xProlog)
{
  if (validator != nullptr) {
    validator->validate(xProlog);
  } else {
    XML_LIB_THROW(Error("No DTD specified for validation."));
  }
}
/// <summary>
/// Parser can validate XML.
/// </summary>
/// <returns>Returns true if parser can validate XML.</returns>
bool Default_Parser::canValidate() { return validator != nullptr; }
}// namespace XML_Lib
