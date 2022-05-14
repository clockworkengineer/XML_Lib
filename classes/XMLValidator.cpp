//
// Class: XMLValidator
//
// Description: XML DTD validator.
//
// Dependencies:   C20++ - Language standard features used.
//
// =================
// CLASS DEFINITIONS
// =================
#include "XML.hpp"
#include "XMLValidator.hpp"
#include "XML_errors.hpp"
#include "XML_sources.hpp"
#include "XML_destinations.hpp"
// ====================
// CLASS IMPLEMENTATION
// ====================
//
// C++ STL
//
#include <regex>
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
    /// Check whether a token value is valid.
    /// </summary>
    /// <param name="nmTokenValue">Token value</param>
    /// <returns>true then token is valid otherwise false.</returns>
    bool XMLValidator::checkIsNMTOKENOK(std::string nmTokenValue)
    {
        trimmString(nmTokenValue);
        BufferSource nmTokenValueSource(nmTokenValue);
        while (nmTokenValueSource.more())
        {
            if (!validNameChar(nmTokenValueSource.current()))
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
    bool XMLValidator::checkIsIDOK(const std::string &idValue)
    {
        try
        {
            BufferSource idSource(idValue);
            parseName(idSource);
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
    bool XMLValidator::checkIsPCDATA(XMLNode *xmlNode)
    {
        for (auto &element : static_cast<XMLNodeElement *>(xmlNode)->children)
        {
            if ((XMLNodeRef<XMLNode>(*element).getNodeType() == XMLNodeType::element) ||
                (XMLNodeRef<XMLNode>(*element).getNodeType() == XMLNodeType::self))
            {
                return (false);
            }
        }
        return (!static_cast<XMLNodeElement *>(xmlNode)->getContents().empty());
    }
    /// <summary>
    /// Check whether an element does not contain any content (is empty).
    /// </summary>
    /// <param name="xmlNode">Current element node.</param>
    /// <returns>true if element empty otherwise false.</returns>
    bool XMLValidator::checkIsEMPTY(XMLNode *xmlNode)
    {
        return (static_cast<XMLNodeElement *>(xmlNode)->children.empty() || static_cast<XMLNodeElement *>(xmlNode)->getNodeType() == XMLNodeType::self);
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
    void XMLValidator::checkAttributeValue(XMLNode *xmlNode, DTDAttribute &attribute)
    {
        if ((attribute.type & DTDAttributeType::required) != 0)
        {
            if (!static_cast<XMLNodeElement *>(xmlNode)->isAttributePresent(attribute.name))
            {
                throw ValidationError(m_lineNumber, "Required attribute '" + attribute.name + "' missing for element <" + static_cast<XMLNodeElement *>(xmlNode)->elementName + ">.");
            }
        }
        else if ((attribute.type & DTDAttributeType::implied) != 0)
        {
            return;
        }
        else if ((attribute.type & DTDAttributeType::fixed) != 0)
        {
            if (static_cast<XMLNodeElement *>(xmlNode)->isAttributePresent(attribute.name))
            {
                XMLAttribute elementAttribute = static_cast<XMLNodeElement *>(xmlNode)->getAttribute(attribute.name);
                if (attribute.value.parsed != elementAttribute.value.parsed)
                {
                    throw ValidationError(m_lineNumber, "Element <" + static_cast<XMLNodeElement *>(xmlNode)->elementName + "> attribute '" + attribute.name + "' is '" + elementAttribute.value.parsed + "' instead of '" + attribute.value.parsed + "'.");
                }
            }
            else
            {
                XMLValue value;
                value.parsed = value.unparsed = attribute.value.parsed;
                static_cast<XMLNodeElement *>(xmlNode)->addAttribute(attribute.name, value);
            }
        }
        else
        {
            if (!static_cast<XMLNodeElement *>(xmlNode)->isAttributePresent(attribute.name))
            {
                XMLValue value;
                value.parsed = value.unparsed = attribute.value.parsed;
                static_cast<XMLNodeElement *>(xmlNode)->addAttribute(attribute.name, value);
            }
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
    void XMLValidator::checkAttributeType(XMLNode *xmlNode, DTDAttribute &attribute)
    {
        XMLAttribute elementAttribute = static_cast<XMLNodeElement *>(xmlNode)->getAttribute(attribute.name);
        if ((attribute.type & DTDAttributeType::cdata) != 0)
        {
            if (elementAttribute.value.parsed.empty()) // No character data present.
            {
                ValidationError(m_lineNumber, "Element <" + static_cast<XMLNodeElement *>(xmlNode)->elementName + "> attribute '" + attribute.name + "' does not contain character data.");
            }
        }
        else if ((attribute.type & DTDAttributeType::id) != 0)
        {
            if (!checkIsIDOK(elementAttribute.value.parsed))
            {
                throw ValidationError(m_lineNumber, "Element <" + static_cast<XMLNodeElement *>(xmlNode)->elementName + "> ID attribute '" + attribute.name + "' is invalid.");
            }
            if (m_assignedIDValues.find(elementAttribute.value.parsed) != m_assignedIDValues.end())
            {
                throw ValidationError(m_lineNumber, "Element <" + static_cast<XMLNodeElement *>(xmlNode)->elementName + "> ID attribute '" + attribute.name + "' is not unique.");
            }
            m_assignedIDValues.insert(elementAttribute.value.parsed);
        }
        else if ((attribute.type & DTDAttributeType::idref) != 0)
        {
            if (!checkIsIDOK(elementAttribute.value.parsed))
            {
                throw ValidationError(m_lineNumber, "Element <" + static_cast<XMLNodeElement *>(xmlNode)->elementName + "> IDREF attribute '" + attribute.name + "' is invalid.");
            }
            m_assignedIDREFValues.insert(elementAttribute.value.parsed);
        }
        else if ((attribute.type & DTDAttributeType::idrefs) != 0)
        {
            for (auto &id : splitString(elementAttribute.value.parsed, ' '))
            {
                if (!checkIsIDOK(id))
                {
                    throw ValidationError(m_lineNumber, "Element <" + static_cast<XMLNodeElement *>(xmlNode)->elementName + "> IDREFS attribute '" + attribute.name + "' contains an invalid IDREF.");
                }
                m_assignedIDREFValues.insert(id);
            }
        }
        else if ((attribute.type & DTDAttributeType::nmtoken) != 0)
        {
            if (!checkIsNMTOKENOK(elementAttribute.value.parsed))
            {
                throw ValidationError(m_lineNumber, "Element <" + static_cast<XMLNodeElement *>(xmlNode)->elementName + "> NMTOKEN attribute '" + attribute.name + "' is invalid.");
            }
        }
        else if ((attribute.type & DTDAttributeType::nmtokens) != 0)
        {
            for (auto &nmtoken : splitString(elementAttribute.value.parsed, ' '))
            {
                if (!checkIsNMTOKENOK(nmtoken))
                {
                    throw ValidationError(m_lineNumber, "Element <" + static_cast<XMLNodeElement *>(xmlNode)->elementName + "> NMTOKEN attribute '" + attribute.name + "' contains an invald NMTOKEN.");
                }
            }
        }
        else if ((attribute.type & DTDAttributeType::entity) != 0)
        {
            if (!m_dtd.isEntityPresent("&" + elementAttribute.value.parsed + ";"))
            {
                throw ValidationError(m_lineNumber, "Element <" + static_cast<XMLNodeElement *>(xmlNode)->elementName + "> ENTITY attribute '" + attribute.name + "' value '" + elementAttribute.value.parsed + "' is not defined.");
            }
        }
        else if ((attribute.type & DTDAttributeType::entities) != 0)
        {
            for (auto &entity : splitString(elementAttribute.value.parsed, ' '))
            {
                if (!m_dtd.isEntityPresent("&" + entity + ";"))
                {
                    throw ValidationError(m_lineNumber, "Element <" + static_cast<XMLNodeElement *>(xmlNode)->elementName + "> ENTITIES attribute '" + attribute.name + "' value '" + entity + "' is not defined.");
                }
            }
        }
        else if ((attribute.type & DTDAttributeType::notation) != 0)
        {
            std::set<std::string> notations;
            for (auto &notation : splitString(attribute.enumeration.substr(1, attribute.enumeration.size() - 2), '|'))
            {
                notations.insert(notation);
            }
            if (notations.count(elementAttribute.value.parsed) == 0)
            {
                throw ValidationError(m_lineNumber, "Element <" + static_cast<XMLNodeElement *>(xmlNode)->elementName + "> NOTATION attribute '" + attribute.name + "' value '" + elementAttribute.value.parsed + "' is not defined.");
            }
        }
        else if ((attribute.type & DTDAttributeType::enumeration) != 0)
        {
            std::set<std::string> enumeration;
            for (auto &option : splitString(attribute.enumeration.substr(1, attribute.enumeration.size() - 2), '|'))
            {
                enumeration.insert(option);
            }
            if (enumeration.find(elementAttribute.value.parsed) == enumeration.end())
            {
                throw ValidationError(m_lineNumber, "Element <" + static_cast<XMLNodeElement *>(xmlNode)->elementName + "> attribute '" + attribute.name + "' contains invalid enumeration value '" + elementAttribute.value.parsed + "'.");
            }
        }
    }
    /// <summary>
    /// Check element has the correct attribute type(s) and value(s) associated with it.
    /// </summary>
    /// <param name="xmlNode">Current element node.</param>
    void XMLValidator::checkAttributes(XMLNode *xmlNode)
    {
        for (auto &attribute : m_dtd.getElement(static_cast<XMLNodeElement *>(xmlNode)->elementName).attributes)
        {
            if (static_cast<XMLNodeElement *>(xmlNode)->isAttributePresent(attribute.name))
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
    void XMLValidator::checkContentSpecification(XMLNode *xmlNode)
    {
        if (m_dtd.getElementCount() == 0)
        {
            return;
        }
        if (m_dtd.getElement(static_cast<XMLNodeElement *>(xmlNode)->elementName).content.parsed == "((<#PCDATA>))")
        {
            if (!checkIsPCDATA(xmlNode))
            {
                throw ValidationError(m_lineNumber, "Element <" + static_cast<XMLNodeElement *>(xmlNode)->elementName + "> does not contain just any parsable data.");
            }
            return;
        }
        if (m_dtd.getElement(static_cast<XMLNodeElement *>(xmlNode)->elementName).content.parsed == "EMPTY")
        {
            if (!checkIsEMPTY(xmlNode))
            {
                throw ValidationError(m_lineNumber, "Element <" + static_cast<XMLNodeElement *>(xmlNode)->elementName + "> is not empty.");
            }
            return;
        }
        if (m_dtd.getElement(static_cast<XMLNodeElement *>(xmlNode)->elementName).content.parsed == "ANY")
        {
            return;
        }
        std::regex match(m_dtd.getElement(static_cast<XMLNodeElement *>(xmlNode)->elementName).content.parsed);
        std::string elements;
        for (auto &element : static_cast<XMLNodeElement *>(xmlNode)->children)
        {
            if ((XMLNodeRef<XMLNode>(*element).getNodeType() == XMLNodeType::element) ||
                (XMLNodeRef<XMLNode>(*element).getNodeType() == XMLNodeType::self))
            {
                elements += "<" + XMLNodeRef<XMLNodeElement>(*element).elementName + ">";
            }
            else if (XMLNodeRef<XMLNode>(*element).getNodeType() == XMLNodeType::content)
            {
                if (!XMLNodeRef<XMLNodeContent>(*element).isWhiteSpace)
                {
                    elements += "<#PCDATA>";
                }
            }
        }
        if (!std::regex_match(elements, match))
        {
            throw ValidationError(m_lineNumber, "<" + static_cast<XMLNodeElement *>(xmlNode)->elementName + "> element does not conform to the content specification " +
                                                    m_dtd.getElement(static_cast<XMLNodeElement *>(xmlNode)->elementName).content.unparsed + ".");
        }
    }
    /// <summary>
    /// Check elements content and associated attributes.
    /// </summary>
    /// <param name="xmlNode">Current element node.</param>
    void XMLValidator::checkElement(XMLNode *xmlNode)
    {
        checkContentSpecification(xmlNode);
        checkAttributes(xmlNode);
    }
    /// <summary>
    /// Recursively check elements of XML document.
    /// </summary>
    /// <param name="xmlNode">Current element node.</param>
    void XMLValidator::checkElements(XMLNode *xmlNode)
    {
        switch (xmlNode->getNodeType())
        {
        case XMLNodeType::prolog:
            for (auto &element : XMLNodeRef<XMLNodeElement>((*xmlNode)).children)
            {
                checkElements(element.get());
            }
            break;
        case XMLNodeType::root:
        case XMLNodeType::element:
            if (xmlNode->getNodeType() == XMLNodeType::root && XMLNodeRef<XMLNodeElement>((*xmlNode)).elementName != m_dtd.getRootName())
            {
                throw ValidationError(m_lineNumber, "DOCTYPE name does not match that of root element " + XMLNodeRef<XMLNodeElement>((*xmlNode)).elementName + " of DTD.");
            }
            checkElement(static_cast<XMLNodeElement *>(xmlNode));
            for (auto &element : XMLNodeRef<XMLNodeElement>((*xmlNode)).children)
            {
                checkElements(element.get());
            }
            break;
        case XMLNodeType::self:
            checkElement(static_cast<XMLNodeElement *>(xmlNode));
            break;
        case XMLNodeType::comment:
        case XMLNodeType::entity:
        case XMLNodeType::pi:
        case XMLNodeType::cdata:
        case XMLNodeType::dtd:
            break;
        case XMLNodeType::content:
            for (auto &ch : XMLNodeRef<XMLNodeContent>((*xmlNode)).content)
            {
                if (ch == kLineFeed)
                {
                    m_lineNumber++;
                }
            }
            break;
        default:
            throw ValidationError(m_lineNumber, "Invalid XMLNode encountered during validation.");
        }
    }
    /// <summary>
    /// Check XML element by element and then check all ID values reference an element.
    /// </summary>
    /// <param name=""></param>
    /// <param name="prolog">Prolog element containing root of XML to validate.</param>
    void XMLValidator::checkAgainstDTD(XMLNodeElement &prolog)
    {
        m_lineNumber = m_dtd.getLineCount();
        checkElements(&prolog);
        for (auto &idref : m_assignedIDREFValues)
        {
            if (m_assignedIDValues.find(idref) == m_assignedIDValues.end())
            {
                throw ValidationError(m_lineNumber, "IDREF attribute '" + idref + "' does not reference any element with the ID.");
            }
        }
    }
    /// <summary>
    /// Validate XML against its DTD. Throwing an exception if there is a
    /// issue with the XML that is being validated.
    /// </summary>
    /// <param name="prolog">Prolog element containing root of XML to validate.</param>
    void XMLValidator::validate(XMLNodeElement &prolog)
    {
        checkAgainstDTD(prolog);
    }
} // namespace XMLLib
