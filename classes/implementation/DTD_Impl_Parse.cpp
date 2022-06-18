//
// Class: DTD_impl
//
// Description: Parse XML DTD.
//
// Dependencies:   C20++ - Language standard features used.
//
// =================
// CLASS DEFINITIONS
// =================
#include "DTD.hpp"
#include "DTD_Impl.hpp"
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
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void DTD_Impl::parseValidNotations(const std::string &notations)
    {
        for (auto &notation : splitString(notations.substr(1, notations.size() - 2), '|'))
        {
            if (m_xmlNodeDTD.getNotationCount(notation) == 0)
            {
                throw SyntaxError("NOTATION " + notation + " is not defined.");
            }
        }
    }
    /// <summary>
    /// Validate attribute description.
    /// </summary>
    /// <param name="elementName">Element associated with attribute.</param>
    /// <param name="dtdAttribute">Attribute description to validate.</param>
    void DTD_Impl::parseValidateAttribute(const std::string &elementName, const XMLNodeDTD::Attribute &dtdAttribute)
    {
        // Attribute cannot be ID and fixed
        if (dtdAttribute.type == (XMLNodeDTD::AttributeType::id | XMLNodeDTD::AttributeType::fixed))
        {
            throw SyntaxError("Attribute '" + dtdAttribute.name + "' may not be of type ID and FIXED.");
        }
        // Only one ID attribute allowed per element
        else if ((dtdAttribute.type & XMLNodeDTD::AttributeType::id) != 0)
        {
            if (m_xmlNodeDTD.getElement(elementName).idAttributePresent)
            {
                throw SyntaxError("Element <" + elementName + "> has more than one ID attribute.");
            }
            m_xmlNodeDTD.getElement(elementName).idAttributePresent = true;
        }
        // Enumeration contains unique values and default is valid value
        else if (dtdAttribute.type == (XMLNodeDTD::AttributeType::enumeration | XMLNodeDTD::AttributeType::normal))
        {
            std::set<std::string> options;
            for (auto &option : splitString(dtdAttribute.enumeration.substr(1, dtdAttribute.enumeration.size() - 2), '|'))
            {
                if (options.find(option) == options.end())
                {
                    options.insert(option);
                }
                else
                {
                    throw SyntaxError("Enumerator value '" + option + "' for attribute '" + dtdAttribute.name + "' occurs more than once in its definition.");
                }
            }
            if (options.find(dtdAttribute.value.parsed) == options.end())
            {
                throw SyntaxError("Default value '" + dtdAttribute.value.parsed + "' for enumeration attribute '" + dtdAttribute.name + "' is invalid.");
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
        std::string enumerationType(source.current_to_bytes());
        source.next();
        source.ignoreWS();
        enumerationType += parseName(source);
        while (source.more() && source.current() == '|')
        {
            enumerationType += source.current_to_bytes();
            source.next();
            source.ignoreWS();
            enumerationType += parseName(source);
        }
        if (source.current() != ')')
        {
            throw SyntaxError(source, "Missing closing ')' on enumeration attribute type.");
        }
        enumerationType += source.current_to_bytes();
        source.next();
        source.ignoreWS();
        return (enumerationType);
    }
    /// <summary>
    /// Parse DTD attribute type field.
    /// </summary>
    /// <param name="source">DTD source stream.</param>
    /// <param name="attribute">Attribute description.</param>
    /// <returns>Attribute type as string (UTF-8 encoded).</returns>
    void DTD_Impl::parseAttributeType(ISource &source, XMLNodeDTD::Attribute &attribute)
    {
        if (source.match(U"CDATA"))
        {
            attribute.type = XMLNodeDTD::AttributeType::cdata;
            source.ignoreWS();
            return;
        }
        else if (source.match(U"IDREFS"))
        {
            attribute.type = XMLNodeDTD::AttributeType::idrefs;
            source.ignoreWS();
            return;
        }
        else if (source.match(U"IDREF"))
        {
            attribute.type = XMLNodeDTD::AttributeType::idref;
            source.ignoreWS();
            return;
        }
        else if (source.match(U"ID"))
        {
            attribute.type = XMLNodeDTD::AttributeType::id;
            source.ignoreWS();
            return;
        }
        else if (source.match(U"NMTOKENS"))
        {
            attribute.type = XMLNodeDTD::AttributeType::nmtokens;
            source.ignoreWS();
            return;
        }
        else if (source.match(U"NMTOKEN"))
        {
            attribute.type = XMLNodeDTD::AttributeType::nmtoken;
            source.ignoreWS();
            return;
        }
        else if (source.match(U"ENTITY"))
        {
            attribute.type = XMLNodeDTD::AttributeType::entity;
            source.ignoreWS();
            return;
        }
        else if (source.match(U"ENTITIES"))
        {
            attribute.type = XMLNodeDTD::AttributeType::entities;
            source.ignoreWS();
            return;
        }
        else if (source.match(U"NOTATION"))
        {
            attribute.type = XMLNodeDTD::AttributeType::notation;
            source.ignoreWS();
        }
        if (source.current() == '(')
        {
            attribute.enumeration = parseAttributeEnumerationType(source);
            if (attribute.type == XMLNodeDTD::AttributeType::notation)
            {
                parseValidNotations(attribute.enumeration);
            }
            else
            {
                attribute.type = XMLNodeDTD::AttributeType::enumeration;
            }
            return;
        }
        throw SyntaxError(source, "Invalid attribute type specified.");
    }
    /// <summary>
    /// Parse DTD attribute value.
    /// </summary>
    /// <param name="source">DTD source stream.</param>
    /// <param name="attribute">Attribute description.</param>
    void DTD_Impl::parseAttributeValue(ISource &source, XMLNodeDTD::Attribute &attribute)
    {
        if (source.match(U"#REQUIRED"))
        {
            attribute.type |= XMLNodeDTD::AttributeType::required;
        }
        else if (source.match(U"#IMPLIED"))
        {
            attribute.type |= XMLNodeDTD::AttributeType::implied;
        }
        else if (source.match(U"#FIXED"))
        {
            source.ignoreWS();
            attribute.value = parseValue(source, m_xmlNodeDTD.m_entityMapper);
            attribute.type |= XMLNodeDTD::AttributeType::fixed;
        }
        else
        {
            source.ignoreWS();
            attribute.value = parseValue(source, m_xmlNodeDTD.m_entityMapper);
            attribute.type |= XMLNodeDTD::AttributeType::normal;
        }
    }
    /// <summary>
    /// Parse DTD attribute list.
    /// </summary>
    /// <param name="source">DTD source stream.</param>
    void DTD_Impl::parseAttributeList(ISource &source)
    {
        source.ignoreWS();
        std::string elementName = parseName(source);
        while (source.more() && validNameStartChar(source.current()))
        {
            XMLNodeDTD::Attribute dtdAttribute;
            dtdAttribute.name = parseName(source);
            parseAttributeType(source, dtdAttribute);
            parseAttributeValue(source, dtdAttribute);
            parseValidateAttribute(elementName, dtdAttribute);
            m_xmlNodeDTD.getElement(elementName).attributes.emplace_back(dtdAttribute);
            source.ignoreWS();
        }
    }
    /// <summary>
    /// Parse DTD notation.
    /// </summary>
    /// <param name="source">DTD source stream.</param>
    void DTD_Impl::parseNotation(ISource &source)
    {
        source.ignoreWS();
        std::string name = parseName(source);
        m_xmlNodeDTD.addNotation(name, parseExternalReference(source));
        source.ignoreWS();
    }
    /// <summary>
    /// Parse DTD entity.
    /// </summary>
    /// <param name="source">DTD source stream.</param>
    void DTD_Impl::parseEntity(ISource &source)
    {
        std::string entityName = "&";
        source.ignoreWS();
        if (source.current() == '%')
        {
            entityName = "%";
            source.next();
            source.ignoreWS();
        }
        entityName += parseName(source) + ";";
        if (source.current() == '\'' || source.current() == '"')
        {
            XMLValue entityValue = parseValue(source);
            m_xmlNodeDTD.m_entityMapper.get(entityName).internal = entityValue.parsed;
        }
        else
        {
            m_xmlNodeDTD.m_entityMapper.get(entityName).external = parseExternalReference(source);
            if (source.match(U"NDATA"))
            {
                source.ignoreWS();
                m_xmlNodeDTD.m_entityMapper.get(entityName).notation = parseName(source);
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
        std::string elementName = parseName(source);
        if (source.match(U"EMPTY"))
        {
            m_xmlNodeDTD.addElement(elementName, XMLNodeDTD::Element(elementName, XMLValue{"EMPTY", "EMPTY"}));
        }
        else if (source.match(U"ANY"))
        {
            m_xmlNodeDTD.addElement(elementName, XMLNodeDTD::Element(elementName, XMLValue{"ANY", "ANY"}));
        }
        else
        {
            std::string unparsed;
            while (source.more() &&
                   (source.current() != '<') &&
                   (source.current() != '>'))
            {
                unparsed += source.current_to_bytes();
                source.next();
            }
            m_xmlNodeDTD.addElement(elementName, XMLNodeDTD::Element(elementName, parseElementContentSpecification(elementName, XMLValue{unparsed, ""})));
        }
        source.ignoreWS();
    }
    /// <summary>
    /// Parse DTD comment.
    /// </summary>
    /// <param name="source">DTD source stream.</param>
    void DTD_Impl::parseComment(ISource &source)
    {
        while (source.more() && !source.match(U"--"))
        {
            source.next();
        }
    }
    /// <summary>
    /// Parse DTD parameter entity reference.
    /// </summary>
    /// <param name="source">DTD source stream.</param>
    void DTD_Impl::parseParameterEntityReference(ISource &source)
    {
        XMLValue parameterEntity = parseEntityReference(source);
        BufferSource entitySource(m_xmlNodeDTD.m_entityMapper.translate(parameterEntity.unparsed));
        parseInternal(entitySource);
        source.ignoreWS();
    }
    /// <summary>
    /// Parse internally defined DTD.
    /// </summary>
    /// <param name="source">DTD source stream.</param>
    void DTD_Impl::parseInternal(ISource &source)
    {
        while (source.more() && !source.match(U"]>"))
        {
            if (source.match(U"<!ENTITY"))
            {
                parseEntity(source);
            }
            else if (source.match(U"<!ELEMENT"))
            {
                parseElement(source);
            }
            else if (source.match(U"<!ATTLIST"))
            {
                parseAttributeList(source);
            }
            else if (source.match(U"<!NOTATION"))
            {
                parseNotation(source);
            }
            else if (source.match(U"<!--"))
            {
                parseComment(source);
            }
            else if (source.current() == '%')
            {
                parseParameterEntityReference(source);
                continue;
            }
            else
            {
                throw SyntaxError(source, "Invalid DTD tag.");
            }
            if (source.current() != '>')
            {
                throw SyntaxError(source, "Missing '>' terminator.");
            }
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
        long start = source.position();
        source.ignoreWS();
        m_xmlNodeDTD.setRootName(parseName(source));
        // Parse in external DTD reference
        if (source.current() != '[')
        {
            m_xmlNodeDTD.setExternalReference(parseExternalReference(source));
        }
        // We have internal DTD so parse that first
        if (source.current() == '[')
        {
            source.next();
            source.ignoreWS();
            parseInternal(source);
            m_xmlNodeDTD.setType(XMLNodeDTD::Type::internal);
        }
        // Missing '>' after external DTD reference
        else if (source.current() != '>')
        {
            throw SyntaxError(source, "Missing '>' terminator.");
        }
        // Move to the next component in XML prolog
        else
        {
            source.next();
            source.ignoreWS();
        }
        // Parse any DTD in external reference found
        if (!m_xmlNodeDTD.getExternalReference().type.empty())
        {
            parseExternal(source);
            m_xmlNodeDTD.setType(m_xmlNodeDTD.getType() | XMLNodeDTD::Type::external);
        }
        // Save away unparsed form of DTD
        m_xmlNodeDTD.setUnparsed(std::string("<!DOCTYPE") + source.getRange(start, source.position()));
        // Make sure no defined entity contains recursion
        for (auto &entityName : m_xmlNodeDTD.m_entityMapper.getList())
        {
            m_xmlNodeDTD.m_entityMapper.recursive(entityName.first, entityName.first[0]);
        }
        // Count lines in DTD
        std::string unparsedDTD = m_xmlNodeDTD.unparsed();
        m_xmlNodeDTD.setLineCount(static_cast<long>(std::count(unparsedDTD.begin(), unparsedDTD.end(), kLineFeed)) + 1);
    }
} // namespace XMLLib
