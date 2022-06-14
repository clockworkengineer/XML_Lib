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
    /// <param name="dtdSource">DTD source stream.</param>
    /// <param name="includeOn">If set to false then enclosing conditionals treated as ignored.</param>
    void DTD_Impl::parseConditional(ISource &dtdSource, bool includeOn)
    {
        std::string conditionalValue;
        dtdSource.ignoreWS();
        if (includeOn)
        {
            if (dtdSource.current() == '%')
            {
                conditionalValue = m_xmlNodeDTD.parsed().m_entityMapper.map(Core::parseEntityReference(dtdSource)).parsed;
            }
            else if (dtdSource.match(U"INCLUDE"))
            {
                conditionalValue = "INCLUDE";
            }
            else if (dtdSource.match(U"IGNORE"))
            {
                conditionalValue = "IGNORE";
            }
        }
        else
        {
            conditionalValue = "IGNORE";
        }
        dtdSource.ignoreWS();
        if (conditionalValue == "INCLUDE")
        {
            if (dtdSource.current() != '[')
            {
                throw SyntaxError(dtdSource, "Missing opening '[' from conditional.");
            }
            dtdSource.next();
            dtdSource.ignoreWS();
            std::string conditionalDTD;
            while (dtdSource.more() && !dtdSource.match(U"]]"))
            {
                if (dtdSource.match(U"<!["))
                {
                    parseConditional(dtdSource);
                }
                else
                {
                    conditionalDTD += dtdSource.current_to_bytes();
                    dtdSource.next();
                }
            }
            BufferSource conditionalDTDSource(conditionalDTD);
            parseExternalContent(conditionalDTDSource);
        }
        else if (conditionalValue == "IGNORE")
        {
            while (dtdSource.more() && !dtdSource.match(U"]]"))
            {
                if (dtdSource.match(U"<!["))
                {
                    parseConditional(dtdSource, false);
                }
                else
                {
                    dtdSource.next();
                }
            }
        }
        else
        {
            throw SyntaxError(dtdSource, "Conditional value not INCLUDE or IGNORE.");
        }
        if (dtdSource.current() != '>')
        {
            throw SyntaxError(dtdSource, "Missing '>' terminator.");
        }
        dtdSource.next();
        dtdSource.ignoreWS();
    }
    /// <summary>
    /// Parse external DTD.
    /// </summary>
    /// <param name="dtdSource">DTD source stream.</param>
    void DTD_Impl::parseExternalContent(ISource &dtdSource)
    {
        while (dtdSource.more())
        {
            if (dtdSource.match(U"<!ENTITY"))
            {
                BufferSource dtdTranslatedSource(m_xmlNodeDTD.parsed().m_entityMapper.translate(Core::parseTagBody(dtdSource)));
                parseEntity(dtdTranslatedSource);
            }
            else if (dtdSource.match(U"<!ELEMENT"))
            {
                BufferSource dtdTranslatedSource(m_xmlNodeDTD.parsed().m_entityMapper.translate(Core::parseTagBody(dtdSource)));
                parseElement(dtdTranslatedSource);
            }
            else if (dtdSource.match(U"<!ATTLIST"))
            {
                BufferSource dtdTranslatedSource(m_xmlNodeDTD.parsed().m_entityMapper.translate(Core::parseTagBody(dtdSource)));
                parseAttributeList(dtdTranslatedSource);
            }
            else if (dtdSource.match(U"<!NOTATION"))
            {
                BufferSource dtdTranslatedSource(m_xmlNodeDTD.parsed().m_entityMapper.translate(Core::parseTagBody(dtdSource)));
                parseNotation(dtdTranslatedSource);
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
            else if (dtdSource.match(U"<!["))
            {
                parseConditional(dtdSource);
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
    /// Parse externally defined DTD into XMLNodeDTD.
    /// </summary>
    void DTD_Impl::parseExternalReferenceContent()
    {
        if (m_xmlNodeDTD.parsed().getExternalReference().type == "SYSTEM")
        {
            FileSource dtdFile(m_xmlNodeDTD.parsed().getExternalReference().systemID);
            parseExternalContent(dtdFile);
        }
        else if (m_xmlNodeDTD.parsed().getExternalReference().type == "PUBLIC")
        {
            // Public external DTD currently not supported (Use system id ?)
        }
    }
    /// <summary>
    /// Parse an external reference.
    /// </summary>
    /// <param name="dtdSource">DTD source stream.</param>
    /// <returns>External reference.</returns>
    XMLExternalReference DTD_Impl::parseExternalReference(ISource &dtdSource)
    {
        if (dtdSource.match(U"SYSTEM"))
        {
            dtdSource.ignoreWS();
            return (XMLExternalReference{"SYSTEM", Core::parseValue(dtdSource, m_xmlNodeDTD.parsed().m_entityMapper).parsed, ""});
        }
        else if (dtdSource.match(U"PUBLIC"))
        {
            dtdSource.ignoreWS();
            std::string publicID{Core::parseValue(dtdSource, m_xmlNodeDTD.parsed().m_entityMapper).parsed};
            std::string systemID{Core::parseValue(dtdSource, m_xmlNodeDTD.parsed().m_entityMapper).parsed};
            return (XMLExternalReference{"PUBLIC", systemID, publicID});
        }
        throw SyntaxError(dtdSource, "Invalid external DTD specifier.");
    }
    /// <summary>
    /// Parse externally defined DTD.
    /// </summary>
    /// <param name="dtdSource">DTD source stream.</param>
    void DTD_Impl::parseExternal(ISource & /*dtdSource*/)
    {
        parseExternalReferenceContent();
    }
} // namespace XMLLib
