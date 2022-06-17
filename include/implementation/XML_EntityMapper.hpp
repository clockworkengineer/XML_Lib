#pragma once
//
// C++ STL
//
#include <string>
#include <map>
#include <filesystem>
#include <set>
//
// XML
//
#include "XML_Sources.hpp"
//
// XML Entity Mapper interface
//
#include "IXMLEntityMapper.hpp"
// =========
// NAMESPACE
// =========
namespace XMLLib
{
    // ================
    // CLASS DEFINITION
    // ================
    class XML_EntityMapper : public IXMLEntityMapper
    {
    public:
        // ==========================
        // PUBLIC TYPES AND CONSTANTS
        // ==========================
        // ======================
        // CONSTRUCTOR/DESTRUCTOR
        // ======================
        XML_EntityMapper();
        XML_EntityMapper(const XML_EntityMapper &other) = delete;
        XML_EntityMapper &operator=(const XML_EntityMapper &other) = delete;
        XML_EntityMapper(XML_EntityMapper &&other) = delete;
        XML_EntityMapper &operator=(XML_EntityMapper &&other) = delete;
        ~XML_EntityMapper() override;
        // ==============
        // PUBLIC METHODS
        // ==============
        void add(const std::string &entityName, const XMLEntityMapping &entityMapping) override;
        XMLEntityMapping &get(const std::string &entityName) override;
        void remove(const std::string &entityName) override;
        [[nodiscard]] bool isPresent(const std::string &entityName) const override;
        std::map<std::string, XMLEntityMapping> &getList() override;
        XMLValue map(const XMLValue &entityReference) override;
        [[nodiscard]] std::string translate(const std::string &toTranslate, char type = '%') const override;
        void recursive(const std::string &entityReference, ISource::Char type, std::set<std::string> currentEntities) override;
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
        std::map<std::string, XMLEntityMapping> m_entityMappings;
    };
} // namespace XMLLib
