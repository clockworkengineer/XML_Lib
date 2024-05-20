//
// Class: DTD_Impl_Validator
//
// Description: DTD validator.
//
// Dependencies: C++20 - Language standard features used..
//

#include "DTD_Impl.hpp"

namespace XML_Lib {

/// <summary>
/// Generate an exception for an element error.
/// </summary>
/// <param name="xElement">Element X Node.</param>
/// <param name="error">Error text string.</param>
void DTD_Impl::elementError(const XElement &xElement, const std::string &error) const
{
  throw ValidationError(lineNumber, "Element <" + xElement.name() + "> " + error);
}

/// <summary>
/// Check whether a token value is valid.
/// </summary>
/// <param name="nmTokenValue">Token value.</param>
/// <returns>true then token is valid otherwise false.</returns>
bool DTD_Impl::checkIsNMTOKENOK(const std::string &nmTokenValue)
{
  BufferSource nmTokenValueSource(trimmString(nmTokenValue));
  while (nmTokenValueSource.more()) {
    if (!validNameChar(nmTokenValueSource.current())) { return (false); }
    nmTokenValueSource.next();
  }
  return (true);
}

/// <summary>
/// Check whether ID value is valid.
/// </summary>
/// <param name="idValue">ID string value.</param>
/// <returns>true then ID is valid otherwise false.</returns>
bool DTD_Impl::checkIsIDOK(const std::string &idValue)
{
  try {
    BufferSource idSource(idValue);
    auto name = parseName(idSource);
  } catch (std::exception &) {
    return (false);
  }
  return (true);
}

/// <summary>
/// Check whether element contains characters.
/// </summary>
/// <param name="xNode">Current element XNode.</param>
/// <returns>true if element contains characters otherwise false.</returns>
bool DTD_Impl::checkIsPCDATA(const XNode &xNode)
{
  if (auto &children = xNode.getChildren(); std::ranges::all_of(
        children, [](const XNode &element) {
        return (!(element.isElement()) || (element.isSelf()));
      })) {
    return (!xNode.getContents().empty());
  }
  return (false);
}

/// <summary>
/// Check whether an element does not contain any content (is empty).
/// </summary>
/// <param name="xNode">Current element XNode.</param>
/// <returns>true if element empty otherwise false.</returns>
bool DTD_Impl::checkIsEMPTY(const XNode &xNode) { return (xNode.getChildren().empty() || xNode.isSelf()); }

/// <summary>
///
/// Validate attribute value which can be:
///
/// value	    The default value of the attribute
/// #REQUIRED       The attribute is required
/// #IMPLIED	    The attribute is optional
/// #FIXED value    The attribute value is fixed
///
/// </summary>
/// <param name="xNode">Current element XNode.</param>
/// <param name="attribute">Attribute to check against.</param>
void DTD_Impl::checkAttributeValue(const XNode &xNode, const XDTD::Attribute &attribute) const
{
  const auto &xElement = XRef<XElement>(xNode);
  const bool attributePresent = xElement.isAttributePresent(attribute.name);
  if ((attribute.type & XDTD::AttributeType::required) != 0) {
    if (!attributePresent) { elementError(xElement, "is missing required attribute '" + attribute.name + "'."); }
  } else if ((attribute.type & XDTD::AttributeType::fixed) != 0) {
    if (attributePresent) {
      if (const XMLAttribute elementAttribute = xElement[attribute.name];
          attribute.value.getParsed() != elementAttribute.getParsed()) {
        elementError(xElement,
          "attribute '" + attribute.name + "' is '" + elementAttribute.getParsed() + "' instead of '"
            + attribute.value.getParsed() + "'.");
      }
    }
  }
  if (!attributePresent) {
    xElement.addAttribute(attribute.name, { attribute.value.getParsed(), attribute.value.getParsed() });
  }
}

/// <summary>
///
/// Validate a elements attribute type which can be one of the following.
///
/// CDATA	        The value is character data
/// (en1|en2|..)  The value must be one from an enumerated list
/// ID	          The value is a unique id
/// IDREF         The value is the id of another element
/// IDREFS        The value is a list of other ids
/// NMTOKEN       The value is a valid DTD name
/// NMTOKENS	    The value is a list of valid DTD names
/// ENTITY        The value is an entity
/// ENTITIES	    The value is a list of entities
/// NOTATION	    The value is a name of a notation
/// xml:          The value is a predefined xml value
///
/// </summary>
/// <param name="xNode">Current element XNode.</param>
void DTD_Impl::checkAttributeType(const XNode &xNode, const XDTD::Attribute &attribute)
{
  const auto &xElement = XRef<XElement>(xNode);
  auto &elementAttribute = xElement[attribute.name];
  if ((attribute.type & XDTD::AttributeType::cdata) != 0) {
    if (elementAttribute.getParsed().empty())// No character data present.
    {
      elementError(xElement, "attribute '" + attribute.name + "' does not contain character data.");
    }
  } else if ((attribute.type & XDTD::AttributeType::id) != 0) {
    if (!checkIsIDOK(elementAttribute.getParsed())) {
      elementError(xElement, "ID attribute '" + attribute.name + "' is invalid.");
    }
    if (assignedIDValues.contains(elementAttribute.getParsed())) {
      elementError(xElement, "ID attribute '" + attribute.name + "' is not unique.");
    }
    assignedIDValues.insert(elementAttribute.getParsed());
  } else if ((attribute.type & XDTD::AttributeType::idref) != 0) {
    if (!checkIsIDOK(elementAttribute.getParsed())) {
      elementError(xElement, "IDREF attribute '" + attribute.name + "' is invalid.");
    }
    assignedIDREFValues.insert(elementAttribute.getParsed());
  } else if ((attribute.type & XDTD::AttributeType::idrefs) != 0) {
    for (const auto &id : splitString(elementAttribute.getParsed(), ' ')) {
      if (!checkIsIDOK(id)) {
        elementError(xElement, "IDREFS attribute '" + attribute.name + "' contains an invalid IDREF.");
      }
      assignedIDREFValues.insert(id);
    }
  } else if ((attribute.type & XDTD::AttributeType::nmtoken) != 0) {
    if (!checkIsNMTOKENOK(elementAttribute.getParsed())) {
      elementError(xElement, "NMTOKEN attribute '" + attribute.name + "' is invalid.");
    }
  } else if ((attribute.type & XDTD::AttributeType::nmtokens) != 0) {
    for (auto &nmtoken : splitString(elementAttribute.getParsed(), ' ')) {
      if (!checkIsNMTOKENOK(nmtoken)) {
        elementError(xElement, "NMTOKEN attribute '" + attribute.name + "' contains an invalid NMTOKEN.");
      }
    }
  } else if ((attribute.type & XDTD::AttributeType::entity) != 0) {
    if (!xDTD.getEntityMapper().isPresent("&" + elementAttribute.getParsed() + ";")) {
      elementError(xElement,
        "ENTITY attribute '" + attribute.name + "' value '" + elementAttribute.getParsed() + "' is not defined.");
    }
  } else if ((attribute.type & XDTD::AttributeType::entities) != 0) {
    for (const auto &entity : splitString(elementAttribute.getParsed(), ' ')) {
      if (!xDTD.getEntityMapper().isPresent("&" + entity + ";")) {
        elementError(xElement, "ENTITIES attribute '" + attribute.name + "' value '" + entity + "' is not defined.");
      }
    }
  } else if ((attribute.type & XDTD::AttributeType::notation) != 0) {
    std::set<std::string> notations;
    for (auto &notation : splitString(attribute.enumeration.substr(1, attribute.enumeration.size() - 2), '|')) {
      notations.insert(notation);
    }
    if (!notations.contains(elementAttribute.getParsed())) {
      elementError(xElement,
        "NOTATION attribute '" + attribute.name + "' value '" + elementAttribute.getParsed() + "' is not defined.");
    }
  } else if ((attribute.type & XDTD::AttributeType::enumeration) != 0) {
    std::set<std::string> enumeration;
    for (auto &option : splitString(attribute.enumeration.substr(1, attribute.enumeration.size() - 2), '|')) {
      enumeration.insert(option);
    }
    if (!enumeration.contains(elementAttribute.getParsed())) {
      elementError(xElement,
        "attribute '" + attribute.name + "' contains invalid enumeration value '" + elementAttribute.getParsed()
          + "'.");
    }
  }
}

/// <summary>
/// Check element has the correct attribute type(s) and value(s) associated with
/// it.
/// </summary>
/// <param name="xNode">Current element XNode.</param>
void DTD_Impl::checkAttributes(const XNode &xNode)
{
  for (const auto &xElement = XRef<XElement>(xNode); auto &attribute : xDTD.getElement(xElement.name()).attributes) {
    if (xElement.isAttributePresent(attribute.name)) { checkAttributeType(xNode, attribute); }
    checkAttributeValue(xNode, attribute);
  }
}

/// <summary>
/// Check elements structure.
/// </summary>
/// <param name="xNode">Current element XNode.</param>
void DTD_Impl::checkContentSpecification(const XNode &xNode)
{
  const auto &xElement = XRef<XElement>(xNode);
  if (xDTD.getElementCount() == 0) { return; }
  if (xDTD.getElement(xElement.name()).content.getParsed() == "((<#PCDATA>))") {
    if (!checkIsPCDATA(xNode)) { elementError(xElement, "does not contain just any parsable data."); }
    return;
  }
  if (xDTD.getElement(xElement.name()).content.getParsed() == "EMPTY") {
    if (!checkIsEMPTY(xNode)) { elementError(xElement, "is not empty."); }
    return;
  }
  if (xDTD.getElement(xElement.name()).content.getParsed() == "ANY") { return; }
  const std::regex match{ xDTD.getElement(xElement.name()).content.getParsed() };
  std::string elements;
  for (auto &element : xElement.getChildren()) {
    if ((element.isElement()) || (element.isSelf())) {
      elements += "<" + XRef<XElement>(element).name() + ">";
    } else if (element.isContent()) {
      if (!XRef<XContent>(element).isWhiteSpace()) { elements += "<#PCDATA>"; }
    }
  }
  if (!std::regex_match(elements, match)) {
    elementError(xElement,
      "does not conform to the content specification " + xDTD.getElement(xElement.name()).content.getUnparsed() + ".");
  }
}

/// <summary>
/// Check elements content and associated attributes.
/// </summary>
/// <param name="xNode">Current element XNode.</param>
void DTD_Impl::checkElement(const XNode &xNode)
{
  checkContentSpecification(xNode);
  checkAttributes(xNode);
}

/// <summary>
/// Recursively check elements of XML document.
/// </summary>
/// <param name="xNode">Current element XNode.</param>
void DTD_Impl::checkElements(const XNode &xNode)
{
  if (xNode.isProlog()) {
    for (auto &element : xNode.getChildren()) { checkElements(element); }
  } else if (xNode.isDeclaration()) {
    // Nothing for present
    ;
  } else if (xNode.isRoot() || xNode.isElement()) {
    if (xNode.isRoot() && XRef<XElement>(xNode).name() != xDTD.getRootName()) {
      throw ValidationError(
        lineNumber, "DOCTYPE name does not match that of root element " + XRef<XElement>(xNode).name() + " of DTD.");
    }
    checkElement(xNode);
    for (auto &element : xNode.getChildren()) { checkElements(element); }
  } else if (xNode.isSelf()) {
    checkElement(xNode);
  } else if (xNode.isComment() || xNode.isEntity() || xNode.isPI() || xNode.isCDATA() || xNode.isDTD()) {
    // Nothing for present
    ;
  } else if (xNode.isContent()) {
    for (const auto &ch : XRef<XContent>(xNode).getContent()) {
      if (ch == kLineFeed) { lineNumber++; }
    }
  } else {
    throw ValidationError(lineNumber, "Invalid XMLNode encountered during validation.");
  }
}

/// <summary>
/// Check XML element by element and then check all ID values reference an
/// element.
/// </summary>
/// <param name="xNode">XNode element containing root of XML to validate.</param>
void DTD_Impl::checkAgainstDTD(const XNode &xNode)
{
  lineNumber = xDTD.getLineCount();
  checkElements(xNode);
  for (const auto &idref : assignedIDREFValues) {
    if (!assignedIDValues.contains(idref)) {
      throw ValidationError(lineNumber, "IDREF attribute '" + idref + "' does not reference any element with the ID.");
    }
  }
}

/// <summary>
/// Validate XML against its DTD. Throwing an exception if there is a
/// issue with the XML that is being validated.
/// </summary>
/// <param name="xNode">XNode element containing root of XML to validate.</param>
void DTD_Impl::validate(const XNode &xNode) { checkAgainstDTD(xNode); }
}// namespace XML_Lib