#pragma once
//
// C++ STL
//
#include <string>
#include <set>
#include <regex>
//
// XML Validator interface
//
#include "IXMLValidator.hpp"
// =========
// NAMESPACE
// =========
namespace XMLLib
{
    // ================
    // CLASS DEFINITION
    // ================
    class XML_Validator : public IXMLValidator
    {
    public:
        // ==========================
        // PUBLIC TYPES AND CONSTANTS
        // ==========================
        // ======================
        // CONSTRUCTOR/DESTRUCTOR
        // ======================
        explicit XML_Validator(XMLNodeDTD &xmlNodeDTD) : m_xmlNodeDTD(xmlNodeDTD) {}
        XML_Validator(const XML_Validator &other) = delete;
        XML_Validator &operator=(const XML_Validator &other) = delete;
        XML_Validator(XML_Validator &&other) = delete;
        XML_Validator &operator=(XML_Validator &&other) = delete;
        ~XML_Validator() = default;
        // ==========
        // DESTRUCTOR
        // ==========
        // ==============
        // PUBLIC METHODS
        // ==============
        void validate(XMLNodeProlog &prolog) override;
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
        void elementError(const XMLNodeElement &xNodeElement, const std::string &error);
        void checkAttributes(XMLNode &xmlNode);
        void checkContentSpecification(XMLNode &xmlNode);
        void checkElement(XMLNode &xmlNode);
        void checkElements(XMLNode &xmlNode);
        bool checkIsNMTOKENOK(const std::string &nmTokenValue);
        bool checkIsIDOK(const std::string &idValue);
        void checkAttributeValue(XMLNode &xmlNode, const XMLNodeDTD::Attribute &attribute);
        void checkAttributeType(XMLNode &xmlNode, const XMLNodeDTD::Attribute &attribute);
        bool checkIsPCDATA(XMLNode &xmlNode);
        bool checkIsEMPTY(XMLNode &xmlNode);
        void checkAgainstDTD(XMLNodeProlog &prolog);
        // =================
        // PRIVATE VARIABLES
        // =================
        std::set<std::string> m_assignedIDValues;
        std::set<std::string> m_assignedIDREFValues;
        long m_lineNumber = 1;
        XMLNodeDTD &m_xmlNodeDTD;
    };
} // namespace XMLLib
