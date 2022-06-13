#pragma once
//
// C++ STL
//
#include <string>
#include <unordered_map>
#include <memory>
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
    class IXMLEntityMapper;
    struct DTDElement;
    struct DTDParsed;
    struct XMLNodeElement;
    struct XMLNodeDTD;
    struct XMLEntityMapping;
    struct XMLExternalReference;
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
        DTD(XMLNodeDTD &xmlNodeDTD, IXMLEntityMapper &entityMapper);
        // ==========
        // DESTRUCTOR
        // ==========
        ~DTD();
        // ==============
        // PUBLIC METHODS
        // ==============
        DTDParsed &parsed();
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
        // =================
        // PRIVATE VARIABLES
        // =================
        std::unique_ptr<DTD_Impl> m_implementation;
    };
} // namespace XMLLib
