#pragma once
//
// C++ STL
//
#include <string>
#include <stdexcept>
#include <memory>
//
// XML
//
#include "XML_Config.hpp"
#include "XML_Types.hpp"
#include "XML_Nodes.hpp"
#include "XML_Core.hpp"
#include "XML_Validator.hpp"
#include "XML_Errors.hpp"
#include "XML_Sources.hpp"
#include "XML_Destinations.hpp"
#include "XML_EntityMapper.hpp"
// =========
// NAMESPACE
// =========
namespace XMLLib
{
    // ================
    // CLASS DEFINITION
    // ================
    class XML_Impl
    {
    public:
        // ==========================
        // PUBLIC TYPES AND CONSTANTS
        // ==========================
        // ============
        // CONSTRUCTORS
        // ============
        XML_Impl();
        // ==========
        // DESTRUCTOR
        // ==========
        ~XML_Impl();
        // ==============
        // PUBLIC METHODS
        // ==============
        DTD &dtd() { return (*m_dtd); }
        XMLNodeElement &prolog() { return (XMLNodeRef<XMLNodeElement>(*m_prolog)); };
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
        void addContentToElement(XMLNode &xmlNode, const std::string &content);
        void parseEntityMappingContents(XMLNode &xmlNode, const XMLValue &entityReference);
        void parseElementContent(ISource &source, XMLNode &xmlNode);
        std::string parseTagName(ISource &source);
        XMLAttributeList parseAttributes(ISource &source);
        XMLNodePtr parseComment(ISource &source);
        XMLNodePtr parseCDATA(ISource &source);
        XMLNodePtr parsePI(ISource &source);
        void parseElementContents(ISource &source, XMLNode &xmlNode);
        XMLNodePtr parseElement(ISource &source, const XMLAttributeList &namespaces);
        XMLAttributeList parseDeclaration(ISource &source);
        XMLNodePtr parseProlog(ISource &source);
        void parseXML(ISource &source);
        void stringifyElements(XMLNode &xmlNode, IDestination &destination);
        void stringifyXML(IDestination &destination);
        // =================
        // PRIVATE VARIABLES
        // =================
        XMLNodePtr m_prolog;
        std::unique_ptr<DTD> m_dtd;
        std::unique_ptr<IXMLValidator> m_validator;
        std::unique_ptr<IXMLEntityMapper> m_entityMapper;
    };
} // namespace XMLLib
