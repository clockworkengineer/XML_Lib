#pragma once
//
// C++ STL
//
#include <memory>
//
// XML version
//
#include "XML_Config.hpp"
// =========
// NAMESPACE
// =========
namespace XMLLib
{
    // ===================================================
    // Forward declarations for interfaces/classes/structs
    // ===================================================
    class XML_Impl;
    class ISource;
    class IDestination;
    struct XMLNodeElement;
    struct XMLNodeProlog;
    struct XMLNodeDeclaration;
    struct XMLNodeDTD;
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
        XMLNodeDTD &dtd();
        XMLNodeProlog &prolog();
        XMLNodeDeclaration &declaration();
        XMLNodeElement &root();
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
        // =================
        // PRIVATE VARIABLES
        // =================
        const std::unique_ptr<XML_Impl> m_implementation;
    };
} // namespace XMLLib
