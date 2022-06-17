//
// Class: DTD_impl
//
// Description: Parse external XML DTD.
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
    /// Parse conditional DTD (recursively if necessary).
    /// </summary>
    /// <param name="source">DTD source stream.</param>
    /// <param name="includeOn">If set to false then enclosing conditionals treated as ignored.</param>
    void DTD_Impl::parseConditional(ISource &source, bool includeOn)
    {
        std::string conditionalValue;
        source.ignoreWS();
        if (includeOn)
        {
            if (source.current() == '%')
            {
                conditionalValue = m_xmlNodeDTD.m_entityMapper.map(Core::parseEntityReference(source)).parsed;
            }
            else if (source.match(U"INCLUDE"))
            {
                conditionalValue = "INCLUDE";
            }
            else if (source.match(U"IGNORE"))
            {
                conditionalValue = "IGNORE";
            }
        }
        else
        {
            conditionalValue = "IGNORE";
        }
        source.ignoreWS();
        if (conditionalValue == "INCLUDE")
        {
            if (source.current() != '[')
            {
                throw SyntaxError(source, "Missing opening '[' from conditional.");
            }
            source.next();
            source.ignoreWS();
            std::string conditionalDTD;
            while (source.more() && !source.match(U"]]"))
            {
                if (source.match(U"<!["))
                {
                    parseConditional(source);
                }
                else
                {
                    conditionalDTD += source.current_to_bytes();
                    source.next();
                }
            }
            BufferSource conditionalDTDSource(conditionalDTD);
            parseExternalContent(conditionalDTDSource);
        }
        else if (conditionalValue == "IGNORE")
        {
            while (source.more() && !source.match(U"]]"))
            {
                if (source.match(U"<!["))
                {
                    parseConditional(source, false);
                }
                else
                {
                    source.next();
                }
            }
        }
        else
        {
            throw SyntaxError(source, "Conditional value not INCLUDE or IGNORE.");
        }
        if (source.current() != '>')
        {
            throw SyntaxError(source, "Missing '>' terminator.");
        }
        source.next();
        source.ignoreWS();
    }
    /// <summary>
    /// Parse external DTD.
    /// </summary>
    /// <param name="source">DTD source stream.</param>
    void DTD_Impl::parseExternalContent(ISource &source)
    {
        while (source.more())
        {
            if (source.match(U"<!ENTITY"))
            {
                BufferSource dtdTranslatedSource(m_xmlNodeDTD.m_entityMapper.translate(Core::parseTagBody(source)));
                parseEntity(dtdTranslatedSource);
            }
            else if (source.match(U"<!ELEMENT"))
            {
                BufferSource dtdTranslatedSource(m_xmlNodeDTD.m_entityMapper.translate(Core::parseTagBody(source)));
                parseElement(dtdTranslatedSource);
            }
            else if (source.match(U"<!ATTLIST"))
            {
                BufferSource dtdTranslatedSource(m_xmlNodeDTD.m_entityMapper.translate(Core::parseTagBody(source)));
                parseAttributeList(dtdTranslatedSource);
            }
            else if (source.match(U"<!NOTATION"))
            {
                BufferSource dtdTranslatedSource(m_xmlNodeDTD.m_entityMapper.translate(Core::parseTagBody(source)));
                parseNotation(dtdTranslatedSource);
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
            else if (source.match(U"<!["))
            {
                parseConditional(source);
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
    /// Parse externally defined DTD into XMLNodeDTD.
    /// </summary>
    void DTD_Impl::parseExternalReferenceContent()
    {
        if (m_xmlNodeDTD.getExternalReference().type == "SYSTEM")
        {
            FileSource dtdFile(m_xmlNodeDTD.getExternalReference().systemID);
            parseExternalContent(dtdFile);
        }
        else if (m_xmlNodeDTD.getExternalReference().type == "PUBLIC")
        {
            // Public external DTD currently not supported (Use system id ?)
        }
    }
    /// <summary>
    /// Parse an external reference.
    /// </summary>
    /// <param name="source">DTD source stream.</param>
    /// <returns>External reference.</returns>
    XMLExternalReference DTD_Impl::parseExternalReference(ISource &source)
    {
        if (source.match(U"SYSTEM"))
        {
            source.ignoreWS();
            return (XMLExternalReference{"SYSTEM", Core::parseValue(source, m_xmlNodeDTD.m_entityMapper).parsed, ""});
        }
        else if (source.match(U"PUBLIC"))
        {
            source.ignoreWS();
            std::string publicID{Core::parseValue(source, m_xmlNodeDTD.m_entityMapper).parsed};
            std::string systemID{Core::parseValue(source, m_xmlNodeDTD.m_entityMapper).parsed};
            return (XMLExternalReference{"PUBLIC", systemID, publicID});
        }
        throw SyntaxError(source, "Invalid external DTD specifier.");
    }
    /// <summary>
    /// Parse externally defined DTD.
    /// </summary>
    /// <param name="source">DTD source stream.</param>
    void DTD_Impl::parseExternal(ISource & /*source*/)
    {
        parseExternalReferenceContent();
    }
} // namespace XMLLib
