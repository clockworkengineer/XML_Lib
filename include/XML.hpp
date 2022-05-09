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
namespace XMLLib
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
        XML()
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
        void parse(ISource &source);
        void stringify(IDestination &destination);
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
        void parseDefault(ISource &source, XMLNode *xmlNode);
        void parseTagName(ISource &source, XMLNode *xmlNode);
        void parseChildElement(ISource &source, XMLNode *xmlNode);
        void parseAttributes(ISource &source, XMLNode *xmlNode);
        void parseComment(ISource &source, XMLNode *xmlNode);
        void parseCDATA(ISource &source, XMLNode *xmlNode);
        void parsePI(ISource &source, XMLNode *xmlNode);
        void parseElementContents(ISource &source, XMLNode *XmlNode);
        void parseElement(ISource &source, XMLNode *XmlNode);
        void parseDeclaration(ISource &source, XMLNode *xmlNode);
        void parseProlog(ISource &source, XMLNode *xmlNode);
        void parseXML(ISource &source);
        void stringifyElements(XMLNode *xmlNode, IDestination &destination);
        void stringifyXML(IDestination &destination);
        // =================
        // PRIVATE VARIABLES
        // =================
        std::unique_ptr<DTD> m_dtd;
        std::unique_ptr<IXMLValidator> m_validator;
        std::unique_ptr<IXMLEntityMapper> m_entityMapper;
    };
} // namespace XMLLib
