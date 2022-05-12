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
// XML version
//
#include "XMLConfig.hpp"
//
// XML Core
//
#include "XML_core.hpp"
// =========
// NAMESPACE
// =========
namespace XMLLib
{
    //
    // XML syntax error
    //
    struct SyntaxError : public std::exception
    {
    public:
        SyntaxError(const std::string &description = "")
        {
            errorMessage = "XML Syntax Error: " + description;
        }
        SyntaxError(ISource &source, const std::string &description = "")
        {
            errorMessage = "XML Syntax Error [Line: " + std::to_string(source.getLineNo()) +
                           " Column: " + std::to_string(source.getColumnNo()) + "] " + description;
        }
        virtual const char *what() const throw()
        {
            return (errorMessage.c_str());
        }

    private:
        std::string errorMessage;
    };
    // ===================================================
    // Forward declarations for interfaces/classes/structs
    // ===================================================
    class IXMLEntityMapper;
    class DTD;
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
