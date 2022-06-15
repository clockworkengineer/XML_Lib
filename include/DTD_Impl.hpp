#pragma once
//
// C++ STL
//
#include <string>
#include <unordered_map>
//
// XML
//
#include "XML_Core.hpp"
#include "XML_Errors.hpp"
#include "XML_Sources.hpp"
#include "XML_Destinations.hpp"
// =========
// NAMESPACE
// =========
namespace XMLLib
{
    // ================
    // CLASS DEFINITION
    // ================
    class DTD_Impl
    {
    public:
        // ==========================
        // PUBLIC TYPES AND CONSTANTS
        // ==========================
        // ============
        // CONSTRUCTORS
        // ============
        DTD_Impl(XMLNodeDTD &xmlNodeDTD);
        // ==========
        // DESTRUCTOR
        // ==========
        ~DTD_Impl();
        // ==============
        // PUBLIC METHODS
        // ==============
        void parse(ISource &dtdSource);
        void stringify(IDestination &destination);
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

        void parseValidNotations(const std::string &notations);
        void parseValidateAttribute(const std::string &elementName, const XMLNodeDTD::Attribute &dtdAttribute);
        void parseConditional(ISource &dtdSource, bool includeOff = true);
        void parseParameterEntities(ISource &dtdSource);
        void parseExternalContent(ISource &dtdSource);
        std::string parseAttributeEnumerationType(ISource &source);
        bool parseIsChoiceOrSequence(ISource &contentSpecSource);
        void parseElementCP(ISource &contentSpecSource, IDestination &contentSpec);
        void parseElementChoice(ISource &contentSpecSource, IDestination &contentSpec);
        void parseElementSequence(ISource &contentSpecSource, IDestination &contentSpec);
        void parseElementChildren(ISource &contentSpecSource, IDestination &contentSpec);
        void parseElementName(ISource &contentSpecSource, IDestination &contentSpec);
        void parseElementMixedContent(ISource &contentSpecSource, IDestination &contentSpec);
        XMLValue parseElementContentSpecification(const std::string &elementName, const XMLValue &contentSpec);
        void parseExternalReferenceContent();
        void parseAttributeList(ISource &dtdSource);
        void parseNotation(ISource &dtdSource);
        void parseEntity(ISource &dtdSource);
        void parseElement(ISource &dtdSource);
        XMLExternalReference parseExternalReference(ISource &dtdSource);
        void parseAttributeType(ISource &dtdSource, XMLNodeDTD::Attribute &attribute);
        void parseAttributeValue(ISource &dtdSource, XMLNodeDTD::Attribute &attribute);
        void parseComment(ISource &dtdSource);
        void parseParameterEntityReference(ISource &dtdSource);
        void parseExternal(ISource &dtdSource);
        void parseInternal(ISource &dtdSource);
        void parseDTD(ISource &dtdSource);
        // =================
        // PRIVATE VARIABLES
        // =================
        XMLNodeDTD &m_xmlNodeDTD;
    };
} // namespace XMLLib
