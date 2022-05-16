#pragma once
//
// C++ STL
//
#include <string>
#include <unordered_map>
#include <memory>
//
// XML Core
//
#include "XML_Core.hpp"
//
// DTD element/attribute
//
#include "DTD_Element.hpp"
// =========
// NAMESPACE
// =========
namespace XMLLib
{
    // ===================================================
    // Forward declarations for interfaces/classes/structs
    // ===================================================
    class DTD_Impl;
    class ISource;
    class IDestination;
    struct XMLNodeElement;
    class IXMLEntityMapper;
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
        explicit DTD(IXMLEntityMapper &entityMapper);
        // ==========
        // DESTRUCTOR
        // ==========
        ~DTD();
        // ==============
        // PUBLIC METHODS
        // ==============
        [[nodiscard]] uint16_t getType() const;
        std::string &getRootName();
        XMLExternalReference &getExternalReference();
        [[nodiscard]] bool isElementPresent(const std::string &elementName) const;
        [[nodiscard]] long getElementCount() const;
        DTDElement &getElement(const std::string &elementName);
        [[nodiscard]] bool isEntityPresent(const std::string &entityName) const;
        XMLEntityMapping &getEntity(const std::string &entityName);
        XMLExternalReference &getNotation(const std::string &notationName);
        [[nodiscard]] long getLineCount() const;
        void parse(ISource &dtdSource);
        void stringify(IDestination &destination);
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
        // =================
        // PRIVATE VARIABLES
        // =================
        std::unique_ptr<DTD_Impl> m_implementation;
    };
} // namespace XMLLib
