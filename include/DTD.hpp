#pragma once
//
// C++ STL
//
#include <string>
//
// XML Core
//
#include "XML_core.hpp"
//
// DTD element/attrribute
//
#include "DTDElement.hpp"
// =========
// NAMESPACE
// =========
namespace H4
{
    // ================
    // CLASS DEFINITION
    // ================
    class DTD
    {
    public:
        // ==========================
        // PUBLIC TYPES AND CONSTANTS
        // ==========================
        //
        // DTD Type
        //
        enum DTDType : uint8_t
        {
            internal = (0x1 << 0),
            external = (0x1 << 1)
        };
        // ============
        // CONSTRUCTORS
        // ============
        DTD(IXMLEntityMapper &entityMapper) : m_entityMapper(entityMapper) {}
        // ==========
        // DESTRUCTOR
        // ==========
        // ==============
        // PUBLIC METHODS
        // ==============
        uint16_t getType() const;
        std::string &getRootName();
        XMLExternalReference &getExternalReference();
        bool isElementPresent(const std::string &elementName) const;
        long getElementCount() const { return (static_cast<long> (m_elements.size())); }
        DTDElement &getElement(const std::string &elementName);
        bool isEntityPresent(const std::string &entityName) const ;
        XMLEntityMapping &getEntity(const std::string &entityName);
        XMLExternalReference &getNotation(const std::string &notationName);
        long getLineCount() const;
        void parse(ISource &dtdSource);
        void stringify(IDestination &xmlDestination);
        void validate(XMLNodeElement &prolog);
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
        void parseConditional(ISource &dtdSource, bool includeOff = true);
        void parseValidNotations(const std::string &notations);
        void parseValidateAttribute(const std::string &elementName, DTDAttribute xDTDAttribute);
        void parseParameterEntities(ISource &dtdSource);
        void parseExternalContent(ISource &dtdSource);
        std::string parseAttributeEnumerationType(ISource &xmlSource);
        bool parseIsChoiceOrSequence(ISource &contentSpecSource);
        void parseElementCP(ISource &contentSpecSource, IDestination &contentSpec);
        void parseElementChoice(ISource &contentSpecSource, IDestination &contentSpec);
        void parseElementSequence(ISource &contentSpecSource, IDestination &contentSpec);
        void parseElementChildren(ISource &contentSpecSource, IDestination &contentSpec);
        void parseElementName(ISource &contentSpecSource, IDestination &contentSpec);
        void parseElementMixedContent(ISource &contentSpecSource, IDestination &contentSpec);
        void parseElementContentSpecification(const std::string &elementName, XMLValue &contentSpec);
        void parseExternalRefenceContent();
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
        uint16_t m_type = 0;
        std::string m_name;
        XMLExternalReference m_external;
        std::unordered_map<std::string, DTDElement> m_elements;
        std::unordered_map<std::string, XMLExternalReference> m_notations;
        IXMLEntityMapper &m_entityMapper;
        std::string m_unparsed;
    };
} // namespace H4
