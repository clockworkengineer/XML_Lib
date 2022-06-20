#pragma once
// =======
// C++ STL
// =======
#include <string>
#include <set>
#include <regex>
// ===
// XML
// ===
#include "XML_Types.hpp"
#include "XML_Core.hpp"
#include "XML_Sources.hpp"
// ========================================
// XML Validator / Entity Mapper interfaces
// ========================================
#include "IXMLValidator.hpp"
#include "IXMLEntityMapper.hpp"
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
        ~XML_Validator() override = default;
        // ==========
        // DESTRUCTOR
        // ==========
        // ==============
        // PUBLIC METHODS
        // ==============
        void validate(const XMLNodeProlog &prolog) override;
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
        void checkAttributes(const XMLNode &xmlNode);
        void checkContentSpecification(const XMLNode &xmlNode);
        void checkElement(const XMLNode &xmlNode);
        void checkElements(const XMLNode &xmlNode);
        bool checkIsNMTOKENOK(const std::string &nmTokenValue);
        bool checkIsIDOK(const std::string &idValue);
        void checkAttributeValue(const XMLNode &xmlNode, const XMLNodeDTD::Attribute &attribute);
        void checkAttributeType(const XMLNode &xmlNode, const XMLNodeDTD::Attribute &attribute);
        bool checkIsPCDATA(const XMLNode &xmlNode);
        bool checkIsEMPTY(const XMLNode &xmlNode);
        void checkAgainstDTD(const XMLNodeProlog &prolog);
        // =================
        // PRIVATE VARIABLES
        // =================
        std::set<std::string> m_assignedIDValues;
        std::set<std::string> m_assignedIDREFValues;
        long m_lineNumber = 1;
        XMLNodeDTD &m_xmlNodeDTD;
    };
} // namespace XMLLib
