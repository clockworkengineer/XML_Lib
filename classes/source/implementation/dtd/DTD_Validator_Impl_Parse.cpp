//
// Class: DTD_impl_Parse
//
// Description: Parse XML DTD.
//
// Dependencies: C++20 - Language standard features used.
//

#include "DTD_Impl.hpp"

namespace XML_Lib {

/// <summary>
/// Validate notation description
/// </summary>
/// <param name="notations">Notations string values (separated by '|').</param>
/// <returns></returns>
void DTD_Impl::parseValidNotations(const std::string_view &notations) const
{
  for (auto &notation : splitString(notations.substr(1, notations.size() - 2), '|')) {
    if (xDTD.getNotationCount(notation) == 0) { throw SyntaxError("NOTATION " + notation + " is not defined."); }
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
    if (xDTD.getElement(elementName).idAttributePresent) {
      XML_LIB_THROW(SyntaxError("Element <" + std::string(elementName) + "> has more than one ID attribute."));
    }
    xDTD.getElement(elementName).idAttributePresent = true;
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
  std::string enumerationType(toUtf8(source.current()));
  source.next();
  ignoreWS(source);
  enumerationType += parseName(source);
  parseDelimitedList(source, '|',
    [&](ISource &src) {
      enumerationType += toUtf8(src.current());
      src.next();
      ignoreWS(src);
    },
    [&](ISource &) {
      enumerationType += parseName(source);
    }
  );
  if (source.current() != ')') {
    XML_LIB_THROW(SyntaxError(source.getPosition(), "Missing closing ')' on enumeration attribute type."));
  }
  enumerationType += toUtf8(source.current());
  source.next();
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
      attribute.type = type;
      ignoreWS(source);
      return;
    }
  }
  if (match(source, "NOTATION")) {
    attribute.type = DTD::AttributeType::notation;
    ignoreWS(source);
  }
  if (source.current() == '(') {
    attribute.enumeration = parseAttributeEnumerationType(source);
    if (attribute.type == DTD::AttributeType::notation) {
      parseValidNotations(attribute.enumeration);
    } else {
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
    ignoreWS(source);
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
  ignoreWS(source);
  const std::string elementName = parseName(source);
  while (source.more() && validNameStartChar(source.current())) {
    DTD::Attribute dtdAttribute;
    dtdAttribute.name = parseName(source);
    parseAttributeType(source, dtdAttribute);
    parseAttributeValue(source, dtdAttribute);
    parseValidateAttribute(elementName, dtdAttribute);
    xDTD.getElement(elementName).attributes.emplace_back(dtdAttribute);
    ignoreWS(source);
  }
}

/// <summary>
/// Parse DTD notation.
/// </summary>
/// <param name="source">DTD source stream.</param>
void DTD_Impl::parseNotation(ISource &source) const
{
  ignoreWS(source);
  const std::string name = parseName(source);
  xDTD.addNotation(name, parseExternalReference(source));
  ignoreWS(source);
}

/// <summary>
/// Parse DTD entity.
/// </summary>
/// <param name="source">DTD source stream.</param>
void DTD_Impl::parseEntity(ISource &source) const
{
  std::string entityName = "&";
  ignoreWS(source);
  if (source.current() == '%') {
    entityName = "%";
    source.next();
    ignoreWS(source);
  }
  entityName += parseName(source) + ";";
  if (source.current() == '\'' || source.current() == '"') {
    const XMLValue entityValue = parseValue(source);
    // Force expansion to trigger recursion detection
    std::string expanded = entityValue.getParsed();
    if (expanded.find('&') != std::string::npos) {
      std::set<std::string> currentEntities;
      xDTD.getEntityMapper().checkRecursiveEntity(entityName, expanded, currentEntities);
    }
    xDTD.getEntityMapper().setInternal(entityName, expanded);
  } else {
    xDTD.getEntityMapper().setExternal(entityName, parseExternalReference(source));
    if (match(source, "NDATA")) {
      ignoreWS(source);
      xDTD.getEntityMapper().setNotation(entityName, parseName(source));
    }
  }
}

/// <summary>
/// Parse an DTD element.
/// </summary>
/// <param name="source">DTD source stream.</param>
void DTD_Impl::parseElement(ISource &source)
{
  ignoreWS(source);
  const std::string elementName = parseName(source);
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
  while (source.more() && !match(source, "--")) { source.next(); }
}

/// <summary>
/// Parse DTD parameter entity reference.
/// </summary>
/// <param name="source">DTD source stream.</param>
void DTD_Impl::parseParameterEntityReference(ISource &source)
{
  const XMLValue parameterEntity = parseEntityReference(source);
  BufferSource entitySource(xDTD.getEntityMapper().translate(parameterEntity.getUnparsed()));
  parseInternal(entitySource);
  ignoreWS(source);
}

/// <summary>
/// Parse internally defined DTD.
/// </summary>
/// <param name="source">DTD source stream.</param>
void DTD_Impl::parseInternal(ISource &source)
{
  while (source.more() && !match(source, "]>")) {
    if (match(source, "<!ENTITY")) {
      parseEntity(source);
    } else if (match(source, "<!ELEMENT")) {
      parseElement(source);
    } else if (match(source, "<!ATTLIST")) {
      parseAttributeList(source);
    } else if (match(source, "<!NOTATION")) {
      parseNotation(source);
    } else if (match(source, "<!--")) {
      parseComment(source);
    } else if (source.current() == '%') {
      parseParameterEntityReference(source);
      continue;
    } else {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Invalid DTD tag."));
    }
    if (source.current() != '>') { throw SyntaxError(source.getPosition(), "Missing '>' terminator."); }
    source.next();
    ignoreWS(source);
  }
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
  ignoreWS(source);
  xDTD.setRootName(parseName(source));
  // Parse in external DTD reference
  if (source.current() != '[') { xDTD.setExternalReference(parseExternalReference(source)); }
  // We have internal DTD so parse that first
  if (source.current() == '[') {
    source.next();
    ignoreWS(source);
    parseInternal(source);
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
  // Count lines in DTD
  std::string unparsedDTD = xDTD.unparsed();
  xDTD.setLineCount(std::ranges::count(unparsedDTD, kLineFeed) + 1);
}
}// namespace XML_Lib
