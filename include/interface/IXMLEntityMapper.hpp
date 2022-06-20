#pragma once
//
// C++ STL
//
#include <set>
#include <map>

// =========
// NAMESPACE
// =========
namespace XMLLib
{
    // ===================================================
    // Forward declarations for interfaces/classes/structs
    // ===================================================
    struct XMLEntityMapping;
    struct XMLValue;
    // ===========================
    // XML Entity mapper interface
    // ===========================
    class IXMLEntityMapper
    {
    public:
        // ========================
        // Constructors/destructors
        // ========================
        IXMLEntityMapper() = default;
        IXMLEntityMapper(const IXMLEntityMapper &other) = delete;
        IXMLEntityMapper &operator=(const IXMLEntityMapper &other) = delete;
        IXMLEntityMapper(IXMLEntityMapper &&other) = delete;
        IXMLEntityMapper &operator=(IXMLEntityMapper &&other) = delete;
        virtual ~IXMLEntityMapper() = default;
        // ============================
        // Add entity reference mapping
        // ============================
        virtual void add(const std::string &entityName, const XMLEntityMapping &entityMapping) = 0;
        // ==================================
        // Get entity reference mapping entry
        // ==================================
        virtual XMLEntityMapping &get(const std::string &entityName) = 0;
        // ===============================
        // Remove entity reference mapping
        // ===============================
        virtual void remove(const std::string &entityName) = 0;
        // ===========================================
        // Is entity reference mapping entry present ?
        // ===========================================
        [[nodiscard]] virtual bool isPresent(const std::string &entityName) const = 0;
        // =================================
        // Get entity reference mapping list
        // =================================
        virtual std::map<std::string, XMLEntityMapping> &getList() = 0;
        // ===================================
        // Get mapping for an entity reference
        // ===================================
        virtual XMLValue map(const XMLValue &entityReference) = 0;
        // ==========================================
        // Translate any entity reference in a string
        // ==========================================
        [[nodiscard]] virtual std::string translate(const std::string &toTranslate, char type = '%') const = 0;
        // ==============================================
        // Check for a recursive entity reference mapping
        // ==============================================
        virtual void checkForRecursion(const std::string &entityName) = 0;
    };
    //
} // namespace XMLLib