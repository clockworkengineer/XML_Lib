#pragma once
//
// C++ STL
//
#include <string>
#include <map>
#include <filesystem>
#include <set>
//
// XML errors
//
#include "XML_Errors.hpp"
//
// XML sources
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
        // ============
        // CONSTRUCTORS
        // ============
        XML_EntityMapper();
        // ==========
        // DESTRUCTOR
        // ==========
        virtual ~XML_EntityMapper();
        // ==============
        // PUBLIC METHODS
        // ==============
        virtual void add(const std::string &entityName, XMLEntityMapping &entityMapping) override;
        virtual XMLEntityMapping &get(const std::string &entityName) override;
        virtual void remove(const std::string &entityName) override;
        virtual bool isPresent(const std::string &entityName) const override;
        virtual std::map<std::string, XMLEntityMapping> &getList() override;
        virtual void map(XMLValue &entityReference) override;
        virtual std::string translate(const std::string &toTranslate, char type = '%') const override;
        virtual void recursive(const std::string &entityReference, ISource::Char type, std::set<std::string> currentEntities) override;
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
