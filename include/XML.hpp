#pragma once
//
// C++ STL
//
#include <string>
#include <stdexcept>
#include <memory>
//
// XML version
//
#include "XMLConfig.hpp"
// =========
// NAMESPACE
// =========
namespace XMLLib
{
    // ===================================================
    // Forward declarations for interfaces/classes/structs
    // ===================================================
    class ISource;
    class IDestination;
    class IXMLEntityMapper;
    class IXMLValidator;
    class DTD;
    struct XMLNode;
    struct XMLNodeElement;
    struct XMLValue;
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
        XML();
        // ==========
        // DESTRUCTOR
        // ==========
        ~XML();
        // ==============
        // PUBLIC METHODS
        // ==============
        DTD &dtd() { return (*m_dtd); }
        XMLNodeElement &prolog() { return (*m_prolog); };
        void parse(ISource &source);
        void stringify(IDestination &destination);
        void validate();
        // ================
        // PUBLIC VARIABLES
        // ================
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
        std::unique_ptr<XMLNodeElement> m_prolog;
        std::unique_ptr<DTD> m_dtd;
        std::unique_ptr<IXMLValidator> m_validator;
        std::unique_ptr<IXMLEntityMapper> m_entityMapper;
    };
} // namespace XMLLib
