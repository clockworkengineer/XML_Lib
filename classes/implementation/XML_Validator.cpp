//
// Class: XML_Validator
//
// Description: XML DTD validator.
//
// Dependencies:   C20++ - Language standard features used.
//
// =================
// CLASS DEFINITIONS
// =================
#include "XML_Validator.hpp"
// ====================
// CLASS IMPLEMENTATION
// ====================
// =================
// LIBRARY NAMESPACE
// =================
namespace XML_Lib {
// ===========================
// PRIVATE TYPES AND CONSTANTS
// ===========================
// ==========================
// PUBLIC TYPES AND CONSTANTS
// ==========================
// ========================
// PRIVATE STATIC VARIABLES
// ========================
// =======================
// PUBLIC STATIC VARIABLES
// =======================
// ===============
// PRIVATE METHODS
// ===============
/// <summary>
/// Generate an exception for an element error.
/// </summary>
/// <param name="xNodeElement">Element X Node.</param>
/// <param name="error">Error text string.</param>
void XML_Validator::elementError(const XNodeElement &xNodeElement, const std::string &error)
{
  throw ValidationError(m_lineNumber, "Element <" + xNodeElement.name() + "> " + error);
}
/// <summary>
/// Check whether a token value is valid.
/// </summary>
/// <param name="nmTokenValue">Token value</param>
/// <returns>true then token is valid otherwise false.</returns>
bool XML_Validator::checkIsNMTOKENOK(const std::string &nmTokenValue)
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
bool XML_Validator::checkIsIDOK(const std::string &idValue)
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
/// <param name="xNode">Current element node.</param>
/// <returns>true if element contains characters otherwise false.</returns>
bool XML_Validator::checkIsPCDATA(const XNode &xNode)
{
  for (const auto &element : xNode.getChildren()) {
    if ((element->getNodeType() == XNode::Type::element) || (element->getNodeType() == XNode::Type::self)) {
      return (false);
    }
  }
  return (!xNode.getContents().empty());
}
/// <summary>
/// Check whether an element does not contain any content (is empty).
/// </summary>
/// <param name="xNode">Current element node.</param>
/// <returns>true if element empty otherwise false.</returns>
bool XML_Validator::checkIsEMPTY(const XNode &xNode)
{
  return (xNode.getChildren().empty() || xNode.getNodeType() == XNode::Type::self);
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
/// <param name="xNode">Current element node.</param>
void XML_Validator::checkAttributeValue(const XNode &xNode, const XNodeDTD::Attribute &attribute)
{
  const XNodeElement &xNodeElement = XRef<XNodeElement>(xNode);
  bool attributePresent = xNodeElement.isAttributePresent(attribute.name);
  if ((attribute.type & XNodeDTD::AttributeType::required) != 0) {
    if (!attributePresent) { elementError(xNodeElement, "is missing required attribute '" + attribute.name + "'."); }
  } else if ((attribute.type & XNodeDTD::AttributeType::fixed) != 0) {
    if (attributePresent) {
      XMLAttribute elementAttribute = xNodeElement.getAttribute(attribute.name);
      if (attribute.value.parsed != elementAttribute.value.parsed) {
        elementError(xNodeElement,
          "attribute '" + attribute.name + "' is '" + elementAttribute.value.parsed + "' instead of '"
            + attribute.value.parsed + "'.");
      }
    }
  }
  if (!attributePresent) {
    xNodeElement.addAttribute(attribute.name, { attribute.value.parsed, attribute.value.parsed });
  }
}
/// <summary>
///
/// Validate a elements attribute type which can be one of the following.
///
/// CDATA	      The value is character data
/// (en1|en2|..)  The value must be one from an enumerated list
/// ID	          The value is a unique id
/// IDREF         The value is the id of another element
/// IDREFS        The value is a list of other ids
/// NMTOKEN       The value is a valid DTD name
/// NMTOKENS	  The value is a list of valid DTD names
/// ENTITY        The value is an entity
/// ENTITIES	  The value is a list of entities
/// NOTATION	  The value is a name of a notation
/// xml:          The value is a predefined xml value
///
/// </summary>
/// <param name="xNode">Current element node.</param>
void XML_Validator::checkAttributeType(const XNode &xNode, const XNodeDTD::Attribute &attribute)
{
  const XNodeElement &xNodeElement = XRef<XNodeElement>(xNode);
  XMLAttribute elementAttribute = xNodeElement.getAttribute(attribute.name);
  if ((attribute.type & XNodeDTD::AttributeType::cdata) != 0) {
    if (elementAttribute.value.parsed.empty())// No character data present.
    {
      elementError(xNodeElement, "attribute '" + attribute.name + "' does not contain character data.");
    }
  } else if ((attribute.type & XNodeDTD::AttributeType::id) != 0) {
    if (!checkIsIDOK(elementAttribute.value.parsed)) {
      elementError(xNodeElement, "ID attribute '" + attribute.name + "' is invalid.");
    }
    if (m_assignedIDValues.count(elementAttribute.value.parsed) > 0) {
      elementError(xNodeElement, "ID attribute '" + attribute.name + "' is not unique.");
    }
    m_assignedIDValues.insert(elementAttribute.value.parsed);
  } else if ((attribute.type & XNodeDTD::AttributeType::idref) != 0) {
    if (!checkIsIDOK(elementAttribute.value.parsed)) {
      elementError(xNodeElement, "IDREF attribute '" + attribute.name + "' is invalid.");
    }
    m_assignedIDREFValues.insert(elementAttribute.value.parsed);
  } else if ((attribute.type & XNodeDTD::AttributeType::idrefs) != 0) {
    for (auto &id : splitString(elementAttribute.value.parsed, ' ')) {
      if (!checkIsIDOK(id)) {
        elementError(xNodeElement, "IDREFS attribute '" + attribute.name + "' contains an invalid IDREF.");
      }
      m_assignedIDREFValues.insert(id);
    }
  } else if ((attribute.type & XNodeDTD::AttributeType::nmtoken) != 0) {
    if (!checkIsNMTOKENOK(elementAttribute.value.parsed)) {
      elementError(xNodeElement, "NMTOKEN attribute '" + attribute.name + "' is invalid.");
    }
  } else if ((attribute.type & XNodeDTD::AttributeType::nmtokens) != 0) {
    for (auto &nmtoken : splitString(elementAttribute.value.parsed, ' ')) {
      if (!checkIsNMTOKENOK(nmtoken)) {
        elementError(xNodeElement, "NMTOKEN attribute '" + attribute.name + "' contains an invalid NMTOKEN.");
      }
    }
  } else if ((attribute.type & XNodeDTD::AttributeType::entity) != 0) {
    if (!m_xNodeDTD.m_entityMapper.isPresent("&" + elementAttribute.value.parsed + ";")) {
      elementError(xNodeElement,
        "ENTITY attribute '" + attribute.name + "' value '" + elementAttribute.value.parsed + "' is not defined.");
    }
  } else if ((attribute.type & XNodeDTD::AttributeType::entities) != 0) {
    for (auto &entity : splitString(elementAttribute.value.parsed, ' ')) {
      if (!m_xNodeDTD.m_entityMapper.isPresent("&" + entity + ";")) {
        elementError(
          xNodeElement, "ENTITIES attribute '" + attribute.name + "' value '" + entity + "' is not defined.");
      }
    }
  } else if ((attribute.type & XNodeDTD::AttributeType::notation) != 0) {
    std::set<std::string> notations;
    for (auto &notation : splitString(attribute.enumeration.substr(1, attribute.enumeration.size() - 2), '|')) {
      notations.insert(notation);
    }
    if (notations.count(elementAttribute.value.parsed) == 0) {
      elementError(xNodeElement,
        "NOTATION attribute '" + attribute.name + "' value '" + elementAttribute.value.parsed + "' is not defined.");
    }
  } else if ((attribute.type & XNodeDTD::AttributeType::enumeration) != 0) {
    std::set<std::string> enumeration;
    for (auto &option : splitString(attribute.enumeration.substr(1, attribute.enumeration.size() - 2), '|')) {
      enumeration.insert(option);
    }
    if (enumeration.find(elementAttribute.value.parsed) == enumeration.end()) {
      elementError(xNodeElement,
        "attribute '" + attribute.name + "' contains invalid enumeration value '" + elementAttribute.value.parsed
          + "'.");
    }
  }
}
/// <summary>
/// Check element has the correct attribute type(s) and value(s) associated with
/// it.
/// </summary>
/// <param name="xNode">Current element node.</param>
void XML_Validator::checkAttributes(const XNode &xNode)
{
  const XNodeElement &xNodeElement = XRef<XNodeElement>(xNode);
  for (auto &attribute : m_xNodeDTD.getElement(xNodeElement.name()).attributes) {
    if (xNodeElement.isAttributePresent(attribute.name)) { checkAttributeType(xNode, attribute); }
    checkAttributeValue(xNode, attribute);
  }
}
/// <summary>
/// Check elements structure.
/// </summary>
/// <param name="xNode">Current element node.</param>
void XML_Validator::checkContentSpecification(const XNode &xNode)
{
  const XNodeElement &xNodeElement = XRef<XNodeElement>(xNode);
  if (m_xNodeDTD.getElementCount() == 0) { return; }
  if (m_xNodeDTD.getElement(xNodeElement.name()).content.parsed == "((<#PCDATA>))") {
    if (!checkIsPCDATA(xNode)) { elementError(xNodeElement, "does not contain just any parsable data."); }
    return;
  }
  if (m_xNodeDTD.getElement(xNodeElement.name()).content.parsed == "EMPTY") {
    if (!checkIsEMPTY(xNode)) { elementError(xNodeElement, "is not empty."); }
    return;
  }
  if (m_xNodeDTD.getElement(xNodeElement.name()).content.parsed == "ANY") { return; }
  std::regex match{ m_xNodeDTD.getElement(xNodeElement.name()).content.parsed };
  std::string elements;
  for (auto &element : xNodeElement.getChildren()) {
    if ((element->getNodeType() == XNode::Type::element) || (element->getNodeType() == XNode::Type::self)) {
      elements += "<" + XRef<XNodeElement>(*element).name() + ">";
    } else if (element->getNodeType() == XNode::Type::content) {
      if (!XRef<XNodeContent>(*element).isWhiteSpace()) { elements += "<#PCDATA>"; }
    }
  }
  if (!std::regex_match(elements, match)) {
    elementError(xNodeElement,
      "does not conform to the content specification " + m_xNodeDTD.getElement(xNodeElement.name()).content.unparsed
        + ".");
  }
}
/// <summary>
/// Check elements content and associated attributes.
/// </summary>
/// <param name="xNode">Current element node.</param>
void XML_Validator::checkElement(const XNode &xNode)
{
  checkContentSpecification(xNode);
  checkAttributes(xNode);
}
/// <summary>
/// Recursively check elements of XML document.
/// </summary>
/// <param name="xNode">Current element node.</param>
void XML_Validator::checkElements(const XNode &xNode)
{
  switch (xNode.getNodeType()) {
  case XNode::Type::prolog:
    for (auto &element : xNode.getChildren()) { checkElements(*element); }
    break;
  case XNode::Type::declaration:
    // Nothing for present
    break;
  case XNode::Type::root:
  case XNode::Type::element:
    if (xNode.getNodeType() == XNode::Type::root && XRef<XNodeElement>(xNode).name() != m_xNodeDTD.getRootName()) {
      throw ValidationError(m_lineNumber,
        "DOCTYPE name does not match that of root element " + XRef<XNodeElement>(xNode).name() + " of DTD.");
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
    for (auto &ch : XRef<XNodeContent>(xNode).content()) {
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
/// <param name=""></param>
/// <param name="prolog">Prolog element containing root of XML to validate.</param>
void XML_Validator::checkAgainstDTD(const XNode &prolog)
{
  m_lineNumber = m_xNodeDTD.getLineCount();
  checkElements(prolog);
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
/// <param name="prolog">Prolog element containing root of XML to validate.</param>
void XML_Validator::validate(const XNode &prolog) { checkAgainstDTD(prolog); }
}// namespace XML_Lib