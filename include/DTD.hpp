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
    struct XMLNodeDTD;
    // ================
    // CLASS DEFINITION
    // ================
    class DTD
    {
    public:
        // ==========================
        // PUBLIC TYPES AND CONSTANTS
        // ==========================
        // ======================
        // CONSTRUCTOR/DESTRUCTOR
        // ======================
        explicit DTD(XMLNodeDTD &xmlNodeDTD);
        DTD(const DTD &other) = delete;
        DTD &operator=(const DTD &other) = delete;
        DTD(DTD &&other) = delete;
        DTD &operator=(DTD &&other) = delete;
        ~DTD();
        // ==============
        // PUBLIC METHODS
        // ==============
        void parse(ISource &source);
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
