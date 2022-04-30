#pragma once
//
// C++ STL
//
#include <string>
#include <stdexcept>
#include <memory>
#include <unordered_map>
#include <set>
#include <vector>
#include <cwctype>
#include <sstream>
//
// XML Core
//
#include "XML_core.hpp"
#include "XMLEntityMapper.hpp"
//
// XML DTD
//
#include "DTD.hpp"
// =========
// NAMESPACE
// =========
namespace H4
{
    // ================
    // CLASS DEFINITION
    // ================
    class XML
    {
    public:
        // ==========================
        // PUBLIC TYPES AND CONSTANTS
        // ==========================
        // ============
        // CONSTRUCTORS
        // ============
        XML(ISource &xmlSource) : m_xmlSource(xmlSource)
        {
            m_entityMapper = std::make_unique<XMLEntityMapper>();
        }
        // ==========
        // DESTRUCTOR
        // ==========
        // ==============
        // PUBLIC METHODS
        // ==============
        DTD &getDTD() { return (*m_dtd); }
        void parse();
        void stringify(IDestination &xmlDestination);
        void validate();
        // ================
        // PUBLIC VARIABLES
        // ================
        XMLNodeElement m_prolog;

    private:
        // ===========================
        // PRIVATE TYPES AND CONSTANTS
        // ===========================
        // ===========================================
        // DISABLED CONSTRUCTORS/DESTRUCTORS/OPERATORS
        // ===========================================
        // ===============
        // PRIVATE METHODS
        // ===============
        void parseAddElementContent(XMLNode *xmlNode, const std::string &content);
        void parseEntityMappingContents(XMLNode *xmlNode, XMLValue &entityReference);
        void parseDefault(ISource &xmlSource, XMLNode *xmlNode);
        void parseTagName(ISource &xmlSource, XMLNode *xmlNode);
        void parseChildElement(ISource &xmlSource, XMLNode *xmlNode);
        void parseAttributes(ISource &xmlSource, XMLNode *xmlNode);
        void parseComment(ISource &xmlSource, XMLNode *xmlNode);
        void parseCDATA(ISource &xmlSource, XMLNode *xmlNode);
        void parsePI(ISource &xmlSource, XMLNode *xmlNode);
        void parseElementContents(ISource &xmlSource, XMLNode *XmlNode);
        void parseElement(ISource &xmlSource, XMLNode *XmlNode);
        void parseDeclaration(ISource &xmlSource, XMLNode *xmlNode);
        void parseProlog(ISource &xmlSource, XMLNode *xmlNode);
        void parseXML();
        void stringifyElements(XMLNode *xmlNode, IDestination &xmlDestination);
        void stringifyXML(IDestination &xmlDestination);
        // =================
        // PRIVATE VARIABLES
        // =================
        ISource &m_xmlSource;
        std::unique_ptr<DTD> m_dtd;
        std::unique_ptr<IXMLValidator> m_validator;
        std::unique_ptr<IXMLEntityMapper> m_entityMapper;
    };
} // namespace H4
