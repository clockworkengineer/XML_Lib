#pragma once
//
// C++ STL
//
#include <string>
#include <stdexcept>
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
    class IXMLEntityMapper;
    class DTD;
    struct XMLNodeElement;
    struct XMLNodeProlog;
    struct XMLNodeDeclaration;
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
        DTD &dtd();
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
