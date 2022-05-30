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
    void XML_Impl::parseEntityContents(XMLNode &xmlNode, const XMLValue &entityReference)
    {
        XMLNodeElement entityElement;
        BufferSource entitySource(entityReference.parsed);
        // Parse entity XML
        while (entitySource.more())
        {
            parseElementContents(entitySource, entityElement);
        }
        // Place into node (element) child list
        for (auto &xmlNodePtr : entityElement.children)
        {
            xmlNode.children.emplace_back(std::move(xmlNodePtr));
        }
    }
    void XML_Impl::resetIsWhiteSpace(XMLNode &xmlNode)
    {
        if (!XMLNodeRef<XMLNodeElement>(xmlNode).children.empty())
        {
            if (XMLNodeRef<XMLNodeElement>(xmlNode).children.back()->getNodeType() == XMLNodeType::content)
            {
                XMLNodeRef<XMLNodeContent>(*XMLNodeRef<XMLNodeElement>(xmlNode).children.back()).isWhiteSpace = false;
            }
        }
    }
    /// <summary>
    /// Add content node to elements child list.
    /// </summary>
    /// <param name="xmlNode">Current element node.</param>
    /// <param name="content">Content to add to new content node (XMLNodeContent).</param>
    void XML_Impl::addContentToElement(XMLNode &xmlNode, const std::string &content)
    {
        // Make sure there is a content node to receive characters
        if (xmlNode.children.empty() || xmlNode.children.back()->getNodeType() != XMLNodeType::content)
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
        XMLNodeContent &xmlContent = XMLNodeRef<XMLNodeContent>(*xmlNode.children.back());
        if (xmlContent.isWhiteSpace)
        {
            for (const auto ch : content)
            {
                if (!std::iswspace(ch))
                {
                    xmlContent.isWhiteSpace = false;
                    break;
                }
            }
        }
        xmlContent.content += content;
    }
    /// <summary>
    /// Take an entity reference and parse its contents into XML internal description.
    /// </summary>
    /// <param name="xmlNode">Current element node.</param>
    /// <param name="entityReference">Entity reference to be parsed.</param>
    void XML_Impl::parseEntityMappingContents(XMLNode &xmlNode, const XMLValue &entityReference)
    {
        XMLNodeEntityReference xNodeEntityReference(entityReference);
        if (entityReference.unparsed[1] != '#')
        {
            // Does entity contain start tag ?
            // YES then XML into current element list
            if (entityReference.parsed.find("<") != std::string::npos)
            {
                parseEntityContents(xmlNode, entityReference);
                return;
            }
            // NO XML into entity elements list.
            else
            {
                parseEntityContents(xNodeEntityReference, entityReference);
                resetIsWhiteSpace(xmlNode);
            }
        }
        XMLNodeRef<XMLNodeElement>(xmlNode).children.emplace_back(std::make_unique<XMLNodeEntityReference>(std::move(xNodeEntityReference)));
    }
    /// <summary>
    /// Parse a element tag name and set its value in current XMLNodeElement.
    /// </summary>
    /// <param name="source">XML source stream.</param>
    std::string XML_Impl::parseTagName(ISource &source)
    {
        return (Core::parseName(source));
    }
    /// <summary>
    /// Parse a XML comment, create a XMLNodeComment for it and add to list
    /// of elements for the current XMLNodeElement.
    /// </summary>
    /// <param name="source">XML source stream.</param>
    XMLNodePtr XML_Impl::parseComment(ISource &source)
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
        return (std::make_unique<XMLNodeComment>(std::move(xmlNodeComment)));
    }
    /// <summary>
    /// Parse a XML process instruction, create an XMLNodePI for it and add it to
    /// the list of elements under the current XMLNodeElement.
    /// </summary>
    /// <param name="source">XML source stream.</param>
    XMLNodePtr XML_Impl::parsePI(ISource &source)
    {
        XMLNodePI xmlNodePI;
        xmlNodePI.name = Core::parseName(source);
        while (source.more() && !source.match(U"?>"))
        {
            xmlNodePI.parameters += source.current_to_bytes();
            source.next();
        }
        return (std::make_unique<XMLNodePI>(std::move(xmlNodePI)));
    }
    /// <summary>
    /// Parse an XML CDATA section, create an XNodeCDATA for it and add it to
    /// the list of elements under the current XMLNodeElement.
    /// </summary>
    /// <param name="source">XML source stream.</param>
    /// <param name="xmlNode">Current element node.</param>
    XMLNodePtr XML_Impl::parseCDATA(ISource &source)
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
        return (std::make_unique<XMLNodeCDATA>(std::move(xmlNodeCDATA)));
    }
    /// <summary>
    /// Parse list of attributes (name/value pairs) that exist in a tag and add them to
    /// the list of attributes associated with the current XMLNodeElement.
    /// </summary>
    /// <param name="source">XML source stream.</param>
    /// <param name="xmlNode">Current element node.</param>
    XMLAttributeList XML_Impl::parseAttributes(ISource &source)
    {
        XMLAttributeList attributes;
        std::set<std::string> attributeNames;
        while (source.more() && source.current() != '?' && source.current() != '/' && source.current() != '>')
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
            if (!attributeNames.contains(attributeName))
            {
                attributes.emplace_back(attributeName, attributeValue);
                attributeNames.insert(attributeName);
            }
            else
            {
                throw SyntaxError(source, "Attribute defined more than once within start tag.");
            }
        }
        return (attributes);
    }
    /// <summary>
    /// Parse any element content that is found.
    /// </summary>
    /// <param name="source">XML source stream.</param>
    /// <param name="xmlNode">Current element node.</param>
    void XML_Impl::parseElementContent(ISource &source, XMLNode &xmlNode)
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
            addContentToElement(xmlNode, entityReference.parsed);
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
            xmlNode.children.emplace_back(parseComment(source));
        }
        else if (source.match(U"<?"))
        {
            xmlNode.children.emplace_back(parsePI(source));
        }
        else if (source.match(U"<![CDATA["))
        {
            resetIsWhiteSpace(xmlNode);
            xmlNode.children.emplace_back(parseCDATA(source));
        }
        else if (source.match(U"<"))
        {
            xmlNode.children.emplace_back(parseElement(source, XMLNodeRef<XMLNodeElement>(xmlNode).getNameSpaceList()));
            XMLNodeElement &xmlNodeChildElement = XMLNodeRef<XMLNodeElement>(*xmlNode.children.back());
            if (auto pos = xmlNodeChildElement.elementName.find(':'); pos != std::string::npos)
            {
                if (!xmlNodeChildElement.isNameSpacePresent(xmlNodeChildElement.elementName.substr(0, pos)))
                {
                    throw SyntaxError(source, "Namespace used but not defined.");
                }
            }
        }
        else
        {
            if (source.match(U"</"))
            {
                throw SyntaxError(source, "Missing closing tag.");
            }
            else if (source.match(U"]]>"))
            {
                throw SyntaxError(source, "']]>' invalid in element content area.");
            }
            parseElementContent(source, xmlNode);
        }
    }
    /// <summary>
    /// Parse current XML element found.
    /// </summary>
    /// <param name="source">XML source stream.</param>
    /// <param name="namespaces">Current list of namespaces.</param>
    XMLNodePtr XML_Impl::parseElement(ISource &source, const XMLAttributeList &namespaces)
    {
        XMLNodeElement xmlNodeElement;
        for (const auto &ns : namespaces)
        {
            xmlNodeElement.addNameSpace(ns.name, ns.value);
        }
        xmlNodeElement.elementName = parseTagName(source);
        for (const auto &attribute : parseAttributes(source))
        {
            xmlNodeElement.addAttribute(attribute.name, attribute.value);
            if (attribute.name.find("xmlns") == 0)
            {
                xmlNodeElement.addNameSpace((attribute.name.size() > 5) ? attribute.name.substr(6) : ":", attribute.value);
            }
        }
        if (source.match(U">"))
        {
            while (source.more() && !source.match(U"</"))
            {
                parseElementContents(source, xmlNodeElement);
            }
            if (!source.match(source.from_bytes(xmlNodeElement.elementName) + U">"))
            {
                throw SyntaxError(source, "Missing closing tag.");
            }
        }
        else if (source.match(U"/>"))
        {
            // Self closing element tag
            xmlNodeElement.setNodeType(XMLNodeType::self);
        }
        else
        {
            throw SyntaxError(source, "Missing closing tag.");
        }
        return (std::make_unique<XMLNodeElement>(std::move(xmlNodeElement)));
    }
    /// <summary>
    /// </summary>
    /// <param name="source">XML source stream.</param>
    XMLNodePtr XML_Impl::parseDeclaration(ISource &source)
    {
        XMLNodeDeclaration declaration;
        if (source.match(U"<?xml"))
        {
            source.ignoreWS();
            if (source.match(U"version"))
            {
                source.ignoreWS();
                if (!source.match(U"="))
                {
                    throw SyntaxError(source, "Missing '=' after version.");
                }
                source.ignoreWS();
                declaration.version = Core::parseValue(source).parsed;
                if (!declaration.isValidVersion())
                {
                    throw SyntaxError(source, "Unsupported version " + declaration.version + ".");
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
                declaration.encoding = Core::toUpperString(Core::parseValue(source).parsed);
                if (!declaration.isValidEncoding())
                {
                    throw SyntaxError(source, "Unsupported encoding " + declaration.encoding + " specified.");
                }
            }
            if (source.match(U"standalone"))
            {
                source.ignoreWS();
                if (!source.match(U"="))
                {
                    throw SyntaxError(source, "Missing '=' after standalone.");
                }
                source.ignoreWS();
                declaration.standalone = Core::parseValue(source).parsed;
                if (!declaration.isValidStandalone())
                {
                    throw SyntaxError(source, "Invalid standalone value of '" + declaration.standalone + "'.");
                }
            }
            if (source.match(U"encoding"))
            {
                throw SyntaxError(source, "Incorrect order for version, encoding and standalone attributes.");
            }
            if (!source.match(U"?>"))
            {
                throw SyntaxError(source, "Declaration end tag not found.");
            }
        }
        return (std::make_unique<XMLNodeDeclaration>(std::move(declaration)));
    }
    /// <summary>
    /// </summary>
    /// <param name="source">XML source stream.</param>
    void XML_Impl::parseXMLTail(ISource &source)
    {
        while (source.more())
        {
            if (source.match(U"<!--"))
            {
                prolog().children.emplace_back(parseComment(source));
            }
            else if (source.match(U"<?"))
            {
                prolog().children.emplace_back(parsePI(source));
            }
            else if (source.isWS())
            {
                addContentToElement(prolog(), source.current_to_bytes());
                source.next();
            }
            else
            {
                throw SyntaxError(source, "Extra content at the end of document.");
            }
        }
    }
    XMLNodePtr XML_Impl::parseDTD(ISource &source)
    {
        if (m_dtd == nullptr)
        {
            m_dtd = std::make_unique<DTD>(*m_entityMapper);
            m_dtd->parse(source);
            m_validator = std::make_unique<XML_Validator>(m_dtd->parsed());
        }
        else
        {
            throw SyntaxError(source, "More than one DOCTYPE declaration.");
        }
        return (std::make_unique<XMLNodeDTD>());
    }
    /// <summary>
    /// Parse XML prolog and create the necessary XMLNodeElements for it. Valid
    /// parts of the prolog include declaration (first line if present),
    /// processing instructions, comments, whitespace content and XML
    /// Document Type Declaration (DTD).
    /// </summary>
    /// <param name="source">XML source stream.</param>
    XMLNodePtr XML_Impl::parseProlog(ISource &source)
    {
        XMLNodeProlog xmlNodeProlog{XMLNodeType::prolog};
        source.ignoreWS();
        xmlNodeProlog.children.emplace_back(parseDeclaration(source));
        while (source.more())
        {
            if (source.match(U"<!--"))
            {
                xmlNodeProlog.children.emplace_back(parseComment(source));
            }
            else if (source.match(U"<?"))
            {
                xmlNodeProlog.children.emplace_back(parsePI(source));
            }
            else if (source.isWS())
            {
                addContentToElement(xmlNodeProlog, source.current_to_bytes());
                source.next();
            }
            else if (source.match(U"<!DOCTYPE"))
            {
                xmlNodeProlog.children.emplace_back(parseDTD(source));
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
        return (std::make_unique<XMLNodeProlog>(std::move(xmlNodeProlog)));
    }
    /// <summary>
    /// Parse XML from source stream.
    /// </summary>
    void XML_Impl::parseXML(ISource &source)
    {
        m_prolog = parseProlog(source);
        if (source.match(U"<"))
        {
            prolog().children.emplace_back(parseElement(source, {}));
            if (prolog().children.back()->getNodeType() != XMLNodeType::self)
            {
                prolog().children.back()->setNodeType(XMLNodeType::root);
            }
            parseXMLTail(source);
        }
        else
        {
            throw SyntaxError(source, "Missing root element.");
        }
    }
} // namespace XMLLib
