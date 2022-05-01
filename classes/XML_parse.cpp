//
// Class: XML
//
// Description: XML parser code. All parsing of characters takes place having
// converted the characters to UTF-32 to make the process easier (any data once
// parsed is stored in UTF-8 strings).
//
// Dependencies:   C17++ - Language standard features used.
//
// =================
// CLASS DEFINITIONS
// =================
#include "XMLConfig.hpp"
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
    /// <param name="xmlSource">XML source stream.</param>
    /// <param name="xmlNode">Current element node.</param>
    void XML::parseTagName(ISource &xmlSource, XMLNode *xmlNode)
    {
        static_cast<XMLNodeElement*>(xmlNode)->elementName = parseName(xmlSource);
    }
    /// <summary>
    /// Parse a XML comment, create a XMLNodeComment for it and add to list
    /// of elements for the current XMLNodeElement.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <param name="xmlNode">Current element node.</param>
    void XML::parseComment(ISource &xmlSource, XMLNode *xmlNode)
    {
        XMLNodeComment xmlNodeComment;
        while (xmlSource.more() && !xmlSource.match(U"--"))
        {
            xmlNodeComment.comment += xmlSource.current_to_bytes();
            xmlSource.next();
        }
        if (!xmlSource.match(U">"))
        {
            throw XMLSyntaxError(xmlSource, "Missing closing '>' for comment line.");
        }
        static_cast<XMLNodeElement*>(xmlNode)->children.emplace_back(std::make_unique<XMLNodeComment>(std::move(xmlNodeComment)));
    }
    /// <summary>
    /// Parse a XML process instruction, create an XMLNodePI for it and add it to
    /// the list of elements under the current XMLNodeElement.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <param name="xmlNode">Current element node.</param>
    void XML::parsePI(ISource &xmlSource, XMLNode *xmlNode)
    {
        XMLNodePI xmlNodePI;
        xmlNodePI.name = parseName(xmlSource);
        while (xmlSource.more() && !xmlSource.match(U"?>"))
        {
            xmlNodePI.parameters += xmlSource.current_to_bytes();
            xmlSource.next();
        }
        static_cast<XMLNodeElement*>(xmlNode)->children.emplace_back(std::make_unique<XMLNodePI>(std::move(xmlNodePI)));
    }
    /// <summary>
    /// Parse an XML CDATA section, create an XNodeCDATA for it and add it to
    /// the list of elements under the current XMLNodeElement.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <param name="xmlNode">Current element node.</param>
    void XML::parseCDATA(ISource &xmlSource, XMLNode *xmlNode)
    {
        XMLNodeCDATA xmlNodeCDATA;
        while (xmlSource.more() && !xmlSource.match(U"]]>"))
        {
            if (xmlSource.match(U"<![CDATA["))
            {
                throw XMLSyntaxError(xmlSource, "Nesting of CDATA sections is not allowed.");
            }
            xmlNodeCDATA.cdata += xmlSource.current_to_bytes();
            xmlSource.next();
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
    /// <param name="xmlSource">XML source stream.</param>
    /// <param name="xmlNode">Current element node.</param>
    void XML::parseAttributes(ISource &xmlSource, XMLNode *xmlNode)
    {
        while (xmlSource.more() &&
               xmlSource.current() != '?' &&
               xmlSource.current() != '/' &&
               xmlSource.current() != '>')
        {
            std::string attributeName = parseName(xmlSource);
            if (!xmlSource.match(U"="))
            {
                throw XMLSyntaxError(xmlSource, "Missing '=' between attribute name and value.");
            }
            xmlSource.ignoreWS();
            XMLValue attributeValue = parseValue(xmlSource, *m_entityMapper);
            if (!validAttributeValue(attributeValue))
            {
                throw XMLSyntaxError(xmlSource, "Attribute value contains invalid character '<', '\"', ''' or '&'.");
            }
            if (!static_cast<XMLNodeElement*>(xmlNode)->isAttributePresent(attributeName))
            {
                static_cast<XMLNodeElement*>(xmlNode)->addAttribute(attributeName, attributeValue);
            }
            else
            {
                throw XMLSyntaxError(xmlSource, "Attribute defined more than once within start tag.");
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
    /// <param name="xmlSource">XML source stream.</param>
    /// <param name="xmlNode">Current element node.</param>
    void XML::parseChildElement(ISource &xmlSource, XMLNode *xmlNode)
    {
        XMLNodeElement xmlNodeChildElement;
        for (auto &ns : static_cast<XMLNodeElement*>(xmlNode)->getNameSpaceList())
        {
            xmlNodeChildElement.addNameSpace(ns.name, ns.value);
        }
        parseElement(xmlSource, &xmlNodeChildElement);
        if (auto pos = xmlNodeChildElement.elementName.find(':'); pos != std::string::npos)
        {
            if (!xmlNodeChildElement.isNameSpacePresent(xmlNodeChildElement.elementName.substr(0, pos)))
            {
                throw XMLSyntaxError(xmlSource, "Namespace used but not defined.");
            }
        }
        static_cast<XMLNodeElement*>(xmlNode)->children.push_back(std::make_unique<XMLNodeElement>(std::move(xmlNodeChildElement)));
    }
    /// <summary>
    /// Parse any element content that is found.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <param name="xmlNode">Current element node.</param>
    void XML::parseDefault(ISource &xmlSource, XMLNode *xmlNode)
    {
        XMLValue entityReference = parseCharacter(xmlSource);
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
    /// <param name="xmlSource">XMl source stream.</param>
    /// <param name="xmlNode">Current element node.</param>
    void XML::parseElementContents(ISource &xmlSource, XMLNode *xmlNode)
    {
        if (xmlSource.match(U"<!--"))
        {
            parseComment(xmlSource, xmlNode);
        }
        else if (xmlSource.match(U"<?"))
        {
            parsePI(xmlSource, xmlNode);
        }
        else if (xmlSource.match(U"<![CDATA["))
        {
            parseCDATA(xmlSource, xmlNode);
        }
        else if (xmlSource.match(U"</"))
        {
            throw XMLSyntaxError(xmlSource, "Missing closing tag.");
        }
        else if (xmlSource.match(U"<"))
        {
            parseChildElement(xmlSource, xmlNode);
        }
        else if (xmlSource.match(U"]]>"))
        {
            throw XMLSyntaxError(xmlSource, "']]>' invalid in element content area.");
        }
        else
        {
            parseDefault(xmlSource, xmlNode);
        }
    }
    /// <summary>
    /// Parse current XML element found.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <param name="xmlNode">Current element node.</param>
    void XML::parseElement(ISource &xmlSource, XMLNode *xmlNode)
    {
        parseTagName(xmlSource, xmlNode);
        parseAttributes(xmlSource, xmlNode);
        if (xmlSource.match(U">"))
        {
            while (xmlSource.more() && !xmlSource.match(U"</"))
            {
                parseElementContents(xmlSource, xmlNode);
            }
            if (!xmlSource.match(xmlSource.from_bytes(static_cast<XMLNodeElement*>(xmlNode)->elementName) + U">"))
            {
                throw XMLSyntaxError(xmlSource, "Missing closing tag.");
            }
        }
        else if (xmlSource.match(U"/>"))
        {
            // Self closing element tag
            static_cast<XMLNodeElement*>(xmlNode)->setNodeType(XMLNodeType::self);
        }
        else
        {
            throw XMLSyntaxError(xmlSource, "Missing closing tag.");
        }
    }
    /// <summary>
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <param name="xmlNode">Prolog element node.</param>
    void XML::parseDeclaration(ISource &xmlSource, XMLNode *xmlNode)
    {
        if (xmlSource.match(U"version"))
        {
            xmlSource.ignoreWS();
            if (!xmlSource.match(U"="))
            {
                throw XMLSyntaxError(xmlSource, "Missing '=' after version.");
            }
            xmlSource.ignoreWS();
            static_cast<XMLNodeElement*>(xmlNode)->addAttribute("version", parseValue(xmlSource));
            // Check valid declaration values
            std::set<std::string> validVersions{"1.0", "1.1"};
            if (validVersions.find(static_cast<XMLNodeElement*>(xmlNode)->getAttribute("version").value.parsed) == validVersions.end())
            {
                throw XMLSyntaxError(xmlSource, "Unsupported version " + static_cast<XMLNodeElement*>(xmlNode)->getAttribute("version").value.parsed + ".");
            }
        }
        else
        {
            throw XMLSyntaxError(xmlSource, "Version missing from declaration.");
        }
        if (xmlSource.match(U"encoding"))
        {
            xmlSource.ignoreWS();
            if (!xmlSource.match(U"="))
            {
                throw XMLSyntaxError(xmlSource, "Missing '=' after encoding.");
            }
            xmlSource.ignoreWS();
            static_cast<XMLNodeElement*>(xmlNode)->addAttribute("encoding", parseValue(xmlSource));
            // Check valid declaration values
            toUpperString(static_cast<XMLNodeElement*>(xmlNode)->getAttribute("encoding").value.parsed);
            std::set<std::string> validEncodings{"UTF-8", "UTF-16"};
            if (validEncodings.find(static_cast<XMLNodeElement*>(xmlNode)->getAttribute("encoding").value.parsed) == validEncodings.end())
            {
                throw XMLSyntaxError(xmlSource, "Unsupported encoding " + static_cast<XMLNodeElement*>(xmlNode)->getAttribute("encoding").value.parsed + " specified.");
            }
        }
        else
        {
            static_cast<XMLNodeElement*>(xmlNode)->addAttribute("encoding", {"UTF-8", "UTF-8"});
        }
        if (xmlSource.match(U"standalone"))
        {
            xmlSource.ignoreWS();
            if (!xmlSource.match(U"="))
            {
                throw XMLSyntaxError(xmlSource, "Missing '=' after standalone.");
            }
            xmlSource.ignoreWS();
            static_cast<XMLNodeElement*>(xmlNode)->addAttribute("standalone", parseValue(xmlSource));
            // Check valid declaration values
            std::set<std::string> validStandalone{"yes", "no"};
            if (validStandalone.find(static_cast<XMLNodeElement*>(xmlNode)->getAttribute("standalone").value.parsed) == validStandalone.end())
            {
                throw XMLSyntaxError(xmlSource, "Invalid standalone value of '" + static_cast<XMLNodeElement*>(xmlNode)->getAttribute("standalone").value.parsed + "'.");
            }
        }
        else
        {
            static_cast<XMLNodeElement*>(xmlNode)->addAttribute("standalone", {"no", "no"});
        }
        if (xmlSource.match(U"encoding"))
        {
            throw XMLSyntaxError(xmlSource, "Incorrect order for version, encoding and standalone attributes.");
        }
    }
    /// <summary>
    /// Parse XML prolog and create the necessary XMLNodeElements for it. Valid
    /// parts of the prolog include delaration (first line if present),
    /// processing instructions, comments, whitespace content and XML
    /// Document Type Declaration (DTD).
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <param name="xmlNode">Prolog element node.</param>
    void XML::parseProlog(ISource &xmlSource, XMLNode *xmlNode)
    {
        static_cast<XMLNodeElement*>(xmlNode)->setNodeType(XMLNodeType::prolog);
        xmlSource.ignoreWS();
        if (xmlSource.match(U"<?xml"))
        {
            xmlSource.ignoreWS();
            parseDeclaration(xmlSource, xmlNode);
            if (!xmlSource.match(U"?>"))
            {
                throw XMLSyntaxError(xmlSource, "Declaration end tag not found.");
            }
        }
        while (xmlSource.more())
        {
            if (xmlSource.match(U"<!--"))
            {
                parseComment(xmlSource, xmlNode);
            }
            else if (xmlSource.match(U"<?"))
            {
                parsePI(xmlSource, xmlNode);
            }
            else if (xmlSource.isWS())
            {
                parseAddElementContent(xmlNode, xmlSource.current_to_bytes());
                xmlSource.next();
            }
            else if (xmlSource.match(U"<!DOCTYPE"))
            {
                if (m_dtd == nullptr)
                {
                    m_dtd = std::make_unique<DTD>(*m_entityMapper);
                    m_dtd->parse(xmlSource);
                    m_validator = std::make_unique<XMLValidator>(*m_dtd);
                    static_cast<XMLNodeElement*>(xmlNode)->children.emplace_back(std::make_unique<XMLNodeDTD>());
                }
                else
                {
                    throw XMLSyntaxError(xmlSource, "More than one DOCTYPE declaration.");
                }
            }
            else if (xmlSource.current() != '<')
            {
                throw XMLSyntaxError(xmlSource, "Content detected before root element.");
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
    void XML::parseXML()
    {
        parseProlog(m_xmlSource, &m_prolog);
        if (m_xmlSource.match(U"<"))
        {
            m_prolog.children.emplace_back(std::make_unique<XMLNodeElement>(XMLNodeElement(XMLNodeType::root)));
            parseElement(m_xmlSource, static_cast<XMLNode *>(m_prolog.children.back().get()));
            while (m_xmlSource.more())
            {
                if (m_xmlSource.match(U"<!--"))
                {
                    parseComment(m_xmlSource, &m_prolog);
                }
                else if (m_xmlSource.match(U"<?"))
                {
                    parsePI(m_xmlSource, &m_prolog);
                }
                else if (m_xmlSource.isWS())
                {
                    parseAddElementContent(&m_prolog, m_xmlSource.current_to_bytes());
                    m_xmlSource.next();
                }
                else
                {
                    throw XMLSyntaxError(m_xmlSource, "Extra content at the end of document.");
                }
            }
        }
        else
        {
            throw XMLSyntaxError(m_xmlSource, "Missing root element.");
        }
    }
} // namespace XMLLib
