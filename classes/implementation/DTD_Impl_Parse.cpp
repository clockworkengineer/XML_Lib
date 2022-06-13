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
        for (auto &notation : Core::splitString(notations.substr(1, notations.size() - 2), '|'))
        {
            if (m_xmlNodeDTD.m_parsed->getNotationCount(notation) == 0)
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
    void DTD_Impl::parseValidateAttribute(const std::string &elementName, const DTDAttribute &dtdAttribute)
    {
        // Attribute cannot be ID and fixed
        if (dtdAttribute.type == (DTDAttributeType::id | DTDAttributeType::fixed))
        {
            throw SyntaxError("Attribute '" + dtdAttribute.name + "' may not be of type ID and FIXED.");
        }
        // Only one ID attribute allowed per element
        else if ((dtdAttribute.type & DTDAttributeType::id) != 0)
        {
            if (m_xmlNodeDTD.m_parsed->getElement(elementName).idAttributePresent)
            {
                throw SyntaxError("Element <" + elementName + "> has more than one ID attribute.");
            }
            m_xmlNodeDTD.m_parsed->getElement(elementName).idAttributePresent = true;
        }
        // Enumeration contains unique values and default is valid value
        else if (dtdAttribute.type == (DTDAttributeType::enumeration | DTDAttributeType::normal))
        {
            std::set<std::string> options;
            for (auto &option : Core::splitString(dtdAttribute.enumeration.substr(1, dtdAttribute.enumeration.size() - 2), '|'))
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
    /// <param name="dtdSource">DTD source stream.</param>
    /// <returns>Enumeration string.</returns>
    std::string DTD_Impl::parseAttributeEnumerationType(ISource &dtdSource)
    {
        std::string enumerationType(dtdSource.current_to_bytes());
        dtdSource.next();
        dtdSource.ignoreWS();
        enumerationType += Core::parseName(dtdSource);
        while (dtdSource.more() && dtdSource.current() == '|')
        {
            enumerationType += dtdSource.current_to_bytes();
            dtdSource.next();
            dtdSource.ignoreWS();
            enumerationType += Core::parseName(dtdSource);
        }
        if (dtdSource.current() != ')')
        {
            throw SyntaxError(dtdSource, "Missing closing ')' on enumeration attribute type.");
        }
        enumerationType += dtdSource.current_to_bytes();
        dtdSource.next();
        dtdSource.ignoreWS();
        return (enumerationType);
    }
    /// <summary>
    /// Parse DTD attribute type field.
    /// </summary>
    /// <param name="dtdSource">DTD source stream.</param>
    /// <param name="dtdattribute">Attribute description.</param>
    /// <returns>Attribute type as string (UTF-8 encoded).</returns>
    void DTD_Impl::parseAttributeType(ISource &dtdSource, DTDAttribute &attribute)
    {
        if (dtdSource.match(U"CDATA"))
        {
            attribute.type = DTDAttributeType::cdata;
            dtdSource.ignoreWS();
            return;
        }
        else if (dtdSource.match(U"IDREFS"))
        {
            attribute.type = DTDAttributeType::idrefs;
            dtdSource.ignoreWS();
            return;
        }
        else if (dtdSource.match(U"IDREF"))
        {
            attribute.type = DTDAttributeType::idref;
            dtdSource.ignoreWS();
            return;
        }
        else if (dtdSource.match(U"ID"))
        {
            attribute.type = DTDAttributeType::id;
            dtdSource.ignoreWS();
            return;
        }
        else if (dtdSource.match(U"NMTOKENS"))
        {
            attribute.type = DTDAttributeType::nmtokens;
            dtdSource.ignoreWS();
            return;
        }
        else if (dtdSource.match(U"NMTOKEN"))
        {
            attribute.type = DTDAttributeType::nmtoken;
            dtdSource.ignoreWS();
            return;
        }
        else if (dtdSource.match(U"ENTITY"))
        {
            attribute.type = DTDAttributeType::entity;
            dtdSource.ignoreWS();
            return;
        }
        else if (dtdSource.match(U"ENTITIES"))
        {
            attribute.type = DTDAttributeType::entities;
            dtdSource.ignoreWS();
            return;
        }
        else if (dtdSource.match(U"NOTATION"))
        {
            attribute.type = DTDAttributeType::notation;
            dtdSource.ignoreWS();
        }
        if (dtdSource.current() == '(')
        {
            attribute.enumeration = parseAttributeEnumerationType(dtdSource);
            if (attribute.type == DTDAttributeType::notation)
            {
                parseValidNotations(attribute.enumeration);
            }
            else
            {
                attribute.type = DTDAttributeType::enumeration;
            }
            return;
        }
        throw SyntaxError(dtdSource, "Invalid attribute type specified.");
    }
    /// <summary>
    /// Parse DTD attribute value.
    /// </summary>
    /// <param name="dtdSource">DTD source stream.</param>
    /// <param name="attribute">Attribute description.</param>
    void DTD_Impl::parseAttributeValue(ISource &dtdSource, DTDAttribute &attribute)
    {
        if (dtdSource.match(U"#REQUIRED"))
        {
            attribute.type |= DTDAttributeType::required;
        }
        else if (dtdSource.match(U"#IMPLIED"))
        {
            attribute.type |= DTDAttributeType::implied;
        }
        else if (dtdSource.match(U"#FIXED"))
        {
            dtdSource.ignoreWS();
            attribute.value = Core::parseValue(dtdSource, m_xmlNodeDTD.m_parsed->m_entityMapper);
            attribute.type |= DTDAttributeType::fixed;
        }
        else
        {
            dtdSource.ignoreWS();
            attribute.value = Core::parseValue(dtdSource, m_xmlNodeDTD.m_parsed->m_entityMapper);
            attribute.type |= DTDAttributeType::normal;
        }
    }
    /// <summary>
    /// Parse DTD attribute list.
    /// </summary>
    /// <param name="dtdSource">DTD source stream.</param>
    void DTD_Impl::parseAttributeList(ISource &dtdSource)
    {
        dtdSource.ignoreWS();
        std::string elementName = Core::parseName(dtdSource);
        while (dtdSource.more() && Core::validNameStartChar(dtdSource.current()))
        {
            DTDAttribute dtdAttribute;
            dtdAttribute.name = Core::parseName(dtdSource);
            parseAttributeType(dtdSource, dtdAttribute);
            parseAttributeValue(dtdSource, dtdAttribute);
            parseValidateAttribute(elementName, dtdAttribute);
            m_xmlNodeDTD.m_parsed->getElement(elementName).attributes.emplace_back(dtdAttribute);
            dtdSource.ignoreWS();
        }
    }
    /// <summary>
    /// Parse DTD notation.
    /// </summary>
    /// <param name="dtdSource">DTD source stream.</param>
    void DTD_Impl::parseNotation(ISource &dtdSource)
    {
        dtdSource.ignoreWS();
        std::string name = Core::parseName(dtdSource);
        m_xmlNodeDTD.m_parsed->addNotation(name, parseExternalReference(dtdSource));
        dtdSource.ignoreWS();
    }
    /// <summary>
    /// Parse DTD entity.
    /// </summary>
    /// <param name="dtdSource">DTD source stream.</param>
    void DTD_Impl::parseEntity(ISource &dtdSource)
    {
        std::string entityName = "&";
        dtdSource.ignoreWS();
        if (dtdSource.current() == '%')
        {
            entityName = "%";
            dtdSource.next();
            dtdSource.ignoreWS();
        }
        entityName += Core::parseName(dtdSource) + ";";
        if (dtdSource.current() == '\'' || dtdSource.current() == '"')
        {
            XMLValue entityValue = Core::parseValue(dtdSource);
            m_xmlNodeDTD.m_parsed->m_entityMapper.get(entityName).internal = entityValue.parsed;
        }
        else
        {
            m_xmlNodeDTD.m_parsed->m_entityMapper.get(entityName).external = parseExternalReference(dtdSource);
            if (dtdSource.match(U"NDATA"))
            {
                dtdSource.ignoreWS();
                m_xmlNodeDTD.m_parsed->m_entityMapper.get(entityName).notation = Core::parseName(dtdSource);
            }
        }
    }
    /// <summary>
    /// Parse an DTD element.
    /// </summary>
    /// <param name="dtdSource">DTD source stream.</param>
    void DTD_Impl::parseElement(ISource &dtdSource)
    {
        dtdSource.ignoreWS();
        std::string elementName = Core::parseName(dtdSource);
        if (dtdSource.match(U"EMPTY"))
        {
            m_xmlNodeDTD.m_parsed->addElement(elementName, DTDElement(elementName, XMLValue{"EMPTY", "EMPTY"}));
        }
        else if (dtdSource.match(U"ANY"))
        {
            m_xmlNodeDTD.m_parsed->addElement(elementName, DTDElement(elementName, XMLValue{"ANY", "ANY"}));
        }
        else
        {
            std::string unparsed;
            while (dtdSource.more() &&
                   (dtdSource.current() != '<') &&
                   (dtdSource.current() != '>'))
            {
                unparsed += dtdSource.current_to_bytes();
                dtdSource.next();
            }
            m_xmlNodeDTD.m_parsed->addElement(elementName, DTDElement(elementName, parseElementContentSpecification(elementName, XMLValue{unparsed, ""})));
        }
        dtdSource.ignoreWS();
    }
    /// <summary>
    /// Parse DTD comment.
    /// </summary>
    /// <param name="dtdSource">DTD source stream.</param>
    void DTD_Impl::parseComment(ISource &dtdSource)
    {
        while (dtdSource.more() && !dtdSource.match(U"--"))
        {
            dtdSource.next();
        }
    }
    /// <summary>
    /// Parse DTD parameter entity reference.
    /// </summary>
    /// <param name="dtdSource">DTD source stream.</param>
    void DTD_Impl::parseParameterEntityReference(ISource &dtdSource)
    {
        XMLValue parameterEntity = Core::parseEntityReference(dtdSource);
        BufferSource entitySource(m_xmlNodeDTD.m_parsed->m_entityMapper.translate(parameterEntity.unparsed));
        parseInternal(entitySource);
        dtdSource.ignoreWS();
    }
    /// <summary>
    /// Parse internally defined DTD.
    /// </summary>
    /// <param name="dtdSource">DTD source stream.</param>
    void DTD_Impl::parseInternal(ISource &dtdSource)
    {
        while (dtdSource.more() && !dtdSource.match(U"]>"))
        {
            if (dtdSource.match(U"<!ENTITY"))
            {
                parseEntity(dtdSource);
            }
            else if (dtdSource.match(U"<!ELEMENT"))
            {
                parseElement(dtdSource);
            }
            else if (dtdSource.match(U"<!ATTLIST"))
            {
                parseAttributeList(dtdSource);
            }
            else if (dtdSource.match(U"<!NOTATION"))
            {
                parseNotation(dtdSource);
            }
            else if (dtdSource.match(U"<!--"))
            {
                parseComment(dtdSource);
            }
            else if (dtdSource.current() == '%')
            {
                parseParameterEntityReference(dtdSource);
                continue;
            }
            else
            {
                throw SyntaxError(dtdSource, "Invalid DTD tag.");
            }
            if (dtdSource.current() != '>')
            {
                throw SyntaxError(dtdSource, "Missing '>' terminator.");
            }
            dtdSource.next();
            dtdSource.ignoreWS();
        }
    }
    /// <summary>
    /// Parse XML DTD. If the DTD contains an external reference then the DTD
    /// that points to is parsed after any internal DTD that may be specified
    /// after it.
    /// </summary>
    /// <param name="dtdSource">DTD source stream.</param>
    void DTD_Impl::parseDTD(ISource &dtdSource)
    {
        // We take the easy option for allowing a DTD to be stringified
        // and keeping the correct order for its components by storing it
        // in its raw unparsed form.
        long start = dtdSource.position();
        dtdSource.ignoreWS();
        m_xmlNodeDTD.m_parsed->setRootName(Core::parseName(dtdSource));
        // Parse in external DTD reference
        if (dtdSource.current() != '[')
        {
            m_xmlNodeDTD.m_parsed->setExternalReference(parseExternalReference(dtdSource));
        }
        // We have internal DTD so parse that first
        if (dtdSource.current() == '[')
        {
            dtdSource.next();
            dtdSource.ignoreWS();
            parseInternal(dtdSource);
            m_xmlNodeDTD.m_parsed->setType(DTD::DTDType::internal);
        }
        // Missing '>' after external DTD reference
        else if (dtdSource.current() != '>')
        {
            throw SyntaxError(dtdSource, "Missing '>' terminator.");
        }
        // Move to the next component in XML prolog
        else
        {
            dtdSource.next();
            dtdSource.ignoreWS();
        }
        // Parse any DTD in external reference found
        if (!m_xmlNodeDTD.m_parsed->getExternalReference().type.empty())
        {
            parseExternal(dtdSource);
            m_xmlNodeDTD.m_parsed->setType(m_xmlNodeDTD.m_parsed->getType() | DTD::DTDType::external);
        }
        // Save away unparsed form of DTD
        m_xmlNodeDTD.m_unparsed = "<!DOCTYPE" + dtdSource.getRange(start, dtdSource.position());
        // Make sure no defined entity contains recursion
        for (auto &entityName : m_xmlNodeDTD.m_parsed->m_entityMapper.getList())
        {
            m_xmlNodeDTD.m_parsed->m_entityMapper.recursive(entityName.first, entityName.first[0]);
        }
        // Count lines in DTD
        m_xmlNodeDTD.m_parsed->setLineCount(static_cast<long>(std::count(m_xmlNodeDTD.m_unparsed.begin(), m_xmlNodeDTD.m_unparsed.end(), kLineFeed)) + 1);
    }
} // namespace XMLLib
