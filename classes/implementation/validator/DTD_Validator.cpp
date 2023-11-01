//
// Class: DTD_Validator
//
// Description: XML DTD validator.
//
// Dependencies:   C++20 - Language standard features used.
//

#include "DTD_Validator.hpp"

namespace XML_Lib {

/// <summary>
/// Generate an exception for an element error.
/// </summary>
/// <param name="xElement">Element X Node.</param>
/// <param name="error">Error text string.</param>
void DTD_Validator::elementError(const XElement &xElement, const std::string &error)
{
  throw ValidationError(m_lineNumber, "Element <" + xElement.name() + "> " + error);
}

/// <summary>
/// Check whether a token value is valid.
/// </summary>
/// <param name="nmTokenValue">Token value.</param>
/// <returns>true then token is valid otherwise false.</returns>
bool DTD_Validator::checkIsNMTOKENOK(const std::string &nmTokenValue)
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
bool DTD_Validator::checkIsIDOK(const std::string &idValue)
{
  try {
    BufferSource idSource(idValue);
    parseName(idSource);
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
bool DTD_Validator::checkIsPCDATA(const XNode &xNode)
{
  for (const auto &element : xNode.getChildren()) {
    if ((element->getType() == XNode::Type::element) || (element->getType() == XNode::Type::self)) { return (false); }
  }
  return (!xNode.getContents().empty());
}

/// <summary>
/// Check whether an element does not contain any content (is empty).
/// </summary>
/// <param name="xNode">Current element XNode.</param>
/// <returns>true if element empty otherwise false.</returns>
bool DTD_Validator::checkIsEMPTY(const XNode &xNode)
{
  return (xNode.getChildren().empty() || xNode.getType() == XNode::Type::self);
}

/// <summary>
///
/// Validate attribute value which can be:
///
/// value	        The default value of the attribute
/// #REQUIRED       The attribute is required
/// #IMPLIED	    The attribute is optional
/// #FIXED value	The attribute value is fixed
///
/// </summary>
/// <param name="xNode">Current element XNode.</param>
void DTD_Validator::checkAttributeValue(const XNode &xNode, const XDTD::Attribute &attribute)
{
  const XElement &xElement = XRef<XElement>(xNode);
  bool attributePresent = xElement.isAttributePresent(attribute.name);
  if ((attribute.type & XDTD::AttributeType::required) != 0) {
    if (!attributePresent) { elementError(xElement, "is missing required attribute '" + attribute.name + "'."); }
  } else if ((attribute.type & XDTD::AttributeType::fixed) != 0) {
    if (attributePresent) {
      XMLAttribute elementAttribute = xElement.getAttribute(attribute.name);
      if (attribute.value.getParsed() != elementAttribute.getValue()) {
        elementError(xElement,
          "attribute '" + attribute.name + "' is '" + elementAttribute.getValue() + "' instead of '"
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
void DTD_Validator::checkAttributeType(const XNode &xNode, const XDTD::Attribute &attribute)
{
  const XElement &xElement = XRef<XElement>(xNode);
  XMLAttribute elementAttribute = xElement.getAttribute(attribute.name);
  if ((attribute.type & XDTD::AttributeType::cdata) != 0) {
    if (elementAttribute.getValue().empty())// No character data present.
    {
      elementError(xElement, "attribute '" + attribute.name + "' does not contain character data.");
    }
  } else if ((attribute.type & XDTD::AttributeType::id) != 0) {
    if (!checkIsIDOK(elementAttribute.getValue())) {
      elementError(xElement, "ID attribute '" + attribute.name + "' is invalid.");
    }
    if (m_assignedIDValues.count(elementAttribute.getValue()) > 0) {
      elementError(xElement, "ID attribute '" + attribute.name + "' is not unique.");
    }
    m_assignedIDValues.insert(elementAttribute.getValue());
  } else if ((attribute.type & XDTD::AttributeType::idref) != 0) {
    if (!checkIsIDOK(elementAttribute.getValue())) {
      elementError(xElement, "IDREF attribute '" + attribute.name + "' is invalid.");
    }
    m_assignedIDREFValues.insert(elementAttribute.getValue());
  } else if ((attribute.type & XDTD::AttributeType::idrefs) != 0) {
    for (auto &id : splitString(elementAttribute.getValue(), ' ')) {
      if (!checkIsIDOK(id)) {
        elementError(xElement, "IDREFS attribute '" + attribute.name + "' contains an invalid IDREF.");
      }
      m_assignedIDREFValues.insert(id);
    }
  } else if ((attribute.type & XDTD::AttributeType::nmtoken) != 0) {
    if (!checkIsNMTOKENOK(elementAttribute.getValue())) {
      elementError(xElement, "NMTOKEN attribute '" + attribute.name + "' is invalid.");
    }
  } else if ((attribute.type & XDTD::AttributeType::nmtokens) != 0) {
    for (auto &nmtoken : splitString(elementAttribute.getValue(), ' ')) {
      if (!checkIsNMTOKENOK(nmtoken)) {
        elementError(xElement, "NMTOKEN attribute '" + attribute.name + "' contains an invalid NMTOKEN.");
      }
    }
  } else if ((attribute.type & XDTD::AttributeType::entity) != 0) {
    if (!m_xDTD.m_entityMapper.isPresent("&" + elementAttribute.getValue() + ";")) {
      elementError(xElement,
        "ENTITY attribute '" + attribute.name + "' value '" + elementAttribute.getValue() + "' is not defined.");
    }
  } else if ((attribute.type & XDTD::AttributeType::entities) != 0) {
    for (auto &entity : splitString(elementAttribute.getValue(), ' ')) {
      if (!m_xDTD.m_entityMapper.isPresent("&" + entity + ";")) {
        elementError(xElement, "ENTITIES attribute '" + attribute.name + "' value '" + entity + "' is not defined.");
      }
    }
  } else if ((attribute.type & XDTD::AttributeType::notation) != 0) {
    std::set<std::string> notations;
    for (auto &notation : splitString(attribute.enumeration.substr(1, attribute.enumeration.size() - 2), '|')) {
      notations.insert(notation);
    }
    if (notations.count(elementAttribute.getValue()) == 0) {
      elementError(xElement,
        "NOTATION attribute '" + attribute.name + "' value '" + elementAttribute.getValue() + "' is not defined.");
    }
  } else if ((attribute.type & XDTD::AttributeType::enumeration) != 0) {
    std::set<std::string> enumeration;
    for (auto &option : splitString(attribute.enumeration.substr(1, attribute.enumeration.size() - 2), '|')) {
      enumeration.insert(option);
    }
    if (enumeration.find(elementAttribute.getValue()) == enumeration.end()) {
      elementError(xElement,
        "attribute '" + attribute.name + "' contains invalid enumeration value '" + elementAttribute.getValue() + "'.");
    }
  }
}

/// <summary>
/// Check element has the correct attribute type(s) and value(s) associated with
/// it.
/// </summary>
/// <param name="xNode">Current element XNode.</param>
void DTD_Validator::checkAttributes(const XNode &xNode)
{
  const XElement &xElement = XRef<XElement>(xNode);
  for (auto &attribute : m_xDTD.getElement(xElement.name()).attributes) {
    if (xElement.isAttributePresent(attribute.name)) { checkAttributeType(xNode, attribute); }
    checkAttributeValue(xNode, attribute);
  }
}

/// <summary>
/// Check elements structure.
/// </summary>
/// <param name="xNode">Current element XNode.</param>
void DTD_Validator::checkContentSpecification(const XNode &xNode)
{
  const XElement &xElement = XRef<XElement>(xNode);
  if (m_xDTD.getElementCount() == 0) { return; }
  if (m_xDTD.getElement(xElement.name()).content.getParsed() == "((<#PCDATA>))") {
    if (!checkIsPCDATA(xNode)) { elementError(xElement, "does not contain just any parsable data."); }
    return;
  }
  if (m_xDTD.getElement(xElement.name()).content.getParsed() == "EMPTY") {
    if (!checkIsEMPTY(xNode)) { elementError(xElement, "is not empty."); }
    return;
  }
  if (m_xDTD.getElement(xElement.name()).content.getParsed() == "ANY") { return; }
  std::regex match{ m_xDTD.getElement(xElement.name()).content.getParsed() };
  std::string elements;
  for (auto &element : xElement.getChildren()) {
    if ((element->getType() == XNode::Type::element) || (element->getType() == XNode::Type::self)) {
      elements += "<" + XRef<XElement>(*element).name() + ">";
    } else if (element->getType() == XNode::Type::content) {
      if (!XRef<XContent>(*element).isWhiteSpace()) { elements += "<#PCDATA>"; }
    }
  }
  if (!std::regex_match(elements, match)) {
    elementError(xElement,
      "does not conform to the content specification " + m_xDTD.getElement(xElement.name()).content.getUnparsed()
        + ".");
  }
}

/// <summary>
/// Check elements content and associated attributes.
/// </summary>
/// <param name="xNode">Current element XNode.</param>
void DTD_Validator::checkElement(const XNode &xNode)
{
  checkContentSpecification(xNode);
  checkAttributes(xNode);
}

/// <summary>
/// Recursively check elements of XML document.
/// </summary>
/// <param name="xNode">Current element XNode.</param>
void DTD_Validator::checkElements(const XNode &xNode)
{
  switch (xNode.getType()) {
  case XNode::Type::prolog:
    for (auto &element : xNode.getChildren()) { checkElements(*element); }
    break;
  case XNode::Type::declaration:
    // Nothing for present
    break;
  case XNode::Type::root:
  case XNode::Type::element:
    if (xNode.getType() == XNode::Type::root && XRef<XElement>(xNode).name() != m_xDTD.getRootName()) {
      throw ValidationError(
        m_lineNumber, "DOCTYPE name does not match that of root element " + XRef<XElement>(xNode).name() + " of DTD.");
    }
    checkElement(xNode);
    for (auto &element : xNode.getChildren()) { checkElements(*element); }
    break;
  case XNode::Type::self:
    checkElement(xNode);
    break;
  case XNode::Type::comment:
  case XNode::Type::entity:
  case XNode::Type::pi:
  case XNode::Type::cdata:
  case XNode::Type::dtd:
    break;
  case XNode::Type::content:
    for (auto &ch : XRef<XContent>(xNode).getContent()) {
      if (ch == kLineFeed) { m_lineNumber++; }
    }
    break;
  default:
    throw ValidationError(m_lineNumber, "Invalid XMLNode encountered during validation.");
  }
}

/// <summary>
/// Check XML element by element and then check all ID values reference an
/// element.
/// </summary>
/// <param name="">.</param>
/// <param name="xNode">XNode element containing root of XML to validate.</param>
void DTD_Validator::checkAgainstDTD(const XNode &xNode)
{
  m_lineNumber = m_xDTD.getLineCount();
  checkElements(xNode);
  for (auto &idref : m_assignedIDREFValues) {
    if (m_assignedIDValues.count(idref) == 0) {
      throw ValidationError(
        m_lineNumber, "IDREF attribute '" + idref + "' does not reference any element with the ID.");
    }
  }
}

/// <summary>
/// Validate XML against its DTD. Throwing an exception if there is a
/// issue with the XML that is being validated.
/// </summary>
/// <param name="xNode">XNode element containing root of XML to validate.</param>
void DTD_Validator::validate(const XNode &xNode) { checkAgainstDTD(xNode); }
}// namespace XML_Lib