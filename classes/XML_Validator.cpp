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
#include "XML.hpp"
#include "XML_Types.hpp"
#include "XML_Nodes.hpp"
#include "XML_Core.hpp"
#include "XML_Validator.hpp"
#include "XML_Errors.hpp"
#include "XML_Sources.hpp"
#include "XML_Destinations.hpp"
#include "IXMLEntityMapper.hpp"
// ====================
// CLASS IMPLEMENTATION
// ====================
// =========
// NAMESPACE
// =========
namespace XMLLib
{
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
    void XML_Validator::elementError(const XMLNodeElement &xNodeElement, const std::string &error)
    {
        throw ValidationError(m_lineNumber, "Element <" + xNodeElement.elementName + "> " + error);
    }
    /// <summary>
    /// Check whether a token value is valid.
    /// </summary>
    /// <param name="nmTokenValue">Token value</param>
    /// <returns>true then token is valid otherwise false.</returns>
    bool XML_Validator::checkIsNMTOKENOK(const std::string &nmTokenValue)
    {
        BufferSource nmTokenValueSource(Core::trimmString(nmTokenValue));
        while (nmTokenValueSource.more())
        {
            if (!Core::validNameChar(nmTokenValueSource.current()))
            {
                return (false);
            }
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
        try
        {
            BufferSource idSource(idValue);
            Core::parseName(idSource);
        }
        catch (std::exception &)
        {
            return (false);
        }
        return (true);
    }
    /// <summary>
    /// Check whether element contains characters.
    /// </summary>
    /// <param name="xmlNode">Current element node.</param>
    /// <returns>true if element contains characters otherwise false.</returns>
    bool XML_Validator::checkIsPCDATA(XMLNode &xmlNode)
    {
        for (const auto &element : xmlNode.children)
        {
            if ((element->getNodeType() == XMLNodeType::element) || (element->getNodeType() == XMLNodeType::self))
            {
                return (false);
            }
        }
        return (!xmlNode.getContents().empty());
    }
    /// <summary>
    /// Check whether an element does not contain any content (is empty).
    /// </summary>
    /// <param name="xmlNode">Current element node.</param>
    /// <returns>true if element empty otherwise false.</returns>
    bool XML_Validator::checkIsEMPTY(XMLNode &xmlNode)
    {
        return (xmlNode.children.empty() || xmlNode.getNodeType() == XMLNodeType::self);
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
    /// <param name="xmlNode">Current element node.</param>
    void XML_Validator::checkAttributeValue(XMLNode &xmlNode, const XMLNodeDTD::Attribute &attribute)
    {
        XMLNodeElement &xNodeElement = XMLNodeRef<XMLNodeElement>(xmlNode);
        bool attributePresent = xNodeElement.isAttributePresent(attribute.name);
        if ((attribute.type & XMLNodeDTD::AttributeType::required) != 0)
        {
            if (!attributePresent)
            {
                elementError(xNodeElement, "is missing required attribute '" + attribute.name + "'.");
            }
        }
        else if ((attribute.type & XMLNodeDTD::AttributeType::fixed) != 0)
        {
            if (attributePresent)
            {
                XMLAttribute elementAttribute = xNodeElement.getAttribute(attribute.name);
                if (attribute.value.parsed != elementAttribute.value.parsed)
                {
                    elementError(xNodeElement, "attribute '" + attribute.name + "' is '" +
                                                   elementAttribute.value.parsed + "' instead of '" +
                                                   attribute.value.parsed + "'.");
                }
            }
        }
        if (!attributePresent)
        {
            xNodeElement.addAttribute(attribute.name, {attribute.value.parsed, attribute.value.parsed});
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
    /// <param name="xmlNode">Current element node.</param>
    void XML_Validator::checkAttributeType(XMLNode &xmlNode, const XMLNodeDTD::Attribute &attribute)
    {
        XMLNodeElement &xNodeElement = XMLNodeRef<XMLNodeElement>(xmlNode);
        XMLAttribute elementAttribute = xNodeElement.getAttribute(attribute.name);
        if ((attribute.type & XMLNodeDTD::AttributeType::cdata) != 0)
        {
            if (elementAttribute.value.parsed.empty()) // No character data present.
            {
                elementError(xNodeElement, "attribute '" + attribute.name +
                                               "' does not contain character data.");
            }
        }
        else if ((attribute.type & XMLNodeDTD::AttributeType::id) != 0)
        {
            if (!checkIsIDOK(elementAttribute.value.parsed))
            {
                elementError(xNodeElement, "ID attribute '" + attribute.name + "' is invalid.");
            }
            if (m_assignedIDValues.find(elementAttribute.value.parsed) != m_assignedIDValues.end())
            {
                elementError(xNodeElement, "ID attribute '" + attribute.name + "' is not unique.");
            }
            m_assignedIDValues.insert(elementAttribute.value.parsed);
        }
        else if ((attribute.type & XMLNodeDTD::AttributeType::idref) != 0)
        {
            if (!checkIsIDOK(elementAttribute.value.parsed))
            {
                elementError(xNodeElement, "IDREF attribute '" + attribute.name + "' is invalid.");
            }
            m_assignedIDREFValues.insert(elementAttribute.value.parsed);
        }
        else if ((attribute.type & XMLNodeDTD::AttributeType::idrefs) != 0)
        {
            for (auto &id : Core::splitString(elementAttribute.value.parsed, ' '))
            {
                if (!checkIsIDOK(id))
                {
                    elementError(xNodeElement, "IDREFS attribute '" + attribute.name +
                                                   "' contains an invalid IDREF.");
                }
                m_assignedIDREFValues.insert(id);
            }
        }
        else if ((attribute.type & XMLNodeDTD::AttributeType::nmtoken) != 0)
        {
            if (!checkIsNMTOKENOK(elementAttribute.value.parsed))
            {
                elementError(xNodeElement, "NMTOKEN attribute '" + attribute.name + "' is invalid.");
            }
        }
        else if ((attribute.type & XMLNodeDTD::AttributeType::nmtokens) != 0)
        {
            for (auto &nmtoken :
                 Core::splitString(elementAttribute.value.parsed, ' '))
            {
                if (!checkIsNMTOKENOK(nmtoken))
                {
                    elementError(xNodeElement, "NMTOKEN attribute '" + attribute.name + "' contains an invalid NMTOKEN.");
                }
            }
        }
        else if ((attribute.type & XMLNodeDTD::AttributeType::entity) != 0)
        {
            if (!m_xmlNodeDTD.m_entityMapper.isPresent("&" + elementAttribute.value.parsed + ";"))
            {
                elementError(xNodeElement, "ENTITY attribute '" + attribute.name + "' value '" +
                                               elementAttribute.value.parsed + "' is not defined.");
            }
        }
        else if ((attribute.type & XMLNodeDTD::AttributeType::entities) != 0)
        {
            for (auto &entity : Core::splitString(elementAttribute.value.parsed, ' '))
            {
                if (!m_xmlNodeDTD.m_entityMapper.isPresent("&" + entity + ";"))
                {
                    elementError(xNodeElement, "ENTITIES attribute '" + attribute.name +
                                                   "' value '" + entity + "' is not defined.");
                }
            }
        }
        else if ((attribute.type & XMLNodeDTD::AttributeType::notation) != 0)
        {
            std::set<std::string> notations;
            for (auto &notation : Core::splitString(
                     attribute.enumeration.substr(1, attribute.enumeration.size() - 2), '|'))
            {
                notations.insert(notation);
            }
            if (notations.count(elementAttribute.value.parsed) == 0)
            {
                elementError(xNodeElement, "NOTATION attribute '" + attribute.name + "' value '" +
                                               elementAttribute.value.parsed + "' is not defined.");
            }
        }
        else if ((attribute.type & XMLNodeDTD::AttributeType::enumeration) != 0)
        {
            std::set<std::string> enumeration;
            for (auto &option : Core::splitString(
                     attribute.enumeration.substr(1, attribute.enumeration.size() - 2), '|'))
            {
                enumeration.insert(option);
            }
            if (enumeration.find(elementAttribute.value.parsed) == enumeration.end())
            {
                elementError(xNodeElement, "attribute '" + attribute.name +
                                               "' contains invalid enumeration value '" +
                                               elementAttribute.value.parsed + "'.");
            }
        }
    }
    /// <summary>
    /// Check element has the correct attribute type(s) and value(s) associated with
    /// it.
    /// </summary>
    /// <param name="xmlNode">Current element node.</param>
    void XML_Validator::checkAttributes(XMLNode &xmlNode)
    {
        XMLNodeElement &xNodeElement = XMLNodeRef<XMLNodeElement>(xmlNode);
        for (auto &attribute : m_xmlNodeDTD.getElement(xNodeElement.elementName).attributes)
        {
            if (xNodeElement.isAttributePresent(attribute.name))
            {
                checkAttributeType(xmlNode, attribute);
            }
            checkAttributeValue(xmlNode, attribute);
        }
    }
    /// <summary>
    /// Check elements structure.
    /// </summary>
    /// <param name="xmlNode">Current element node.</param>
    void XML_Validator::checkContentSpecification(XMLNode &xmlNode)
    {
        XMLNodeElement &xNodeElement = XMLNodeRef<XMLNodeElement>(xmlNode);
        if (m_xmlNodeDTD.getElementCount() == 0)
        {
            return;
        }
        if (m_xmlNodeDTD.getElement(xNodeElement.elementName).content.parsed == "((<#PCDATA>))")
        {
            if (!checkIsPCDATA(xmlNode))
            {
                elementError(xNodeElement, "does not contain just any parsable data.");
            }
            return;
        }
        if (m_xmlNodeDTD.getElement(xNodeElement.elementName).content.parsed == "EMPTY")
        {
            if (!checkIsEMPTY(xmlNode))
            {
                elementError(xNodeElement, "is not empty.");
            }
            return;
        }
        if (m_xmlNodeDTD.getElement(xNodeElement.elementName).content.parsed == "ANY")
        {
            return;
        }
        std::regex match{m_xmlNodeDTD.getElement(xNodeElement.elementName).content.parsed};
        std::string elements;
        for (auto &element : xNodeElement.children)
        {
            if ((element->getNodeType() == XMLNodeType::element) ||
                (element->getNodeType() == XMLNodeType::self))
            {
                elements += "<" + XMLNodeRef<XMLNodeElement>(*element).elementName + ">";
            }
            else if (element->getNodeType() == XMLNodeType::content)
            {
                if (!XMLNodeRef<XMLNodeContent>(*element).isWhiteSpace)
                {
                    elements += "<#PCDATA>";
                }
            }
        }
        if (!std::regex_match(elements, match))
        {
            elementError(xNodeElement, "does not conform to the content specification " +
                                           m_xmlNodeDTD.getElement(xNodeElement.elementName).content.unparsed + ".");
        }
    }
    /// <summary>
    /// Check elements content and associated attributes.
    /// </summary>
    /// <param name="xmlNode">Current element node.</param>
    void XML_Validator::checkElement(XMLNode &xmlNode)
    {
        checkContentSpecification(xmlNode);
        checkAttributes(xmlNode);
    }
    /// <summary>
    /// Recursively check elements of XML document.
    /// </summary>
    /// <param name="xmlNode">Current element node.</param>
    void XML_Validator::checkElements(XMLNode &xmlNode)
    {
        switch (xmlNode.getNodeType())
        {
        case XMLNodeType::prolog:
            for (auto &element : xmlNode.children)
            {
                checkElements(*element);
            }
            break;
        case XMLNodeType::declaration:
            // Nothing for present
            break;
        case XMLNodeType::root:
        case XMLNodeType::element:
            if (xmlNode.getNodeType() == XMLNodeType::root &&
                XMLNodeRef<XMLNodeElement>(xmlNode).elementName !=
                    m_xmlNodeDTD.getRootName())
            {
                throw ValidationError(
                    m_lineNumber, "DOCTYPE name does not match that of root element " +
                                      XMLNodeRef<XMLNodeElement>(xmlNode).elementName +
                                      " of DTD.");
            }
            checkElement(xmlNode);
            for (auto &element : xmlNode.children)
            {
                checkElements(*element);
            }
            break;
        case XMLNodeType::self:
            checkElement(xmlNode);
            break;
        case XMLNodeType::comment:
        case XMLNodeType::entity:
        case XMLNodeType::pi:
        case XMLNodeType::cdata:
        case XMLNodeType::dtd:
            break;
        case XMLNodeType::content:
            for (auto &ch : XMLNodeRef<XMLNodeContent>(xmlNode).content)
            {
                if (ch == kLineFeed)
                {
                    m_lineNumber++;
                }
            }
            break;
        default:
            throw ValidationError(m_lineNumber,
                                  "Invalid XMLNode encountered during validation.");
        }
    }
    /// <summary>
    /// Check XML element by element and then check all ID values reference an
    /// element.
    /// </summary>
    /// <param name=""></param>
    /// <param name="prolog">Prolog element containing root of XML to
    /// validate.</param>
    void XML_Validator::checkAgainstDTD(XMLNodeProlog &prolog)
    {
        m_lineNumber = m_xmlNodeDTD.getLineCount();
        checkElements(prolog);
        for (auto &idref : m_assignedIDREFValues)
        {
            if (m_assignedIDValues.find(idref) == m_assignedIDValues.end())
            {
                throw ValidationError(
                    m_lineNumber, "IDREF attribute '" + idref +
                                      "' does not reference any element with the ID.");
            }
        }
    }
    /// <summary>
    /// Validate XML against its DTD. Throwing an exception if there is a
    /// issue with the XML that is being validated.
    /// </summary>
    /// <param name="prolog">Prolog element containing root of XML to
    /// validate.</param>
    void XML_Validator::validate(XMLNodeProlog &prolog)
    {
        checkAgainstDTD(prolog);
    }
} // namespace XMLLib