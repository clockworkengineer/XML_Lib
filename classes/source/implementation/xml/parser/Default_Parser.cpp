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
#include "common/XML_ParseHelpers.hpp"
#include "implementation/io/XML_FileIO.hpp"
#include <array>
#include <filesystem>
#include <set>
#if defined(XML_LIB_ENABLE_DTD)
#include "DTD_Validator.hpp"
#include "implementation/io/XML_FileSource.hpp"
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
  const std::size_t newSize = xmlContent.getContents().size() + content.size();
  Default_Parser::ensureTextNodeSizeWithinLimit(newSize);
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
void Default_Parser::parseEntityReferenceXML(Node &xNode, const XMLValue &entityReference, IEntityMapper &entityMapper, bool isExternal, std::span<const XMLAttribute> inheritedNamespaces)
{
  if (entityReference.getParsed().empty()) {
    return;
  }
  BufferSource entitySource(entityReference.getParsed());
  if (isExternal) {
    parseTextDecl(entitySource);
  }
  // Parse entity XML
  while (entitySource.more()) { parseElementInternal(entitySource, xNode, entityMapper, inheritedNamespaces); }
}

/// <summary>
/// Parse any comments, PI or whitespace in prolog/epilog of the XML file.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <param name="xProlog">XML prolog Node.</param>
/// <returns>True then items parsed.</returns>
bool Default_Parser::tryParseCommentOrPI(ISource &source, Node &xNode)
{
  if (match(source, "<!--")) {
    xNode.addChild(parseComment(source));
    return true;
  }
  if (match(source, "<?")) {
    xNode.addChild(parsePI(source));
    return true;
  }
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
std::string Default_Parser::parseTagName(ISource &source)
{
  const String name = readName(source);
  if (!validName(name)) {
    XML_LIB_THROW(SyntaxError(source.getPosition(), "Invalid name '" + toUtf8(name) + "' encountered."));
  }
  const std::string lower = toLowerString(toUtf8(name));
  if (lower.starts_with("xml")) {
    if (!lower.starts_with("xmlns") && !lower.starts_with("xml-") && lower != "xml" && !lower.starts_with("xml:")) {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Invalid name '" + toUtf8(name) + "' encountered."));
    }
  }
  return toUtf8(name);
}

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
    bool valid = std::ranges::any_of(values, [&](const std::string_view sv) { return sv == value; });
    if (!valid && allowFuture1xVersionsMode && name == "version" && value.starts_with("1.") && value.size() > 2
        && std::all_of(value.begin() + 2, value.end(), [](char c) { return std::isdigit(static_cast<unsigned char>(c)); })) {
      valid = true;
    }
    if (!valid) {
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
    if (!validChar(source.current())) {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Invalid character in comment."));
    }
    comment += source.current();
    source.next();
  }
  if (!match(source, ">")) {
    XML_LIB_THROW(SyntaxError(source.getPosition(), "Missing closing '>' for comment line."));
  }
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
  if (Default_Parser::isNamespacesEnabled() && name.find(':') != std::string::npos) {
    XML_LIB_THROW(SyntaxError(source.getPosition(), "Colons are not allowed in processing instruction targets under XML Namespaces."));
  }
  // Check not a declaration (xml in any case combination)
  std::string lowerName = name;
  for (char &c : lowerName) { c = static_cast<char>(std::tolower(static_cast<unsigned char>(c))); }
  if (lowerName == "xml") {
    XML_LIB_THROW(SyntaxError(source.getPosition(), "Declaration allowed only at the start of the document."));
  }
  String parameters;
  parameters.reserve(64);
  bool closed = false;
  if (match(source, "?>")) {
    closed = true;
  } else if (!isWS(source)) {
    XML_LIB_THROW(SyntaxError(source.getPosition(), "Missing whitespace after processing instruction target."));
  } else {
    source.next();
    while (source.more()) {
      if (match(source, "?>")) {
        closed = true;
        break;
      }
      if (!validChar(source.current())) {
        XML_LIB_THROW(SyntaxError(source.getPosition(), "Invalid character in processing instruction."));
      }
      parameters += source.current();
      source.next();
    }
  }
  if (!closed) {
    XML_LIB_THROW(SyntaxError(source.getPosition(), "Unclosed processing instruction."));
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
  bool closed = false;
  while (source.more()) {
    if (match(source, "]]>")) {
      closed = true;
      break;
    }
    if (match(source, "<![CDATA[")) {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Nesting of CDATA sections is not allowed."));
    }
    if (!validChar(source.current())) {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Invalid character in CDATA section."));
    }
    cdata += source.current();
    source.next();
  }
  if (!closed) {
    XML_LIB_THROW(SyntaxError(source.getPosition(), "Unclosed CDATA section."));
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
    ignoreWS(source);
    if (!match(source, "=")) {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Missing '=' between attribute name and value."));
    }
    ignoreWS(source);
    XMLValue attributeValue = parseValue(source, entityMapper);
    ensureTextNodeSizeWithinLimit(attributeValue.getParsed().size());
    if (!validAttributeValue(attributeValue.getUnparsed(), attributeValue.getQuote()) ||
        attributeValue.getParsed().find('<') != std::string::npos) {
      XML_LIB_THROW(
        SyntaxError(source.getPosition(), "Attribute value contains invalid character '<', '\"', ''' or '&'."));
    }
    if (XMLAttribute::contains(attributes, attributeName)) {
      XML_LIB_THROW(SyntaxError("Attribute '" + attributeName + "' defined more than once within start tag."));
    }
    attributes.emplace_back(attributeName, attributeValue);
    ++currentTotalAttributeCount;
    if (currentTotalAttributeCount > maxTotalAttributeCount) {
      XML_LIB_THROW(SyntaxError("Maximum total attribute count exceeded."));
    }
    if (attributes.size() > maxAttributeCount) { XML_LIB_THROW(SyntaxError("Maximum attribute count exceeded.")); }
    if (source.more() && source.current() != '/' && source.current() != '>') {
      if (!isWS(source)) {
        XML_LIB_THROW(SyntaxError(source.getPosition(), "Missing whitespace after attribute value."));
      }
      ignoreWS(source);
    }
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
    if (isA<Content>(xNode.getChildren().back())) { NRef<Content>(xNode.getChildren().back()).setIsWhiteSpace(false); }
  }
}

/// <summary>
/// Append a parsed character value to xNode as entity reference or plain content.
/// </summary>
/// <param name="xNode">Current element Node.</param>
/// <param name="value">Parsed character value.</param>
/// <param name="entityMapper">Entity mapper interface object.</param>
void Default_Parser::appendEntityOrContent(Node &xNode, const XMLValue &value, IEntityMapper &entityMapper, std::span<const XMLAttribute> inheritedNamespaces)
{
  if (value.isReference()) {
    XMLValue content = value;
    if (content.isEntityReference()) {
      if (isStandaloneDocument && entityMapper.isFromExternalSubset(value.getUnparsed())) {
        XML_LIB_THROW(SyntaxError("Standalone document must not reference entity '" + value.getUnparsed() + "' declared in external subset."));
      }
      content = entityMapper.map(content);
    }
    auto xEntityReference = Node::make<EntityReference>(content);
    if (content.isEntityReference()) {
      if (entityExpansionDepth >= maxEntityExpansionDepth) {
        XML_LIB_THROW(SyntaxError("Entity expansion depth limit exceeded."));
      }
      struct DepthGuard {
        size_t &depth;
        ~DepthGuard() { --depth; }
      } guard{entityExpansionDepth};
      ++entityExpansionDepth;
      const bool isExt = entityMapper.isExternal(value.getUnparsed());
      const auto namespaces = (isA<Root>(xNode) || isA<Element>(xNode) || isA<Self>(xNode))
        ? NRef<Element>(xNode).getNameSpaces()
        : inheritedNamespaces;
      // Does entity contain start tag ?
      // YES then XML into current element list
      if (content.getParsed().starts_with("<")) {
        parseEntityReferenceXML(xNode, content, entityMapper, isExt, namespaces);
        return;
      }
      // NO XML into entity elements list.
      parseEntityReferenceXML(xEntityReference, content, entityMapper, isExt, namespaces);
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
void Default_Parser::parseContent(ISource &source, Node &xNode, IEntityMapper &entityMapper, std::span<const XMLAttribute> inheritedNamespaces)
{
  appendEntityOrContent(xNode, parseCharacter(source), entityMapper, inheritedNamespaces);
}

static void validateElementNamespaces(const Element &element, const ISource &source)
{
  if (!Default_Parser::isNamespacesEnabled()) {
    return;
  }
  const std::string &elemName = element.name();
  if (const auto pos = elemName.find(':'); pos != std::string::npos) {
    if (pos == 0 || pos + 1 >= elemName.size() || elemName.find(':', pos + 1) != std::string::npos) {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Invalid QName in element: '" + elemName + "'."));
    }
    const std::string prefix = elemName.substr(0, pos);
    if (prefix == "xmlns") {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "The prefix 'xmlns' must not be used as the prefix of an element."));
    }
    if (prefix != "xml" && !element.hasNameSpace(prefix)) {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Namespace used but not defined."));
    }
  }

  // Validate namespace declarations
  for (const auto &attr : element.getAttributes()) {
    const std::string &attrName = attr.getName();
    if (attrName == "xmlns") {
      const std::string &uri = attr.getParsed();
      if (uri == "http://www.w3.org/XML/1998/namespace") {
        XML_LIB_THROW(SyntaxError(source.getPosition(), "The xml namespace must not be declared as the default namespace."));
      }
      if (uri == "http://www.w3.org/2000/xmlns/") {
        XML_LIB_THROW(SyntaxError(source.getPosition(), "The xmlns namespace must not be declared as the default namespace."));
      }
    } else if (attrName == "xmlns:") {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Empty prefix in namespace declaration."));
    } else if (attrName.starts_with("xmlns:")) {
      const std::string prefix = attrName.substr(6);
      if (prefix.empty() || prefix.find(':') != std::string::npos) {
        XML_LIB_THROW(SyntaxError(source.getPosition(), "Invalid prefix in namespace declaration."));
      }
      if (prefix == "xmlns") {
        XML_LIB_THROW(SyntaxError(source.getPosition(), "The prefix 'xmlns' must not be declared."));
      }
      const std::string &uri = attr.getParsed();
      if (Default_Parser::isStrictNamespaces() && uri.empty()) {
        XML_LIB_THROW(SyntaxError(source.getPosition(), "Empty namespace URI in prefix declaration is not allowed in XML Namespaces 1.0."));
      }
      if (prefix == "xml") {
        if (uri != "http://www.w3.org/XML/1998/namespace") {
          XML_LIB_THROW(SyntaxError(source.getPosition(), "The prefix 'xml' must only be bound to 'http://www.w3.org/XML/1998/namespace'."));
        }
      } else {
        if (uri == "http://www.w3.org/XML/1998/namespace") {
          XML_LIB_THROW(SyntaxError(source.getPosition(), "The xml namespace can only be bound to prefix 'xml'."));
        }
        if (uri == "http://www.w3.org/2000/xmlns/") {
          XML_LIB_THROW(SyntaxError(source.getPosition(), "The xmlns namespace must not be bound to any prefix."));
        }
      }
    }
  }

  // Validate regular attributes and attribute uniqueness under namespaces
  std::set<std::pair<std::string, std::string>> resolvedAttrs;
  for (const auto &attr : element.getAttributes()) {
    const std::string &attrName = attr.getName();
    if (attrName == "xmlns" || attrName.starts_with("xmlns:")) {
      continue;
    }
    std::string nsUri;
    std::string localPart = attrName;
    if (const auto pos = attrName.find(':'); pos != std::string::npos) {
      if (pos == 0 || pos + 1 >= attrName.size() || attrName.find(':', pos + 1) != std::string::npos) {
        XML_LIB_THROW(SyntaxError(source.getPosition(), "Invalid QName in attribute: '" + attrName + "'."));
      }
      const std::string prefix = attrName.substr(0, pos);
      localPart = attrName.substr(pos + 1);
      if (prefix == "xmlns") {
        XML_LIB_THROW(SyntaxError(source.getPosition(), "The prefix 'xmlns' must not be used as the prefix of an attribute."));
      }
      if (prefix == "xml") {
        nsUri = "http://www.w3.org/XML/1998/namespace";
      } else if (element.hasNameSpace(prefix)) {
        nsUri = element.getNameSpace(prefix).getParsed();
      } else {
        XML_LIB_THROW(SyntaxError(source.getPosition(), "Namespace used but not defined in attribute '" + attrName + "'."));
      }
    }
    if (!resolvedAttrs.insert({ nsUri, localPart }).second) {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Attribute '" + attrName + "' defined more than once after namespace resolution."));
    }
  }
}

/// <summary>
/// Parse element internal area, generating any Node(s) and adding them
/// to the child list of the current XElement. This can be anything from
/// comments, program instructions, CDATA, nested elements or even content.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <param name="xNode">Current element Node.</param>
/// <param name="entityMapper">Entity mapper interface object.</param>
void Default_Parser::parseElementInternal(ISource &source, Node &xNode, IEntityMapper &entityMapper, std::span<const XMLAttribute> inheritedNamespaces)
{
  if (tryParseCommentOrPI(source, xNode)) {
    // comment or PI handled
  } else if (match(source, "<![CDATA[")) {
    markTrailingContentNonWhitespace(xNode);
    xNode.addChild(parseCDATA(source));
  } else if (match(source, "<")) {
    const auto outerNamespaces = (isA<Root>(xNode) || isA<Element>(xNode) || isA<Self>(xNode))
      ? NRef<Element>(xNode).getNameSpaces()
      : inheritedNamespaces;
    xNode.addChild(parseElement(source, outerNamespaces, entityMapper));
    validateElementNamespaces(NRef<Element>(xNode.getChildren().back()), source);
  } else {
    if (match(source, "</")) { XML_LIB_THROW(SyntaxError(source.getPosition(), "Missing closing tag.")); }
    if (match(source, "]]>")) {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "']]>' invalid in element content area."));
    }
    parseContent(source, xNode, entityMapper, inheritedNamespaces);
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
  if (source.more() && source.current() != '/' && source.current() != '>') {
    if (!isWS(source)) {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Missing whitespace after element name."));
    }
    ignoreWS(source);
  }
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
    if (++currentElementCount > maxElementCount) { XML_LIB_THROW(SyntaxError("Maximum element count exceeded.")); }
    xNode.reserveChildren(8);
    if (elementNestingDepth >= maxElementNestingDepth) {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Maximum element nesting depth exceeded."));
    }
    ++elementNestingDepth;
    while (source.more() && !match(source, "</")) { parseElementInternal(source, xNode, entityMapper); }
    --elementNestingDepth;
    if (match(source, toUtf16(NRef<Element>(xNode).name()))) {
      ignoreWS(source);
      if (source.more() && source.current() == '>') {
        source.next();
        return xNode;
      }
    }
  } else if (match(source, "/>")) {
    // Self-closing element tag
    if (++currentElementCount > maxElementCount) { XML_LIB_THROW(SyntaxError("Maximum element count exceeded.")); }
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
  if (match(source, "<?xml")) {
    if (!isWS(source)) {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Version missing from declaration."));
    }
    ignoreWS(source);
    if (match(source, "version")) {
      static constexpr std::array<std::string_view, 2> kVersions{ "1.0", "1.1" };
      version = parseDeclarationAttribute(source, "version", kVersions);
    } else {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Version missing from declaration."));
    }
    bool hadWS = false;
    if (isWS(source)) {
      hadWS = true;
      ignoreWS(source);
    }
    if (match(source, "encoding")) {
      if (!hadWS) {
        XML_LIB_THROW(SyntaxError(source.getPosition(), "Whitespace required before encoding attribute."));
      }
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
      if (encoding == "UTF-16" || encoding == "ISO-10646-UCS-2" || encoding == "ISO-10646-UCS-4") {
        if (!source.getSystemId().empty() && source.getSystemId() != "<buffer>") {
          std::error_code ec;
          if (std::filesystem::exists(source.getSystemId(), ec)) {
            const auto fmt = XML_FileIO::getFileFormat(source.getSystemId());
            if (fmt != XML::Format::utf16BE && fmt != XML::Format::utf16LE) {
              XML_LIB_THROW(SyntaxError(source.getPosition(), "Document declared as " + encoding + " but does not match declared encoding (missing UTF-16 BOM)."));
            }
          }
        }
      }
      hadWS = false;
    }
    if (isWS(source)) {
      hadWS = true;
      ignoreWS(source);
    }
    if (match(source, "standalone")) {
      if (!hadWS) {
        XML_LIB_THROW(SyntaxError(source.getPosition(), "Whitespace required before standalone attribute."));
      }
      static constexpr std::array<std::string_view, 2> kStandalone{ "yes", "no" };
      standalone = parseDeclarationAttribute(source, "standalone", kStandalone);
      hadWS = false;
    }
    ignoreWS(source);
    if (match(source, "encoding")) {
      XML_LIB_THROW(
        SyntaxError(source.getPosition(), "Incorrect order for version, encoding and standalone attributes."));
    }
    ignoreWS(source);
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

void Default_Parser::ensureTextNodeSizeWithinLimit(std::size_t nodeSize)
{
  if (nodeSize > maxTextNodeSize) { XML_LIB_THROW(SyntaxError("Maximum text node size exceeded.")); }
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
  if (!source.getSystemId().empty() && source.getSystemId() != "<buffer>") {
    if (auto *dtdValidator = dynamic_cast<DTD_Validator *>(validator.get())) {
      dtdValidator->setBaseDirectory(std::filesystem::path(source.getSystemId()).parent_path());
    }
  }
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
  maxAttributeCount = options.maxAttributeCount;
  maxElementCount = options.maxElementCount;
  currentElementCount = 0;
  maxTotalAttributeCount = options.maxTotalAttributeCount;
  currentTotalAttributeCount = 0;
  maxTextNodeSize = options.maxTextNodeSize;
  entityMapper.setExternalEntityPolicy(options.allowExternalEntities, options.entityResolver);
  if (!source.getSystemId().empty()) {
    entityMapper.setBaseDirectory(std::filesystem::path(source.getSystemId()).parent_path());
  }
  XML_Arena::ScopedCurrentArena scopedCurrentArena(arena);
  XML_Arena::ScopedDefaultResource scopedDefaultResource(arena);
  // Reset XML before next parse
  entityMapper.reset();
  hasRoot = false;
  isStandaloneDocument = false;
  strictNamespacesMode = options.strictNamespaces;
  enableNamespacesMode = options.enableNamespaces;
  allowFuture1xVersionsMode = options.allowFuture1xVersions;
  firstEntityDeclarationBindingMode = options.firstEntityDeclarationBinding;
  validator.reset();
  // Handle prolog
  Node xmlRoot = parseProlog(source, entityMapper);
  if (!xmlRoot.getChildren().empty() && isA<Declaration>(xmlRoot.getChildren()[0])) {
    isStandaloneDocument = NRef<Declaration>(xmlRoot.getChildren()[0]).standalone() == "yes";
  }
  // Handle main body
  if (match(source, "<")) {
    xmlRoot.addChild(parseElement(source, {}, entityMapper));
    validateElementNamespaces(NRef<Element>(xmlRoot.getChildren().back()), source);
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
