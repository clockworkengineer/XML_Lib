//
// Class: XML
//
// Description: XML parser code. All parsing of characters takes place having
// converted the characters to UTF-32 to make the process easier (any data once
// parsed is stored in UTF-8 strings).
//
// Dependencies:   C20++ - Language standard features used.
//
// =================
// CLASS DEFINITIONS
// =================
#include "XML.hpp"
#include "XML_validator.hpp"
#include "XML_sources.hpp"
#include "XML_destinations.hpp"
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
    /// Take an entity reference and parse its contents into XML internal description.
    /// </summary>
    /// <param name="xmlNode">Current XML Element.</param>
    /// <param name="entityReference">Entity reference to be parsed.</param>
    void XML::parseEntityMappingContents(XMLNode *xmlNode, XMLValue &entityReference)
    {
        XMLNodeEntityReference xNodeEntityReference(entityReference);
        if (entityReference.unparsed[1] != '#')
        {
            XMLNodeElement entityElement;
            BufferSource entitySource(entityReference.parsed);
            // Does contain start tag ?
            // NO then parse XML into entity elements list.
            if (entityReference.parsed.find("<") == std::string::npos)
            {
                while (entitySource.more())
                {
                    parseElementContents(entitySource, &entityElement);
                }
                xNodeEntityReference.children = std::move(entityElement.children);
                if (!static_cast<XMLNodeElement*>(xmlNode)->children.empty())
                {
                    if (static_cast<XMLNodeElement*>(xmlNode)->children.back()->getNodeType() == XMLNodeType::content)
                    {
                        XMLNodeRef<XMLNodeContent>(*static_cast<XMLNodeElement*>(xmlNode)->children.back()).isWhiteSpace = false;
                    }
                }
            }
            else
            // YES then parse XML into current element list
            {
                while (entitySource.more())
                {
                    parseElementContents(entitySource, xmlNode);
                }
                return;
            }
        }
        static_cast<XMLNodeElement*>(xmlNode)->children.emplace_back(std::make_unique<XMLNodeEntityReference>(std::move(xNodeEntityReference)));
    }
    /// <summary>
    /// Add content node to current XMLNodeElement elements list.
    /// </summary>
    /// <param name="xmlNode">Current element node.</param>
    /// <param name="content">Content to add to new content node (XMLNodeCotent).</param>
    void XML::parseAddElementContent(XMLNode *xmlNode, const std::string &content)
    {
        // Make sure there is a content node to recieve characters
        if (static_cast<XMLNodeElement*>(xmlNode)->children.empty() ||
            static_cast<XMLNodeElement*>(xmlNode)->children.back()->getNodeType() != XMLNodeType::content)
        {
            bool isWWhitespace = true;
            if (!static_cast<XMLNodeElement*>(xmlNode)->children.empty())
            {
                if ((static_cast<XMLNodeElement*>(xmlNode)->children.back()->getNodeType() == XMLNodeType::cdata) ||
                    (static_cast<XMLNodeElement*>(xmlNode)->children.back()->getNodeType() == XMLNodeType::entity))
                {
                    isWWhitespace = false;
                }
            }
            static_cast<XMLNodeElement*>(xmlNode)->children.emplace_back(std::make_unique<XMLNodeContent>());
            XMLNodeRef<XMLNodeContent>(*static_cast<XMLNodeElement*>(xmlNode)->children.back()).isWhiteSpace = isWWhitespace;
        }
        if (XMLNodeRef<XMLNodeContent>(*static_cast<XMLNodeElement*>(xmlNode)->children.back()).isWhiteSpace)
        {
            for (auto ch : content)
            {
                if (!std::iswspace(ch))
                {
                    XMLNodeRef<XMLNodeContent>(*static_cast<XMLNodeElement*>(xmlNode)->children.back()).isWhiteSpace = false;
                    break;
                }
            }
        }
        XMLNodeRef<XMLNodeContent>(*static_cast<XMLNodeElement*>(xmlNode)->children.back()).content += content;
    }
    /// <summary>
    /// Parse a element tag name and set its value in current XMLNodeElement.
    /// </summary>
    /// <param name="source">XML source stream.</param>
    /// <param name="xmlNode">Current element node.</param>
    void XML::parseTagName(ISource &source, XMLNode *xmlNode)
    {
        static_cast<XMLNodeElement*>(xmlNode)->elementName = parseName(source);
    }
    /// <summary>
    /// Parse a XML comment, create a XMLNodeComment for it and add to list
    /// of elements for the current XMLNodeElement.
    /// </summary>
    /// <param name="source">XML source stream.</param>
    /// <param name="xmlNode">Current element node.</param>
    void XML::parseComment(ISource &source, XMLNode *xmlNode)
    {
        XMLNodeComment xmlNodeComment;
        while (source.more() && !source.match(U"--"))
        {
            xmlNodeComment.comment += source.current_to_bytes();
            source.next();
        }
        if (!source.match(U">"))
        {
            throw SyntaxError(source, "Missing closing '>' for comment line.");
        }
        static_cast<XMLNodeElement*>(xmlNode)->children.emplace_back(std::make_unique<XMLNodeComment>(std::move(xmlNodeComment)));
    }
    /// <summary>
    /// Parse a XML process instruction, create an XMLNodePI for it and add it to
    /// the list of elements under the current XMLNodeElement.
    /// </summary>
    /// <param name="source">XML source stream.</param>
    /// <param name="xmlNode">Current element node.</param>
    void XML::parsePI(ISource &source, XMLNode *xmlNode)
    {
        XMLNodePI xmlNodePI;
        xmlNodePI.name = parseName(source);
        while (source.more() && !source.match(U"?>"))
        {
            xmlNodePI.parameters += source.current_to_bytes();
            source.next();
        }
        static_cast<XMLNodeElement*>(xmlNode)->children.emplace_back(std::make_unique<XMLNodePI>(std::move(xmlNodePI)));
    }
    /// <summary>
    /// Parse an XML CDATA section, create an XNodeCDATA for it and add it to
    /// the list of elements under the current XMLNodeElement.
    /// </summary>
    /// <param name="source">XML source stream.</param>
    /// <param name="xmlNode">Current element node.</param>
    void XML::parseCDATA(ISource &source, XMLNode *xmlNode)
    {
        XMLNodeCDATA xmlNodeCDATA;
        while (source.more() && !source.match(U"]]>"))
        {
            if (source.match(U"<![CDATA["))
            {
                throw SyntaxError(source, "Nesting of CDATA sections is not allowed.");
            }
            xmlNodeCDATA.cdata += source.current_to_bytes();
            source.next();
        }
        if (!static_cast<XMLNodeElement*>(xmlNode)->children.empty())
        {
            if (static_cast<XMLNodeElement*>(xmlNode)->children.back()->getNodeType() == XMLNodeType::content)
            {
                XMLNodeRef<XMLNodeContent>(*static_cast<XMLNodeElement*>(xmlNode)->children.back()).isWhiteSpace = false;
            }
        }
        static_cast<XMLNodeElement*>(xmlNode)->children.emplace_back(std::make_unique<XMLNodeCDATA>(std::move(xmlNodeCDATA)));
    }
    /// <summary>
    /// Parse list of attributes (name/value pairs) that exist in a tag and add them to
    /// the list of attributes associated with the current XMLNodeElement.
    /// </summary>
    /// <param name="source">XML source stream.</param>
    /// <param name="xmlNode">Current element node.</param>
    void XML::parseAttributes(ISource &source, XMLNode *xmlNode)
    {
        while (source.more() &&
               source.current() != '?' &&
               source.current() != '/' &&
               source.current() != '>')
        {
            std::string attributeName = parseName(source);
            if (!source.match(U"="))
            {
                throw SyntaxError(source, "Missing '=' between attribute name and value.");
            }
            source.ignoreWS();
            XMLValue attributeValue = parseValue(source, *m_entityMapper);
            if (!validAttributeValue(attributeValue))
            {
                throw SyntaxError(source, "Attribute value contains invalid character '<', '\"', ''' or '&'.");
            }
            if (!static_cast<XMLNodeElement*>(xmlNode)->isAttributePresent(attributeName))
            {
                static_cast<XMLNodeElement*>(xmlNode)->addAttribute(attributeName, attributeValue);
            }
            else
            {
                throw SyntaxError(source, "Attribute defined more than once within start tag.");
            }
        }
        for (auto attribute : static_cast<XMLNodeElement*>(xmlNode)->getAttributeList())
        {
            if (attribute.name.find("xmlns") == 0)
            {
                attribute.name = (attribute.name.size() > 5) ? attribute.name.substr(6) : ":";
                static_cast<XMLNodeElement*>(xmlNode)->addNameSpace(attribute.name, attribute.value);
            }
        }
    }
    /// <summary>
    /// Recursively parse any child elements of the current XMLNodeElement.
    /// </summary>
    /// <param name="source">XML source stream.</param>
    /// <param name="xmlNode">Current element node.</param>
    void XML::parseChildElement(ISource &source, XMLNode *xmlNode)
    {
        XMLNodeElement xmlNodeChildElement;
        for (auto &ns : static_cast<XMLNodeElement*>(xmlNode)->getNameSpaceList())
        {
            xmlNodeChildElement.addNameSpace(ns.name, ns.value);
        }
        parseElement(source, &xmlNodeChildElement);
        if (auto pos = xmlNodeChildElement.elementName.find(':'); pos != std::string::npos)
        {
            if (!xmlNodeChildElement.isNameSpacePresent(xmlNodeChildElement.elementName.substr(0, pos)))
            {
                throw SyntaxError(source, "Namespace used but not defined.");
            }
        }
        static_cast<XMLNodeElement*>(xmlNode)->children.push_back(std::make_unique<XMLNodeElement>(std::move(xmlNodeChildElement)));
    }
    /// <summary>
    /// Parse any element content that is found.
    /// </summary>
    /// <param name="source">XML source stream.</param>
    /// <param name="xmlNode">Current element node.</param>
    void XML::parseDefault(ISource &source, XMLNode *xmlNode)
    {
        XMLValue entityReference = parseCharacter(source);
        if (entityReference.isEntityReference())
        {
            m_entityMapper->map(entityReference);
            parseEntityMappingContents(xmlNode, entityReference);
        }
        else if (entityReference.isCharacterReference())
        {
            parseEntityMappingContents(xmlNode, entityReference);
        }
        else
        {
            parseAddElementContent(xmlNode, entityReference.parsed);
        }
    }
    /// <summary>
    /// Parse element content area, generating any XNodes and adding them
    /// to the list of the current XMLNodeElement.
    /// </summary>
    /// <param name="source">XMl source stream.</param>
    /// <param name="xmlNode">Current element node.</param>
    void XML::parseElementContents(ISource &source, XMLNode *xmlNode)
    {
        if (source.match(U"<!--"))
        {
            parseComment(source, xmlNode);
        }
        else if (source.match(U"<?"))
        {
            parsePI(source, xmlNode);
        }
        else if (source.match(U"<![CDATA["))
        {
            parseCDATA(source, xmlNode);
        }
        else if (source.match(U"</"))
        {
            throw SyntaxError(source, "Missing closing tag.");
        }
        else if (source.match(U"<"))
        {
            parseChildElement(source, xmlNode);
        }
        else if (source.match(U"]]>"))
        {
            throw SyntaxError(source, "']]>' invalid in element content area.");
        }
        else
        {
            parseDefault(source, xmlNode);
        }
    }
    /// <summary>
    /// Parse current XML element found.
    /// </summary>
    /// <param name="source">XML source stream.</param>
    /// <param name="xmlNode">Current element node.</param>
    void XML::parseElement(ISource &source, XMLNode *xmlNode)
    {
        parseTagName(source, xmlNode);
        parseAttributes(source, xmlNode);
        if (source.match(U">"))
        {
            while (source.more() && !source.match(U"</"))
            {
                parseElementContents(source, xmlNode);
            }
            if (!source.match(source.from_bytes(static_cast<XMLNodeElement*>(xmlNode)->elementName) + U">"))
            {
                throw SyntaxError(source, "Missing closing tag.");
            }
        }
        else if (source.match(U"/>"))
        {
            // Self closing element tag
            static_cast<XMLNodeElement*>(xmlNode)->setNodeType(XMLNodeType::self);
        }
        else
        {
            throw SyntaxError(source, "Missing closing tag.");
        }
    }
    /// <summary>
    /// </summary>
    /// <param name="source">XML source stream.</param>
    /// <param name="xmlNode">Prolog element node.</param>
    void XML::parseDeclaration(ISource &source, XMLNode *xmlNode)
    {
        if (source.match(U"version"))
        {
            source.ignoreWS();
            if (!source.match(U"="))
            {
                throw SyntaxError(source, "Missing '=' after version.");
            }
            source.ignoreWS();
            static_cast<XMLNodeElement*>(xmlNode)->addAttribute("version", parseValue(source));
            // Check valid declaration values
            std::set<std::string> validVersions{"1.0", "1.1"};
            if (validVersions.find(static_cast<XMLNodeElement*>(xmlNode)->getAttribute("version").value.parsed) == validVersions.end())
            {
                throw SyntaxError(source, "Unsupported version " + static_cast<XMLNodeElement*>(xmlNode)->getAttribute("version").value.parsed + ".");
            }
        }
        else
        {
            throw SyntaxError(source, "Version missing from declaration.");
        }
        if (source.match(U"encoding"))
        {
            source.ignoreWS();
            if (!source.match(U"="))
            {
                throw SyntaxError(source, "Missing '=' after encoding.");
            }
            source.ignoreWS();
            static_cast<XMLNodeElement*>(xmlNode)->addAttribute("encoding", parseValue(source));
            // Check valid declaration values
            toUpperString(static_cast<XMLNodeElement*>(xmlNode)->getAttribute("encoding").value.parsed);
            std::set<std::string> validEncodings{"UTF-8", "UTF-16"};
            if (validEncodings.find(static_cast<XMLNodeElement*>(xmlNode)->getAttribute("encoding").value.parsed) == validEncodings.end())
            {
                throw SyntaxError(source, "Unsupported encoding " + static_cast<XMLNodeElement*>(xmlNode)->getAttribute("encoding").value.parsed + " specified.");
            }
        }
        else
        {
            static_cast<XMLNodeElement*>(xmlNode)->addAttribute("encoding", {"UTF-8", "UTF-8"});
        }
        if (source.match(U"standalone"))
        {
            source.ignoreWS();
            if (!source.match(U"="))
            {
                throw SyntaxError(source, "Missing '=' after standalone.");
            }
            source.ignoreWS();
            static_cast<XMLNodeElement*>(xmlNode)->addAttribute("standalone", parseValue(source));
            // Check valid declaration values
            std::set<std::string> validStandalone{"yes", "no"};
            if (validStandalone.find(static_cast<XMLNodeElement*>(xmlNode)->getAttribute("standalone").value.parsed) == validStandalone.end())
            {
                throw SyntaxError(source, "Invalid standalone value of '" + static_cast<XMLNodeElement*>(xmlNode)->getAttribute("standalone").value.parsed + "'.");
            }
        }
        else
        {
            static_cast<XMLNodeElement*>(xmlNode)->addAttribute("standalone", {"no", "no"});
        }
        if (source.match(U"encoding"))
        {
            throw SyntaxError(source, "Incorrect order for version, encoding and standalone attributes.");
        }
    }
    /// <summary>
    /// Parse XML prolog and create the necessary XMLNodeElements for it. Valid
    /// parts of the prolog include delaration (first line if present),
    /// processing instructions, comments, whitespace content and XML
    /// Document Type Declaration (DTD).
    /// </summary>
    /// <param name="source">XML source stream.</param>
    /// <param name="xmlNode">Prolog element node.</param>
    void XML::parseProlog(ISource &source, XMLNode *xmlNode)
    {
        static_cast<XMLNodeElement*>(xmlNode)->setNodeType(XMLNodeType::prolog);
        source.ignoreWS();
        if (source.match(U"<?xml"))
        {
            source.ignoreWS();
            parseDeclaration(source, xmlNode);
            if (!source.match(U"?>"))
            {
                throw SyntaxError(source, "Declaration end tag not found.");
            }
        }
        while (source.more())
        {
            if (source.match(U"<!--"))
            {
                parseComment(source, xmlNode);
            }
            else if (source.match(U"<?"))
            {
                parsePI(source, xmlNode);
            }
            else if (source.isWS())
            {
                parseAddElementContent(xmlNode, source.current_to_bytes());
                source.next();
            }
            else if (source.match(U"<!DOCTYPE"))
            {
                if (m_dtd == nullptr)
                {
                    m_dtd = std::make_unique<DTD>(*m_entityMapper);
                    m_dtd->parse(source);
                    m_validator = std::make_unique<XMLValidator>(*m_dtd);
                    static_cast<XMLNodeElement*>(xmlNode)->children.emplace_back(std::make_unique<XMLNodeDTD>());
                }
                else
                {
                    throw SyntaxError(source, "More than one DOCTYPE declaration.");
                }
            }
            else if (source.current() != '<')
            {
                throw SyntaxError(source, "Content detected before root element.");
            }
            else
            {
                // Break out as root element detected
                break;
            }
        }
    }
    /// <summary>
    /// Parse XML source stream.
    /// </summary>
    void XML::parseXML(ISource &source)
    {
        parseProlog(source, &m_prolog);
        if (source.match(U"<"))
        {
            m_prolog.children.emplace_back(std::make_unique<XMLNodeElement>(XMLNodeElement(XMLNodeType::root)));
            parseElement(source, static_cast<XMLNode *>(m_prolog.children.back().get()));
            while (source.more())
            {
                if (source.match(U"<!--"))
                {
                    parseComment(source, &m_prolog);
                }
                else if (source.match(U"<?"))
                {
                    parsePI(source, &m_prolog);
                }
                else if (source.isWS())
                {
                    parseAddElementContent(&m_prolog, source.current_to_bytes());
                    source.next();
                }
                else
                {
                    throw SyntaxError(source, "Extra content at the end of document.");
                }
            }
        }
        else
        {
            throw SyntaxError(source, "Missing root element.");
        }
    }
} // namespace XMLLib
