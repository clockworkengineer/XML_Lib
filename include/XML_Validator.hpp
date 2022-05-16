#pragma once
//
// C++ STL
//
#include <string>
#include <set>
//
// XML DTD
//
#include "DTD.hpp"
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
        // ============
        // CONSTRUCTORS
        // ============
        explicit XML_Validator(DTD &dtd) : m_dtd(dtd) {}
        // ==========
        // DESTRUCTOR
        // ==========
        // ==============
        // PUBLIC METHODS
        // ==============
        void validate(XMLNodeElement &prolog) override;
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
        void checkAttributes(XMLNode &xmlNode);
        void checkContentSpecification(XMLNode &xmlNode);
        void checkElement(XMLNode &xmlNode);
        void checkElements(XMLNode &xmlNode);
        bool checkIsNMTOKENOK(std::string nmTokenValue);
        bool checkIsIDOK(const std::string &idValue);
        void checkAttributeValue(XMLNode &xmlNode, DTDAttribute &attribute);
        void checkAttributeType(XMLNode &xmlNode, DTDAttribute &attribute);
        bool checkIsPCDATA(XMLNode &xmlNode);
        bool checkIsEMPTY(XMLNode &xmlNode);
        void checkAgainstDTD(XMLNodeElement &prolog);
        // =================
        // PRIVATE VARIABLES
        // =================
        std::set<std::string> m_assignedIDValues;
        std::set<std::string> m_assignedIDREFValues;
        long m_lineNumber = 1;
        DTD &m_dtd;
    };
} // namespace XMLLib
