//
// Class: XML_Impl
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
#include "XML_Impl.hpp"
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
    void XML_Impl::parseEntityMappingContents(XMLNode &xmlNode, const XMLValue &entityReference)
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
                    parseElementContents(entitySource, entityElement);
                }
                xNodeEntityReference.children = std::move(entityElement.children);
                if (!XMLNodeRef<XMLNodeElement>(xmlNode).children.empty())
                {
                    if (XMLNodeRef<XMLNodeElement>(xmlNode).children.back()->getNodeType() == XMLNodeType::content)
                    {
                        XMLNodeRef<XMLNodeContent>(*XMLNodeRef<XMLNodeElement>(xmlNode).children.back()).isWhiteSpace = false;
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
        XMLNodeRef<XMLNodeElement>(xmlNode).children.emplace_back(std::make_unique<XMLNodeEntityReference>(std::move(xNodeEntityReference)));
    }
    /// <summary>
    /// Add content node to current XMLNodeElement elements list.
    /// </summary>
    /// <param name="xmlNode">Current element node.</param>
    /// <param name="content">Content to add to new content node (XMLNodeContent).</param>
    void XML_Impl::parseAddElementContent(XMLNodeElement &xmlNode, const std::string &content)
    {
        // Make sure there is a content node to receive characters
        if (xmlNode.children.empty() ||
            xmlNode.children.back()->getNodeType() != XMLNodeType::content)
        {
            bool isWWhitespace = true;
            if (!xmlNode.children.empty())
            {
                if ((xmlNode.children.back()->getNodeType() == XMLNodeType::cdata) ||
                    (xmlNode.children.back()->getNodeType() == XMLNodeType::entity))
                {
                    isWWhitespace = false;
                }
            }
            xmlNode.children.emplace_back(std::make_unique<XMLNodeContent>());
            XMLNodeRef<XMLNodeContent>(*xmlNode.children.back()).isWhiteSpace = isWWhitespace;
        }
        if (XMLNodeRef<XMLNodeContent>(*xmlNode.children.back()).isWhiteSpace)
        {
            for (auto ch : content)
            {
                if (!std::iswspace(ch))
                {
                    XMLNodeRef<XMLNodeContent>(*xmlNode.children.back()).isWhiteSpace = false;
                    break;
                }
            }
        }
        XMLNodeRef<XMLNodeContent>(*xmlNode.children.back()).content += content;
    }
    /// <summary>
    /// Parse a element tag name and set its value in current XMLNodeElement.
    /// </summary>
    /// <param name="source">XML source stream.</param>
    /// <param name="xmlNode">Current element node.</param>
    void XML_Impl::parseTagName(ISource &source, XMLNode &xmlNode)
    {
        XMLNodeRef<XMLNodeElement>(xmlNode).elementName = Core::parseName(source);
    }
    /// <summary>
    /// Parse a XML comment, create a XMLNodeComment for it and add to list
    /// of elements for the current XMLNodeElement.
    /// </summary>
    /// <param name="source">XML source stream.</param>
    /// <param name="xmlNode">Current element node.</param>
    XMLNodeComment XML_Impl::parseComment(ISource &source)
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
        return (xmlNodeComment);
    }
    /// <summary>
    /// Parse a XML process instruction, create an XMLNodePI for it and add it to
    /// the list of elements under the current XMLNodeElement.
    /// </summary>
    /// <param name="source">XML source stream.</param>
    XMLNodePI XML_Impl::parsePI(ISource &source)
    {
        XMLNodePI xmlNodePI;
        xmlNodePI.name = Core::parseName(source);
        while (source.more() && !source.match(U"?>"))
        {
            xmlNodePI.parameters += source.current_to_bytes();
            source.next();
        }
        return (xmlNodePI);
    }
    /// <summary>
    /// Parse an XML CDATA section, create an XNodeCDATA for it and add it to
    /// the list of elements under the current XMLNodeElement.
    /// </summary>
    /// <param name="source">XML source stream.</param>
    /// <param name="xmlNode">Current element node.</param>
    void XML_Impl::parseCDATA(ISource &source, XMLNode &xmlNode)
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
        if (!XMLNodeRef<XMLNodeElement>(xmlNode).children.empty())
        {
            if (XMLNodeRef<XMLNodeElement>(xmlNode).children.back()->getNodeType() == XMLNodeType::content)
            {
                XMLNodeRef<XMLNodeContent>(*XMLNodeRef<XMLNodeElement>(xmlNode).children.back()).isWhiteSpace = false;
            }
        }
        XMLNodeRef<XMLNodeElement>(xmlNode).children.emplace_back(std::make_unique<XMLNodeCDATA>(std::move(xmlNodeCDATA)));
    }
    /// <summary>
    /// Parse list of attributes (name/value pairs) that exist in a tag and add them to
    /// the list of attributes associated with the current XMLNodeElement.
    /// </summary>
    /// <param name="source">XML source stream.</param>
    /// <param name="xmlNode">Current element node.</param>
    void XML_Impl::parseAttributes(ISource &source, XMLNode &xmlNode)
    {
        while (source.more() &&
               source.current() != '?' &&
               source.current() != '/' &&
               source.current() != '>')
        {
            std::string attributeName = Core::parseName(source);
            if (!source.match(U"="))
            {
                throw SyntaxError(source, "Missing '=' between attribute name and value.");
            }
            source.ignoreWS();
            XMLValue attributeValue = Core::parseValue(source, *m_entityMapper);
            if (!Core::validAttributeValue(attributeValue))
            {
                throw SyntaxError(source, "Attribute value contains invalid character '<', '\"', ''' or '&'.");
            }
            if (!XMLNodeRef<XMLNodeElement>(xmlNode).isAttributePresent(attributeName))
            {
                XMLNodeRef<XMLNodeElement>(xmlNode).addAttribute(attributeName, attributeValue);
            }
            else
            {
                throw SyntaxError(source, "Attribute defined more than once within start tag.");
            }
        }
        for (const auto attribute : XMLNodeRef<XMLNodeElement>(xmlNode).getAttributeList())
        {
            if (attribute.name.find("xmlns") == 0)
            {
                XMLNodeRef<XMLNodeElement>(xmlNode).addNameSpace((attribute.name.size() > 5) ? attribute.name.substr(6) : ":", attribute.value);
            }
        }
    }
    /// <summary>
    /// Recursively parse any child elements of the current XMLNodeElement.
    /// </summary>
    /// <param name="source">XML source stream.</param>
    /// <param name="xmlNode">Current element node.</param>
    void XML_Impl::parseChildElement(ISource &source, XMLNode &xmlNode)
    {
        XMLNodeElement xmlNodeChildElement;
        for (auto &ns : XMLNodeRef<XMLNodeElement>(xmlNode).getNameSpaceList())
        {
            xmlNodeChildElement.addNameSpace(ns.name, ns.value);
        }
        parseElement(source, xmlNodeChildElement);
        if (auto pos = xmlNodeChildElement.elementName.find(':'); pos != std::string::npos)
        {
            if (!xmlNodeChildElement.isNameSpacePresent(xmlNodeChildElement.elementName.substr(0, pos)))
            {
                throw SyntaxError(source, "Namespace used but not defined.");
            }
        }
        XMLNodeRef<XMLNodeElement>(xmlNode).children.push_back(std::make_unique<XMLNodeElement>(std::move(xmlNodeChildElement)));
    }
    /// <summary>
    /// Parse any element content that is found.
    /// </summary>
    /// <param name="source">XML source stream.</param>
    /// <param name="xmlNode">Current element node.</param>
    void XML_Impl::parseDefault(ISource &source, XMLNode &xmlNode)
    {
        XMLValue entityReference{Core::parseCharacter(source)};
        if (entityReference.isEntityReference())
        {
            parseEntityMappingContents(xmlNode, m_entityMapper->map(entityReference));
        }
        else if (entityReference.isCharacterReference())
        {
            parseEntityMappingContents(xmlNode, entityReference);
        }
        else
        {
            parseAddElementContent(static_cast<XMLNodeElement&>(xmlNode), entityReference.parsed);
        }
    }
    /// <summary>
    /// Parse element content area, generating any XNodes and adding them
    /// to the list of the current XMLNodeElement.
    /// </summary>
    /// <param name="source">XMl source stream.</param>
    /// <param name="xmlNode">Current element node.</param>
    void XML_Impl::parseElementContents(ISource &source, XMLNode &xmlNode)
    {
        if (source.match(U"<!--"))
        {
            xmlNode.children.emplace_back(std::make_unique<XMLNodeComment>(std::move(parseComment(source))));
        }
        else if (source.match(U"<?"))
        {
            XMLNodeRef<XMLNodeElement>(xmlNode).children.emplace_back(std::make_unique<XMLNodePI>(std::move(parsePI(source))));
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
    void XML_Impl::parseElement(ISource &source, XMLNode &xmlNode)
    {
        parseTagName(source, xmlNode);
        parseAttributes(source, xmlNode);
        if (source.match(U">"))
        {
            while (source.more() && !source.match(U"</"))
            {
                parseElementContents(source, xmlNode);
            }
            if (!source.match(source.from_bytes(XMLNodeRef<XMLNodeElement>(xmlNode).elementName) + U">"))
            {
                throw SyntaxError(source, "Missing closing tag.");
            }
        }
        else if (source.match(U"/>"))
        {
            // Self closing element tag
            XMLNodeRef<XMLNodeElement>(xmlNode).setNodeType(XMLNodeType::self);
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
    std::vector<XMLAttribute> XML_Impl::parseDeclaration(ISource &source)
    {
        std::vector<XMLAttribute> declaration;
        std::string value;
        if (source.match(U"version"))
        {
            source.ignoreWS();
            if (!source.match(U"="))
            {
                throw SyntaxError(source, "Missing '=' after version.");
            }
            source.ignoreWS();
            value = Core::parseValue(source).parsed;
            // Check valid declaration values
            std::set<std::string> validVersions{"1.0", "1.1"};
            if (!validVersions.contains(value))
            {
                throw SyntaxError(source, "Unsupported version " + value + ".");
            }
            declaration.emplace_back(XMLAttribute{"version", XMLValue{value, value}});
        }
        else
        {
            throw SyntaxError(source, "Version missing from declaration.");
        }
        value = "UTF-8"; // Default
        if (source.match(U"encoding"))
        {
            source.ignoreWS();
            if (!source.match(U"="))
            {
                throw SyntaxError(source, "Missing '=' after encoding.");
            }
            source.ignoreWS();
            // Check valid declaration values
            value = Core::toUpperString(Core::parseValue(source).parsed);
            std::set<std::string> validEncodings{"UTF-8", "UTF-16"};
            if (!validEncodings.contains(value))
            {
                throw SyntaxError(source, "Unsupported encoding " + value + " specified.");
            }
        }
        declaration.emplace_back(XMLAttribute{"encoding", XMLValue{value, value}});
        value = "no"; // Default
        if (source.match(U"standalone"))
        {
            source.ignoreWS();
            if (!source.match(U"="))
            {
                throw SyntaxError(source, "Missing '=' after standalone.");
            }
            source.ignoreWS();
            value = Core::parseValue(source).parsed;
            // Check valid declaration values
            std::set<std::string> validStandalone{"yes", "no"};
            if (!validStandalone.contains(value))
            {
                throw SyntaxError(source, "Invalid standalone value of '" + value + "'.");
            }
        }
        declaration.emplace_back(XMLAttribute{"standalone", XMLValue{value, value}});
        if (source.match(U"encoding"))
        {
            throw SyntaxError(source, "Incorrect order for version, encoding and standalone attributes.");
        }
        return (declaration);
    }
    /// <summary>
    /// Parse XML prolog and create the necessary XMLNodeElements for it. Valid
    /// parts of the prolog include declaration (first line if present),
    /// processing instructions, comments, whitespace content and XML
    /// Document Type Declaration (DTD).
    /// </summary>
    /// <param name="source">XML source stream.</param>
    /// <param name="xmlNode">Prolog element node.</param>
    XMLNodePtr XML_Impl::parseProlog(ISource &source)
    {
        XMLNodeElement xmlNode{XMLNodeType::prolog};
        source.ignoreWS();
        if (source.match(U"<?xml"))
        {
            source.ignoreWS();
            for (auto &attribute : parseDeclaration(source))
            {
                xmlNode.addAttribute(attribute.name, attribute.value);
            }
            if (!source.match(U"?>"))
            {
                throw SyntaxError(source, "Declaration end tag not found.");
            }
        }
        while (source.more())
        {
            if (source.match(U"<!--"))
            {

                xmlNode.children.emplace_back(std::make_unique<XMLNodeComment>(std::move(parseComment(source))));
            }
            else if (source.match(U"<?"))
            {
                xmlNode.children.emplace_back(std::make_unique<XMLNodePI>(std::move(parsePI(source))));
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
                    m_validator = std::make_unique<XML_Validator>(m_dtd->parsed());
                    xmlNode.children.emplace_back(std::make_unique<XMLNodeDTD>());
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
        return (std::make_unique<XMLNodeElement>(std::move(xmlNode)));
    }
    /// <summary>
    /// Parse XML source stream.
    /// </summary>
    void XML_Impl::parseXML(ISource &source)
    {
        m_prolog = parseProlog(source);
        if (source.match(U"<"))
        {
            prolog().children.emplace_back(std::make_unique<XMLNodeElement>(XMLNodeType::root));
            parseElement(source, XMLNodeRef<XMLNode>(*prolog().children.back()));
            while (source.more())
            {
                if (source.match(U"<!--"))
                {
                    m_prolog->children.emplace_back(std::make_unique<XMLNodeComment>(std::move(parseComment(source))));
                }
                else if (source.match(U"<?"))
                {
                    prolog().children.emplace_back(std::make_unique<XMLNodePI>(std::move(parsePI(source))));
                }
                else if (source.isWS())
                {
                    parseAddElementContent(static_cast<XMLNodeElement &>(*m_prolog), source.current_to_bytes());
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
