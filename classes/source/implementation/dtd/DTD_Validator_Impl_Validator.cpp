//
// Class: DTD_Impl_Validator
//
// Description: DTD validator.
//
// Dependencies: C++20 - Language standard features used.
//

#include "DTD_Impl.hpp"

#include <regex>
#include <set>
#include <string>
#include <string_view>

namespace XML_Lib {

namespace {
/// @brief
/// Implementation of buildEnumerationSet.

static std::set<std::string> buildEnumerationSet(const std::string &enumStr)
{
  std::set<std::string> result;
  for (auto &item : splitString(enumStr.substr(1, enumStr.size() - 2), '|')) { result.insert(item); }
  return result;
}
} // namespace

/// @brief
/// Generate an exception for an element error.

/// @param xElement Element X Node.
/// @param error Error text string.
void DTD_Impl::elementError(const Element &xElement, const std::string_view &error) const
{
  XML_LIB_THROW(ValidationError(lineNumber, "Element <" + xElement.name() + "> " + std::string(error)));
}

/// @brief
/// Check whether a token value is valid.

/// @param nmTokenValue Token value.
/// @return True then token is valid otherwise false.
bool DTD_Impl::checkIsNMTOKENOK(const std::string_view &nmTokenValue)
{
  BufferSource nmTokenValueSource(trimString(nmTokenValue));
  while (nmTokenValueSource.more()) {
    if (!validNameChar(nmTokenValueSource.current())) { return false; }
    nmTokenValueSource.next();
  }
  return true;
}

/// @brief
/// Check whether ID value is valid.

/// @param idValue ID string value.
/// @return True then ID is valid otherwise false.
bool DTD_Impl::checkIsIDOK(const std::string_view &idValue)
{
  try {
    BufferSource idSource(idValue);
    auto name = parseName(idSource);
  } catch (std::exception &) {
    return false;
  }
  return true;
}

/// @brief
/// Check whether element contains characters.

/// @param xNode Current element Node.
/// @return true if element contains characters otherwise false.
bool DTD_Impl::checkIsPCDATA(const Node &xNode)
{
  if (auto &child = xNode.getChildren();
      std::ranges::all_of(child, [](const Node &element) { return !isA<Element>(element) || isA<Self>(element); })) {
    return !xNode.getContents().empty();
  }
  return false;
}

/// @brief
/// Check whether an element does not contain any content (is empty).

/// @param xNode Current element Node.
/// @return true if element empty otherwise false.
bool DTD_Impl::checkIsEMPTY(const Node &xNode) { return xNode.getChildren().empty() || isA<Self>(xNode); }

/// @brief
///
/// Validate attribute value which can be:
///
/// value	    The default value of the attribute
/// #REQUIRED       The attribute is required
/// #IMPLIED	    The attribute is optional
/// #FIXED value    The attribute value is fixed
///

/// @param xNode Current element Node.
/// @param attribute Attribute to check against.
void DTD_Impl::checkAttributeValue(const Node &xNode, const DTD::Attribute &attribute) const
{
  const auto &xElement = NRef<Element>(xNode);
  const bool attributePresent = xElement.hasAttribute(attribute.name);
  if ((attribute.type & DTD::AttributeType::required) != 0) {
    if (!attributePresent) { elementError(xElement, "is missing required attribute '" + attribute.name + "'."); }
  } else if ((attribute.type & DTD::AttributeType::fixed) != 0) {
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
    xElement.addAttribute(attribute.name, XMLValue(attribute.value.getParsed(), attribute.value.getParsed()));
  }
}

/// @brief
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

/// @param xNode Current element Node.
/// @param attribute Attribute to check against.
void DTD_Impl::checkAttributeType(const Node &xNode, const DTD::Attribute &attribute)
{
  const auto &xElement = NRef<Element>(xNode);
  auto &elementAttribute = xElement[attribute.name];
  if ((attribute.type & DTD::AttributeType::cdata) != 0) {
    if (elementAttribute.getParsed().empty())// No character data present.
    {
      elementError(xElement, "attribute '" + attribute.name + "' does not contain character data.");
    }
  } else if ((attribute.type & DTD::AttributeType::id) != 0) {
    if (!checkIsIDOK(elementAttribute.getParsed())) {
      elementError(xElement, "ID attribute '" + attribute.name + "' is invalid.");
    }
    if (assignedIDValues.contains(elementAttribute.getParsed())) {
      elementError(xElement, "ID attribute '" + attribute.name + "' is not unique.");
    }
    assignedIDValues.insert(elementAttribute.getParsed());
  } else if ((attribute.type & DTD::AttributeType::idref) != 0) {
    if (!checkIsIDOK(elementAttribute.getParsed())) {
      elementError(xElement, "IDREF attribute '" + attribute.name + "' is invalid.");
    }
    assignedIDREFValues.insert(elementAttribute.getParsed());
  } else if ((attribute.type & DTD::AttributeType::idrefs) != 0) {
    for (const auto &id : splitString(elementAttribute.getParsed(), ' ')) {
      if (!checkIsIDOK(id)) {
        elementError(xElement, "IDREFS attribute '" + attribute.name + "' contains an invalid IDREF.");
      }
      assignedIDREFValues.insert(id);
    }
  } else if ((attribute.type & DTD::AttributeType::nmtoken) != 0) {
    if (!checkIsNMTOKENOK(elementAttribute.getParsed())) {
      elementError(xElement, "NMTOKEN attribute '" + attribute.name + "' is invalid.");
    }
  } else if ((attribute.type & DTD::AttributeType::nmtokens) != 0) {
    for (auto &nmtoken : splitString(elementAttribute.getParsed(), ' ')) {
      if (!checkIsNMTOKENOK(nmtoken)) {
        elementError(xElement, "NMTOKEN attribute '" + attribute.name + "' contains an invalid NMTOKEN.");
      }
    }
  } else if ((attribute.type & DTD::AttributeType::entity) != 0
             || (attribute.type & DTD::AttributeType::entities) != 0) {
    const bool isEntities = (attribute.type & DTD::AttributeType::entities) != 0;
    const std::string typeLabel = isEntities ? "ENTITIES" : "ENTITY";
    const auto checkEntity = [&](const std::string &entityName) {
      if (!xDTD.getEntityMapper().isPresent("&" + entityName + ";")) {
        elementError(xElement,
          typeLabel + " attribute '" + attribute.name + "' value '" + entityName + "' is not defined.");
      }
    };
    if (isEntities) {
      for (const auto &entity : splitString(elementAttribute.getParsed(), ' ')) { checkEntity(entity); }
    } else {
      checkEntity(elementAttribute.getParsed());
    }
  } else if ((attribute.type & DTD::AttributeType::notation) != 0) {
    if (!buildEnumerationSet(attribute.enumeration).contains(elementAttribute.getParsed())) {
      elementError(xElement,
        "NOTATION attribute '" + attribute.name + "' value '" + elementAttribute.getParsed() + "' is not defined.");
    }
  } else if ((attribute.type & DTD::AttributeType::enumeration) != 0) {
    if (!buildEnumerationSet(attribute.enumeration).contains(elementAttribute.getParsed())) {
      elementError(xElement,
        "attribute '" + attribute.name + "' contains invalid enumeration value '" + elementAttribute.getParsed()
          + "'.");
    }
  }
}

/// @brief
/// Check element has the correct attribute type(s) and value(s) associated with
/// it.

/// @param xNode Current element Node.
void DTD_Impl::checkAttributes(const Node &xNode)
{
  for (const auto &xElement = NRef<Element>(xNode); auto &attribute : xDTD.getElement(xElement.name()).attributes) {
    if (xElement.hasAttribute(attribute.name)) { checkAttributeType(xNode, attribute); }
    checkAttributeValue(xNode, attribute);
  }
}

/// @brief
/// Check elements structure.

/// @param xNode Current element Node.
void DTD_Impl::checkContentSpecification(const Node &xNode) const
{
  const auto &xElement = NRef<Element>(xNode);
  if (xDTD.getElementCount() == 0) { return; }
  const auto &elemDecl = xDTD.getElement(xElement.name());
  if (elemDecl.content.getParsed() == "((<#PCDATA>))") {
    if (!checkIsPCDATA(xNode)) { elementError(xElement, "does not contain just any parsable data."); }
    return;
  }
  if (elemDecl.content.getParsed() == "EMPTY") {
    if (!checkIsEMPTY(xNode)) { elementError(xElement, "is not empty."); }
    return;
  }
  if (elemDecl.content.getParsed() == "ANY") { return; }
  const std::regex match{ elemDecl.content.getParsed() };
  std::string elements;
  for (auto &child : xElement.getChildren()) {
    if (isA<Element>(child) || isA<Self>(child)) {
      elements += "<" + NRef<Element>(child).name() + ">";
    } else if (isA<Content>(child)) {
      if (!NRef<Content>(child).isWhiteSpace()) { elements += "<#PCDATA>"; }
    }
  }
  if (!std::regex_match(elements, match)) {
    elementError(xElement,
      "does not conform to the content specification " + elemDecl.content.getUnparsed() + ".");
  }
}

/// @brief
/// Check elements content and associated attributes.

/// @param xNode Current element Node.
void DTD_Impl::checkElement(const Node &xNode)
{
  checkContentSpecification(xNode);
  checkAttributes(xNode);
}

/// @brief
/// Handle a Prolog node: recurse into its children.

void DTD_Impl::handlePrologNode(const Node &xNode)
{
  for (auto &child : xNode.getChildren()) { checkElements(child); }
}

/// @brief
/// Handle a Root or Element node: validate the element then recurse.

void DTD_Impl::handleElementNode(const Node &xNode)
{
  if (isA<Root>(xNode) && NRef<Element>(xNode).name() != xDTD.getRootName()) {
    XML_LIB_THROW(ValidationError(
      lineNumber, "DOCTYPE name does not match that of root element " + NRef<Element>(xNode).name() + " of DTD."));
  }
  checkElement(xNode);
  for (auto &child : xNode.getChildren()) { checkElements(child); }
}

/// @brief
/// Handle a Self-closing element node: validate the element only.

void DTD_Impl::handleSelfNode(const Node &xNode) { checkElement(xNode); }

/// @brief
/// Handle a Content node: count line feeds for error reporting.

void DTD_Impl::handleContentNode(const Node &xNode)
{
  for (const auto &ch : NRef<Content>(xNode).value()) {
    if (ch == kLineFeed) { lineNumber++; }
  }
}

/// @brief
/// Handle nodes that need no validation action (Comment, EntityReference, PI,
/// CDATA, DTD).

void DTD_Impl::handleIgnorableNode([[maybe_unused]] const Node &xNode) {}

/// @brief
/// Recursively check elements of XML document.

/// @param xNode Current element Node.
void DTD_Impl::checkElements(const Node &xNode)
{
  if (isA<Prolog>(xNode)) {
    handlePrologNode(xNode);
  } else if (isA<Declaration>(xNode)) {
    handleIgnorableNode(xNode);
  } else if (isA<Root>(xNode) || isA<Element>(xNode)) {
    handleElementNode(xNode);
  } else if (isA<Self>(xNode)) {
    handleSelfNode(xNode);
  } else if (isA<Comment>(xNode) || isA<EntityReference>(xNode) || isA<PI>(xNode) || isA<CDATA>(xNode)
             || isA<DTD>(xNode)) {
    handleIgnorableNode(xNode);
  } else if (isA<Content>(xNode)) {
    handleContentNode(xNode);
  } else {
    XML_LIB_THROW(ValidationError(lineNumber, "Invalid XMLNode encountered during validation."));
  }
}

/// @brief
/// Check XML element by element and then check all ID values reference an
/// element.

/// @param xNode Node element containing root of XML to validate.
void DTD_Impl::checkAgainstDTD(const Node &xNode)
{
  lineNumber = static_cast<long>(xDTD.getLineCount());
  checkElements(xNode);
  for (const auto &idref : assignedIDREFValues) {
    if (!assignedIDValues.contains(idref)) {
      XML_LIB_THROW(ValidationError(lineNumber, "IDREF attribute '" + idref + "' does not reference any element with the ID."));
    }
  }
}

/// @brief
/// Validate XML against its DTD. Throwing an exception if there is a
/// issue with the XML that is being validated.

/// @param xNode Node element containing root of XML to validate.
void DTD_Impl::validate(const Node &xNode) { checkAgainstDTD(xNode); }
}// namespace XML_Lib