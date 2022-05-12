//
// Class: XML
//
// Description:  Code to take an XML internal object description and produce 
// the XML text for it; currently as UTF-8 encoded text but this may change in 
// future.
//
// Dependencies:   C20++ - Language standard features used.
//
// =================
// CLASS DEFINITIONS
// =================
#include "XML.hpp"
#include "DTD.hpp"
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
    /// Recursively parse XMLNode passed in to produce XML output on an XML
    /// destination stream in UTF-8 encoding.
    /// </summary>
    /// <param name="xmlNode">XMLNode to convert into XML.</param>
    /// <param name="destination">XML destination stream.</param>
    void XML::stringifyElements(XMLNode *xmlNode, IDestination &destination)
    {
        switch (xmlNode->getNodeType())
        {
        // XML prolog
        case XMLNodeType::prolog:
        {
            destination.add("<?xml version=\"" + XMLNodeRef<XMLNodeElement>((*xmlNode)).getAttribute("version").value.unparsed + "\"" +
                               " encoding=\"" + XMLNodeRef<XMLNodeElement>((*xmlNode)).getAttribute("encoding").value.unparsed + "\"" +
                               " standalone=\"" + XMLNodeRef<XMLNodeElement>((*xmlNode)).getAttribute("standalone").value.unparsed + "\"?>");
            for (auto &element : XMLNodeRef<XMLNodeElement>((*xmlNode)).children)
            {
                stringifyElements(element.get(), destination);
            }
            break;
        }
        // XML root or child elements
        case XMLNodeType::root:
        case XMLNodeType::element:
        {
            XMLNodeElement *xmlNodeElement = static_cast<XMLNodeElement *>(xmlNode);
            destination.add("<" + xmlNodeElement->elementName);
            for (auto attr : xmlNodeElement->getAttributeList())
            {
                destination.add(" " + attr.name + "=\"" + attr.value.unparsed + "\"");
            }
            destination.add(">");
            for (auto &element : XMLNodeRef<XMLNodeElement>((*xmlNode)).children)
            {
                stringifyElements(element.get(), destination);
            }
            destination.add("</" + xmlNodeElement->elementName + ">");
            break;
        }
        // Self closing element
        case XMLNodeType::self:
        {
            XMLNodeElement *xmlNodeElement = static_cast<XMLNodeElement *>(xmlNode);
            destination.add("<" + xmlNodeElement->elementName);
            for (auto attr : xmlNodeElement->getAttributeList())
            {
                destination.add(" " + attr.name + "=\"" + attr.value.unparsed + "\"");
            }
            destination.add("/>");
            break;
        }
        // XML comments
        case XMLNodeType::comment:
        {
            XMLNodeComment *xmlNodeComment = static_cast<XMLNodeComment *>(xmlNode);
            destination.add("<!--" + xmlNodeComment->comment + "-->");
            break;
        }
        // XML element content
        case XMLNodeType::content:
        {
            XMLNodeContent *xmlNodeContent = static_cast<XMLNodeContent *>(xmlNode);
            destination.add(xmlNodeContent->content);
            break;
        }
        // XML character entity
        case XMLNodeType::entity:
        {
            XMLNodeEntityReference *xmlNodeEntity = static_cast<XMLNodeEntityReference *>(xmlNode);
            destination.add(xmlNodeEntity->value.unparsed);
            break;
        }
        // XML processing instruction
        case XMLNodeType::pi:
        {
            XMLNodePI *xmlNodePI = static_cast<XMLNodePI *>(xmlNode);
            destination.add("<?" + xmlNodePI->name + " " + xmlNodePI->parameters + "?>");
            break;
        }
        // XML CDATA section
        case XMLNodeType::cdata:
        {
            XMLNodeCDATA *xmlNodeCDATA = static_cast<XMLNodeCDATA *>(xmlNode);
            destination.add("<![CDATA[" + xmlNodeCDATA->cdata + "]]>");
            break;
        }
        // XML DTD
        case XMLNodeType::dtd:
        {
            getDTD().stringify(destination);
            break;
        }
        default:
            throw std::runtime_error("Invalid XMLNode encountered during stringify.");
        }
    }
    /// <summary>
    /// Recursively parse XMLNode passed in to produce XML output on an XML
    /// destination stream in UTF-8 encoding.
    /// </summary>
    /// <param name="destination">XML destination stream.</param>
    void XML::stringifyXML(IDestination &destination)
    {
        stringifyElements(&m_prolog, destination);
    }
} // namespace XMLLib
