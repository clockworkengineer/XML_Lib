//
// Class: DTD_impl
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
void DTD_Impl::parseValidNotations(const std::string &notations) const
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
void DTD_Impl::parseValidateAttribute(const std::string &elementName, const XDTD::Attribute &dtdAttribute) const
{
  // Attribute cannot be ID and fixed
  if (dtdAttribute.type == (XDTD::AttributeType::id | XDTD::AttributeType::fixed)) {
    throw SyntaxError("Attribute '" + dtdAttribute.name + "' may not be of type ID and FIXED.");
  }
  // Only one ID attribute allowed per element
  if ((dtdAttribute.type & XDTD::AttributeType::id) != 0) {
    if (xDTD.getElement(elementName).idAttributePresent) {
      throw SyntaxError("Element <" + elementName + "> has more than one ID attribute.");
    }
    xDTD.getElement(elementName).idAttributePresent = true;
  }
  // Enumeration contains unique values and default is valid value
  else if (dtdAttribute.type == (XDTD::AttributeType::enumeration | XDTD::AttributeType::normal)) {
    std::set<std::string> options;
    for (auto &option : splitString(dtdAttribute.enumeration.substr(1, dtdAttribute.enumeration.size() - 2), '|')) {
      if (!options.contains(option)) {
        options.insert(option);
      } else {
        throw SyntaxError("Enumerator value '" + option + "' for attribute '" + dtdAttribute.name
                          + "' occurs more than once in its definition.");
      }
    }
    if (!options.contains(dtdAttribute.value.getParsed())) {
      throw SyntaxError("Default value '" + dtdAttribute.value.getParsed() + "' for enumeration attribute '"
                        + dtdAttribute.name + "' is invalid.");
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
  source.ignoreWS();
  enumerationType += parseName(source);
  while (source.more() && source.current() == '|') {
    enumerationType += toUtf8(source.current());
    source.next();
    source.ignoreWS();
    enumerationType += parseName(source);
  }
  if (source.current() != ')') {
    throw SyntaxError(source.getPosition(), "Missing closing ')' on enumeration attribute type.");
  }
  enumerationType += toUtf8(source.current());
  source.next();
  source.ignoreWS();
  return enumerationType;
}

/// <summary>
/// Parse DTD attribute type field.
/// </summary>
/// <param name="source">DTD source stream.</param>
/// <param name="attribute">Attribute description.</param>
/// <returns>Attribute type as string (UTF-8 encoded).</returns>
void DTD_Impl::parseAttributeType(ISource &source, XDTD::Attribute &attribute) const
{
  if (source.match("CDATA")) {
    attribute.type = XDTD::AttributeType::cdata;
    source.ignoreWS();
    return;
  }
  if (source.match("IDREFS")) {
    attribute.type = XDTD::AttributeType::idrefs;
    source.ignoreWS();
    return;
  }
  if (source.match("IDREF")) {
    attribute.type = XDTD::AttributeType::idref;
    source.ignoreWS();
    return;
  }
  if (source.match("ID")) {
    attribute.type = XDTD::AttributeType::id;
    source.ignoreWS();
    return;
  }
  if (source.match("NMTOKENS")) {
    attribute.type = XDTD::AttributeType::nmtokens;
    source.ignoreWS();
    return;
  }
  if (source.match("NMTOKEN")) {
    attribute.type = XDTD::AttributeType::nmtoken;
    source.ignoreWS();
    return;
  }
  if (source.match("ENTITY")) {
    attribute.type = XDTD::AttributeType::entity;
    source.ignoreWS();
    return;
  }
  if (source.match("ENTITIES")) {
    attribute.type = XDTD::AttributeType::entities;
    source.ignoreWS();
    return;
  }
  if (source.match("NOTATION")) {
    attribute.type = XDTD::AttributeType::notation;
    source.ignoreWS();
  }
  if (source.current() == '(') {
    attribute.enumeration = parseAttributeEnumerationType(source);
    if (attribute.type == XDTD::AttributeType::notation) {
      parseValidNotations(attribute.enumeration);
    } else {
      attribute.type = XDTD::AttributeType::enumeration;
    }
    return;
  }
  throw SyntaxError(source.getPosition(), "Invalid attribute type specified.");
}

/// <summary>
/// Parse DTD attribute value.
/// </summary>
/// <param name="source">DTD source stream.</param>
/// <param name="attribute">Attribute description.</param>
void DTD_Impl::parseAttributeValue(ISource &source, XDTD::Attribute &attribute) const
{
  if (source.match("#REQUIRED")) {
    attribute.type |= XDTD::AttributeType::required;
  } else if (source.match("#IMPLIED")) {
    attribute.type |= XDTD::AttributeType::implied;
  } else if (source.match("#FIXED")) {
    source.ignoreWS();
    attribute.value = parseValue(source, xDTD.getEntityMapper());
    attribute.type |= XDTD::AttributeType::fixed;
  } else {
    source.ignoreWS();
    attribute.value = parseValue(source, xDTD.getEntityMapper());
    attribute.type |= XDTD::AttributeType::normal;
  }
}

/// <summary>
/// Parse DTD attribute list.
/// </summary>
/// <param name="source">DTD source stream.</param>
void DTD_Impl::parseAttributeList(ISource &source) const
{
  source.ignoreWS();
  const std::string elementName = parseName(source);
  while (source.more() && validNameStartChar(source.current())) {
    XDTD::Attribute dtdAttribute;
    dtdAttribute.name = parseName(source);
    parseAttributeType(source, dtdAttribute);
    parseAttributeValue(source, dtdAttribute);
    parseValidateAttribute(elementName, dtdAttribute);
    xDTD.getElement(elementName).attributes.emplace_back(dtdAttribute);
    source.ignoreWS();
  }
}

/// <summary>
/// Parse DTD notation.
/// </summary>
/// <param name="source">DTD source stream.</param>
void DTD_Impl::parseNotation(ISource &source) const
{
  source.ignoreWS();
  const std::string name = parseName(source);
  xDTD.addNotation(name, parseExternalReference(source));
  source.ignoreWS();
}

/// <summary>
/// Parse DTD entity.
/// </summary>
/// <param name="source">DTD source stream.</param>
void DTD_Impl::parseEntity(ISource &source) const
{
  std::string entityName = "&";
  source.ignoreWS();
  if (source.current() == '%') {
    entityName = "%";
    source.next();
    source.ignoreWS();
  }
  entityName += parseName(source) + ";";
  if (source.current() == '\'' || source.current() == '"') {
    const XMLValue entityValue = parseValue(source);
    xDTD.getEntityMapper().setInternal(entityName, entityValue.getParsed());
  } else {
    xDTD.getEntityMapper().setExternal(entityName, parseExternalReference(source));
    if (source.match("NDATA")) {
      source.ignoreWS();
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
  source.ignoreWS();
  const std::string elementName = parseName(source);
  if (source.match("EMPTY")) {
    xDTD.addElement(elementName, XDTD::Element(elementName, XMLValue{ "EMPTY", "EMPTY" }));
  } else if (source.match("ANY")) {
    xDTD.addElement(elementName, XDTD::Element(elementName, XMLValue{ "ANY", "ANY" }));
  } else {
    std::string unparsed;
    while (source.more() && source.current() != '<' && source.current() != '>') {
      unparsed += toUtf8(source.current());
      source.next();
    }
    xDTD.addElement(
      elementName, XDTD::Element(elementName, parseElementInternalSpecification(elementName, XMLValue{ unparsed, "" })));
  }
  source.ignoreWS();
}

/// <summary>
/// Parse DTD comment.
/// </summary>
/// <param name="source">DTD source stream.</param>
void DTD_Impl::parseComment(ISource &source)
{
  while (source.more() && !source.match("--")) { source.next(); }
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
  source.ignoreWS();
}

/// <summary>
/// Parse internally defined DTD.
/// </summary>
/// <param name="source">DTD source stream.</param>
void DTD_Impl::parseInternal(ISource &source)
{
  while (source.more() && !source.match("]>")) {
    if (source.match("<!ENTITY")) {
      parseEntity(source);
    } else if (source.match("<!ELEMENT")) {
      parseElement(source);
    } else if (source.match("<!ATTLIST")) {
      parseAttributeList(source);
    } else if (source.match("<!NOTATION")) {
      parseNotation(source);
    } else if (source.match("<!--")) {
      parseComment(source);
    } else if (source.current() == '%') {
      parseParameterEntityReference(source);
      continue;
    } else {
      throw SyntaxError(source.getPosition(), "Invalid DTD tag.");
    }
    if (source.current() != '>') { throw SyntaxError(source.getPosition(), "Missing '>' terminator."); }
    source.next();
    source.ignoreWS();
  }
}

/// <summary>
/// Parse XML DTD. If the DTD contains an external reference then the DTD
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
  source.ignoreWS();
  xDTD.setRootName(parseName(source));
  // Parse in external DTD reference
  if (source.current() != '[') { xDTD.setExternalReference(parseExternalReference(source)); }
  // We have internal DTD so parse that first
  if (source.current() == '[') {
    source.next();
    source.ignoreWS();
    parseInternal(source);
    xDTD.setType(XDTD::Type::internal);
  }
  // Missing '>' after external DTD reference
  else if (source.current() != '>') {
    throw SyntaxError(source.getPosition(), "Missing '>' terminator.");
  }
  // Move to the next component in XML prolog
  else {
    source.next();
    source.ignoreWS();
  }
  // Parse any DTD in external reference found
  if (xDTD.getExternalReference().isPublic() || xDTD.getExternalReference().isSystem()) {
    parseExternal(source);
    xDTD.setType(xDTD.getType() | XDTD::Type::external);
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
