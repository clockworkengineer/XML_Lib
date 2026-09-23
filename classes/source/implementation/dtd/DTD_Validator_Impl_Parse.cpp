//
// Class: DTD_impl_Parse
//
// Description: Parse XML DTD.
//
// Dependencies: C++20 - Language standard features used.
//

#include "DTD_Impl.hpp"
#include "common/XML_ParseHelpers.hpp"
#include "implementation/parser/Default_Parser.hpp"

namespace XML_Lib {

/// <summary>
/// Validate notation description
/// </summary>
/// <param name="notations">Notations string values (separated by '|').</param>
/// <returns></returns>
void DTD_Impl::parseValidNotations(const std::string_view &notations) const
{
  for (auto &notation : splitString(notations.substr(1, notations.size() - 2), '|')) {
    if (xDTD.getNotationCount(notation) == 0) { XML_LIB_THROW(SyntaxError("NOTATION " + notation + " is not defined.")); }
  }
}

/// <summary>
/// Validate attribute description.
/// </summary>
/// <param name="elementName">Element associated with attribute.</param>
/// <param name="dtdAttribute">Attribute description to validate.</param>
void DTD_Impl::parseValidateAttribute(const std::string_view &elementName, const DTD::Attribute &dtdAttribute) const
{
  // Attribute cannot be ID and fixed
  if (dtdAttribute.type == (DTD::AttributeType::id | DTD::AttributeType::fixed)) {
    XML_LIB_THROW(SyntaxError("Attribute '" + dtdAttribute.name + "' may not be of type ID and FIXED."));
  }
  // Only one ID attribute allowed per element
  if ((dtdAttribute.type & DTD::AttributeType::id) != 0) {
    if (xDTD.getOrCreateElement(elementName).idAttributePresent) {
      XML_LIB_THROW(SyntaxError("Element <" + std::string(elementName) + "> has more than one ID attribute."));
    }
    xDTD.getOrCreateElement(elementName).idAttributePresent = true;
  }
  // Enumeration contains unique values and default is valid value
  else if (dtdAttribute.type == (DTD::AttributeType::enumeration | DTD::AttributeType::normal)) {
    std::set<std::string> options;
    for (auto &option : splitString(dtdAttribute.enumeration.substr(1, dtdAttribute.enumeration.size() - 2), '|')) {
      if (!options.contains(option)) {
        options.insert(option);
      } else {
        XML_LIB_THROW(SyntaxError("Enumerator value '" + option + "' for attribute '" + dtdAttribute.name
                          + "' occurs more than once in its definition."));
      }
    }
    if (!options.contains(dtdAttribute.value.getParsed())) {
      XML_LIB_THROW(SyntaxError("Default value '" + dtdAttribute.value.getParsed() + "' for enumeration attribute '"
                        + dtdAttribute.name + "' is invalid."));
    }
  }
}

/// <summary>
/// Parse attribute of type enumeration.
/// </summary>
/// <param name="source">DTD source stream.</param>
/// <returns>Enumeration string.</returns>
std::string DTD_Impl::parseAttributeEnumerationType(ISource &source)
{
  const auto readEnumerationToken = [](ISource &src) -> std::string {
    String token;
    token.reserve(16);
    while (src.more() && validNameChar(src.current())) {
      token += src.current();
      src.next();
    }
    if (token.empty()) {
      XML_LIB_THROW(SyntaxError(src.getPosition(), "Invalid name '' encountered."));
    }
    return toUtf8(token);
  };
  std::string enumerationType(toUtf8(source.current()));
  source.next();
  ignoreWS(source);
  enumerationType += readEnumerationToken(source);
  ignoreWS(source);
  parseDelimitedList(source, '|',
    [&](ISource &src) {
      enumerationType += toUtf8(src.current());
      src.next();
      ignoreWS(src);
    },
    [&](ISource &) {
      enumerationType += readEnumerationToken(source);
      ignoreWS(source);
    }
  );
  if (source.current() != ')') {
    XML_LIB_THROW(SyntaxError(source.getPosition(), "Missing closing ')' on enumeration attribute type."));
  }
  enumerationType += toUtf8(source.current());
  source.next();
  if (source.more() && !isWS(source) && source.current() != '>') {
    XML_LIB_THROW(SyntaxError(source.getPosition(), "Missing whitespace after enumeration attribute type."));
  }
  ignoreWS(source);
  return enumerationType;
}

/// <summary>
/// Parse DTD attribute type field.
/// </summary>
/// <param name="source">DTD source stream.</param>
/// <param name="attribute">Attribute description.</param>
/// <returns>Attribute type as string (UTF-8 encoded).</returns>
void DTD_Impl::parseAttributeType(ISource &source, DTD::Attribute &attribute) const
{
  // Table-driven keyword match; ordering is significant (IDREFS before IDREF, etc.)
  static constexpr std::pair<const char *, DTD::AttributeType> kAttrTypeTable[] = {
    { "CDATA",    DTD::AttributeType::cdata    },
    { "IDREFS",   DTD::AttributeType::idrefs   },
    { "IDREF",    DTD::AttributeType::idref    },
    { "ID",       DTD::AttributeType::id       },
    { "NMTOKENS", DTD::AttributeType::nmtokens },
    { "NMTOKEN",  DTD::AttributeType::nmtoken  },
    { "ENTITIES", DTD::AttributeType::entities },
    { "ENTITY",   DTD::AttributeType::entity   },
  };
  for (const auto &[keyword, type] : kAttrTypeTable) {
    if (match(source, keyword)) {
      if (source.more() && (source.current() == '#' || (!isWS(source.current()) && source.current() != '>'))) {
        XML_LIB_THROW(SyntaxError(source.getPosition(), "Missing whitespace after attribute type."));
      }
      attribute.type = type;
      ignoreWS(source);
      return;
    }
  }
  if (match(source, "NOTATION")) {
    if (!source.more() || !isWS(source.current())) {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Missing whitespace after NOTATION keyword."));
    }
    attribute.type = DTD::AttributeType::notation;
    ignoreWS(source);
  }
  if (source.current() == '(') {
    attribute.enumeration = parseAttributeEnumerationType(source);
    if (attribute.type != DTD::AttributeType::notation) {
      attribute.type = DTD::AttributeType::enumeration;
    }
    return;
  }
  XML_LIB_THROW(SyntaxError(source.getPosition(), "Invalid attribute type specified."));
}

/// <summary>
/// Parse DTD attribute value.
/// </summary>
/// <param name="source">DTD source stream.</param>
/// <param name="attribute">Attribute description.</param>
void DTD_Impl::parseAttributeValue(ISource &source, DTD::Attribute &attribute) const
{
  if (match(source, "#REQUIRED")) {
    attribute.type |= DTD::AttributeType::required;
  } else if (match(source, "#IMPLIED")) {
    attribute.type |= DTD::AttributeType::implied;
  } else {
    const bool isFixed = match(source, "#FIXED");
    if (isFixed) {
      if (!isWS(source)) {
        XML_LIB_THROW(SyntaxError(source.getPosition(), "Missing whitespace after #FIXED."));
      }
      ignoreWS(source);
    }
    attribute.value = parseValue(source, xDTD.getEntityMapper());
    attribute.type |= isFixed ? DTD::AttributeType::fixed : DTD::AttributeType::normal;
  }
}

/// <summary>
/// Parse DTD attribute list.
/// </summary>
/// <param name="source">DTD source stream.</param>
void DTD_Impl::parseAttributeList(ISource &source) const
{
  if (!isWS(source)) {
    XML_LIB_THROW(SyntaxError(source.getPosition(), "Whitespace required after '<!ATTLIST'."));
  }
  ignoreWS(source);
  const std::string elementName = parseName(source);
  while (source.more() && source.current() != '>' && source.current() != '<') {
    if (!isWS(source)) {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Missing whitespace between attribute definitions."));
    }
    ignoreWS(source);
    if (!source.more() || source.current() == '>' || source.current() == '<') { break; }
    DTD::Attribute dtdAttribute;
    dtdAttribute.name = toUtf8(readName(source));
    if (!validName(toUtf16(dtdAttribute.name))) {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Invalid name '" + dtdAttribute.name + "' encountered."));
    }
    if (!source.more() || !isWS(source)) {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Missing whitespace after attribute name."));
    }
    ignoreWS(source);
    parseAttributeType(source, dtdAttribute);
    parseAttributeValue(source, dtdAttribute);
    parseValidateAttribute(elementName, dtdAttribute);
    xDTD.getOrCreateElement(elementName).attributes.emplace_back(dtdAttribute);
  }
}

/// <summary>
/// Parse DTD notation.
/// </summary>
/// <param name="source">DTD source stream.</param>
void DTD_Impl::parseNotation(ISource &source) const
{
  if (!isWS(source)) {
    XML_LIB_THROW(SyntaxError(source.getPosition(), "Whitespace required after '<!NOTATION'."));
  }
  ignoreWS(source);
  const std::string name = parseName(source);
  if (Default_Parser::isNamespacesEnabled() && name.find(':') != std::string::npos) {
    XML_LIB_THROW(SyntaxError(source.getPosition(), "Colons are not allowed in notation names under XML Namespaces."));
  }
  if (!source.more() || !isWS(source)) {
    XML_LIB_THROW(SyntaxError(source.getPosition(), "Missing whitespace after notation name."));
  }
  ignoreWS(source);
  xDTD.addNotation(name, parseExternalReference(source, false));
  ignoreWS(source);
}

/// <summary>
/// Parse DTD entity.
/// </summary>
/// <param name="source">DTD source stream.</param>
void DTD_Impl::parseEntity(ISource &source, bool isInternalSubset) const
{
  if (!isWS(source)) {
    XML_LIB_THROW(SyntaxError(source.getPosition(), "Whitespace required after '<!ENTITY'."));
  }
  std::string entityName = "&";
  ignoreWS(source);
  bool isPE = false;
  if (source.current() == '%') {
    isPE = true;
    entityName = "%";
    source.next();
    if (!source.more() || !isWS(source)) {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Missing whitespace after '%' in parameter entity declaration."));
    }
    ignoreWS(source);
  }
  const std::string rawName = toUtf8(readName(source));
  if (Default_Parser::isNamespacesEnabled() && rawName.find(':') != std::string::npos) {
    XML_LIB_THROW(SyntaxError(source.getPosition(), "Colons are not allowed in entity names under XML Namespaces."));
  }
  const String u16Name = toUtf16(rawName);
  if (u16Name.empty() || !validNameStartChar(u16Name[0])) {
    XML_LIB_THROW(SyntaxError(source.getPosition(), "Invalid name '" + rawName + "' encountered."));
  }
  for (size_t i = 1; i < u16Name.size(); ++i) {
    if (!validNameChar(u16Name[i])) {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Invalid name '" + rawName + "' encountered."));
    }
  }
  if (!source.more() || !isWS(source)) {
    XML_LIB_THROW(SyntaxError(source.getPosition(), "Missing whitespace after entity name."));
  }
  ignoreWS(source);
  entityName += rawName + ";";
  if (source.current() == '\'' || source.current() == '"') {
    const XMLValue entityValue = parseValue(source);
    if (isInternalSubset && entityValue.getUnparsed().find('%') != std::string::npos) {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Parameter entity references must not occur within markup declarations in the internal subset."));
    }
    const std::string &unp = entityValue.getUnparsed();
    for (size_t i = 0; i < unp.size(); ++i) {
      if (unp[i] == '%') {
        size_t semi = unp.find(';', i + 1);
        if (semi == std::string::npos) {
          XML_LIB_THROW(SyntaxError(source.getPosition(), "Character '%' in entity value must be a parameter entity reference."));
        }
      }
    }
    // Force expansion to trigger recursion detection
    std::string expanded = entityValue.getParsed();
    if (!isInternalSubset) {
      size_t depth = 0;
      while (expanded.find('%') != std::string::npos && depth++ < 32) {
        std::string next = xDTD.getEntityMapper().translate(expanded);
        if (next == expanded) break;
        expanded = std::move(next);
      }
    }
    if (expanded.find('&') != std::string::npos) {
      std::set<std::string> currentEntities;
      xDTD.getEntityMapper().checkRecursiveEntity(entityName, expanded, currentEntities);
    }
    const bool alreadyDeclared = Default_Parser::isFirstEntityDeclarationBinding() && xDTD.getEntityMapper().isPresent(entityName);
    if (!alreadyDeclared) {
      xDTD.getEntityMapper().setInternal(entityName, expanded);
      if (!isInternalSubset) {
        xDTD.getEntityMapper().setFromExternalSubset(entityName, true);
      }
    }
  } else {
    const auto extRef = parseExternalReference(source, true);
    const bool alreadyDeclared = Default_Parser::isFirstEntityDeclarationBinding() && xDTD.getEntityMapper().isPresent(entityName);
    if (!alreadyDeclared) {
      xDTD.getEntityMapper().setExternal(entityName, extRef);
      if (!isInternalSubset) {
        xDTD.getEntityMapper().setFromExternalSubset(entityName, true);
      }
    }
    if (source.more() && isWS(source)) {
      ignoreWS(source);
      if (match(source, "NDATA")) {
        if (isPE) {
          XML_LIB_THROW(SyntaxError(source.getPosition(), "Parameter entities cannot declare NDATA."));
        }
        if (!source.more() || !isWS(source)) {
          XML_LIB_THROW(SyntaxError(source.getPosition(), "Missing whitespace after NDATA."));
        }
        ignoreWS(source);
        if (!alreadyDeclared) {
          xDTD.getEntityMapper().setNotation(entityName, parseName(source));
        } else {
          static_cast<void>(parseName(source));
        }
      }
    } else if (match(source, "NDATA")) {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Missing whitespace before NDATA."));
    }
  }
  ignoreWS(source);
}

/// <summary>
/// Parse an DTD element.
/// </summary>
/// <param name="source">DTD source stream.</param>
void DTD_Impl::parseElement(ISource &source)
{
  if (!isWS(source)) {
    XML_LIB_THROW(SyntaxError(source.getPosition(), "Whitespace required after '<!ELEMENT'."));
  }
  ignoreWS(source);
  const std::string elementName = parseName(source);
  if (!source.more() || !isWS(source)) {
    XML_LIB_THROW(SyntaxError(source.getPosition(), "Missing whitespace after element name."));
  }
  ignoreWS(source);
  if (match(source, "EMPTY")) {
    xDTD.addElement(elementName, DTD::Element(elementName, XMLValue{ "EMPTY", "EMPTY" }));
  } else if (match(source, "ANY")) {
    xDTD.addElement(elementName, DTD::Element(elementName, XMLValue{ "ANY", "ANY" }));
  } else {
    std::string unparsed;
    while (source.more() && source.current() != '<' && source.current() != '>') {
      unparsed += toUtf8(source.current());
      source.next();
    }
    xDTD.addElement(
      elementName, DTD::Element(elementName, parseElementInternalSpecification(elementName, XMLValue{ unparsed, "" })));
  }
  ignoreWS(source);
}

/// <summary>
/// Parse DTD comment.
/// </summary>
/// <param name="source">DTD source stream.</param>
void DTD_Impl::parseComment(ISource &source)
{
  (void)parseCommentBody(source);
}

/// <summary>
/// Parse DTD processing instruction.
/// </summary>
/// <param name="source">DTD source stream.</param>
void DTD_Impl::parsePI(ISource &source)
{
  (void)parsePIBody(source, true, false);
}

/// <summary>
/// Parse DTD parameter entity reference.
/// </summary>
/// <param name="source">DTD source stream.</param>
void DTD_Impl::parseParameterEntityReference(ISource &source)
{
  const XMLValue parameterEntity = parseEntityReference(source);
  const bool isExt = xDTD.getEntityMapper().isExternal(parameterEntity.getUnparsed());
  if (isExt) {
    BufferSource entitySource(xDTD.getEntityMapper().translate(parameterEntity.getUnparsed()) + " ");
    std::filesystem::path oldDir = baseDirectory;
    std::filesystem::path entityPath{ xDTD.getEntityMapper().getExternal(parameterEntity.getUnparsed()).getSystemID() };
    if (entityPath.is_relative() && !baseDirectory.empty()) {
      entityPath = (baseDirectory / entityPath).lexically_normal();
    }
    if (entityPath.has_parent_path()) {
      setBaseDirectory(entityPath.parent_path());
      xDTD.getEntityMapper().setBaseDirectory(entityPath.parent_path());
    }
    parseExternalContent(entitySource);
    setBaseDirectory(oldDir);
    xDTD.getEntityMapper().setBaseDirectory(oldDir);
  } else {
    BufferSource entitySource(" " + xDTD.getEntityMapper().translate(parameterEntity.getUnparsed()) + " ");
    parseInternal(entitySource);
  }
  ignoreWS(source);
}

/// <summary>
/// Parse internally defined DTD.
/// </summary>
/// <param name="source">DTD source stream.</param>
void DTD_Impl::parseInternal(ISource &source)
{
  parseSubsetDeclarations(source, DTDSubsetKind::Internal);
}

/// <summary>
/// Parse XML DTD.
/// If the DTD contains an external reference, then the DTD
/// that points to is parsed after any internal DTD that may be specified
/// after it.
/// </summary>
/// <param name="source">DTD source stream.</param>
void DTD_Impl::parseDTD(ISource &source)
{
  // We take the easy option for allowing a DTD to be stringified
  // and keeping the correct order for its components by storing it
  // in its raw unparsed form.
  const long start = source.position();
  if (!isWS(source)) {
    XML_LIB_THROW(SyntaxError(source.getPosition(), "Whitespace required after '<!DOCTYPE'."));
  }
  ignoreWS(source);
  xDTD.setRootName(parseName(source));
  ignoreWS(source);
  // Parse in external DTD reference
  if (source.current() != '[') {
    xDTD.setExternalReference(parseExternalReference(source, true));
    ignoreWS(source);
  }
  // We have internal DTD so parse that first
  if (source.current() == '[') {
    source.next();
    ignoreWS(source);
    parseInternal(source);
    ignoreWS(source);
    if (source.current() != '>') {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Missing '>' terminator."));
    }
    source.next();
    ignoreWS(source);
    xDTD.setType(DTD::Type::internal);
  }
  // Missing '>' after external DTD reference
  else if (source.current() != '>') {
    XML_LIB_THROW(SyntaxError(source.getPosition(), "Missing '>' terminator."));
  }
  // Move to the next component in XML prolog
  else {
    source.next();
    ignoreWS(source);
  }
  // Parse any DTD in external reference found
  if (xDTD.getExternalReference().isPublic() || xDTD.getExternalReference().isSystem()) {
    parseExternal(source);
    xDTD.setType(xDTD.getType() | DTD::Type::external);
  }
  // Save away unparsed form of DTD
  xDTD.setUnparsed(std::string("<!DOCTYPE") + source.getRange(start, source.position()));
  // Make sure no defined entity contains recursion
  xDTD.getEntityMapper().checkForRecursion();
  // Validate all NOTATION attribute types reference declared notations
  for (const auto &[elemName, elem] : xDTD.getElements()) {
    for (const auto &attr : elem.attributes) {
      if ((attr.type & DTD::AttributeType::notation) != 0) {
        for (const auto &notName : splitString(attr.enumeration.substr(1, attr.enumeration.size() - 2), '|')) {
          if (xDTD.getNotationCount(notName) == 0) {
            XML_LIB_THROW(SyntaxError("NOTATION " + notName + " is not defined."));
          }
        }
      }
    }
  }
  // Count lines in DTD
  std::string unparsedDTD = xDTD.unparsed();
  xDTD.setLineCount(std::ranges::count(unparsedDTD, kLineFeed) + 1);
}
}// namespace XML_Lib
