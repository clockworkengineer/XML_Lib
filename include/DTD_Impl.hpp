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
#include "IXMLEntityMapper.hpp"
//
// DTD parsed data structure
//
#include "DTD_Parsed.hpp"
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
        explicit DTD_Impl(IXMLEntityMapper &entityMapper);
        // ==========
        // DESTRUCTOR
        // ==========
        ~DTD_Impl();
        // ==============
        // PUBLIC METHODS
        // ==============
        DTDParsed &parsed() { return (*m_parsed); }
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
        void parseValidateAttribute(const std::string &elementName, const DTDAttribute &dtdAttribute);
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
        void parseAttributeType(ISource &dtdSource, DTDAttribute &attribute);
        void parseAttributeValue(ISource &dtdSource, DTDAttribute &attribute);
        void parseComment(ISource &dtdSource);
        void parseParameterEntityReference(ISource &dtdSource);
        void parseExternal(ISource &dtdSource);
        void parseInternal(ISource &dtdSource);
        void parseDTD(ISource &dtdSource);
        // =================
        // PRIVATE VARIABLES
        // =================
        std::unique_ptr<DTDParsed> m_parsed;
        std::string m_unparsed;
    };
} // namespace XMLLib
