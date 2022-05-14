//
// Class: XMLEntityMapper
//
// Description: XML entity mapper.
//
// Dependencies:   C20++ - Language standard features used.
//
// =================
// CLASS DEFINITIONS
// =================
#include "XML.hpp"
#include "DTD.hpp"
#include "XMLEntityMapper.hpp"
// ====================
// CLASS IMPLEMENTATION
// ====================
// =========
// NAMESPACE
// =========
namespace XMLLib
{
    // ===========================
    // PRIVATE TYPES AND CONSTANTS
    // ===========================
    // ==========================
    // PUBLIC TYPES AND CONSTANTS
    // ==========================
    // ========================
    // PRIVATE STATIC VARIABLES
    // ========================
    // =======================
    // PUBLIC STATIC VARIABLES
    // =======================
    // ===============
    // PRIVATE METHODS
    // ===============
    // ==============
    // PUBLIC METHODS
    // ==============
    XMLEntityMapper::XMLEntityMapper()
    {
        m_entityMappings["&amp;"].internal = "&#x26;";
        m_entityMappings["&quot;"].internal = "&#x22;";
        m_entityMappings["&apos;"].internal = "&#x27;";
        m_entityMappings["&lt;"].internal = "&#x3C;";
        m_entityMappings["&gt;"].internal = "&#x3E;";
    }
    XMLEntityMapper::~XMLEntityMapper()
    {
    }
    void XMLEntityMapper::add(const std::string &entityName, XMLEntityMapping &entityMapping)
    {
        m_entityMappings[entityName] = entityMapping;
    }
    XMLEntityMapping &XMLEntityMapper::get(const std::string &entityName)
    {
        return (m_entityMappings[entityName]);
    }
    void XMLEntityMapper::remove(const std::string &entityName)
    {
        m_entityMappings.erase(entityName);
    }
    bool XMLEntityMapper::isPresent(const std::string &entityName) const
    {
        return (m_entityMappings.count(entityName) != 0);
    }
    std::map<std::string, XMLEntityMapping> &XMLEntityMapper::getList()
    {
        return (m_entityMappings);
    }
    void XMLEntityMapper::map(XMLValue &entityReference)
    {
        if (isPresent(entityReference.unparsed))
        {
            if (!get(entityReference.unparsed).internal.empty())
            {
                entityReference.parsed = get(entityReference.unparsed).internal;
            }
            else
            {
                if (std::filesystem::exists(get(entityReference.unparsed).external.systemID))
                {
                    FileSource entitySource(get(entityReference.unparsed).external.systemID);
                    entityReference.parsed = "";
                    while (entitySource.more())
                    {
                        entityReference.parsed += entitySource.current_to_bytes();
                        entitySource.next();
                    }
                }
                else
                {
                    throw SyntaxError("Entity '" + entityReference.unparsed + "' source file '" + get(entityReference.unparsed).external.systemID + "' does not exist.");
                }
            }
        }
    }
    std::string XMLEntityMapper::translate(const std::string &toTranslate, char type) const
    {
        std::string translated = toTranslate;
        bool matchFound;
        do
        {
            matchFound = false;
            for (auto entity : m_entityMappings)
            {
                if (entity.first[0] == type)
                {
                    size_t pos = translated.find(entity.first);
                    if (pos != std::string::npos)
                    {
                        translated.replace(pos, entity.first.length(), entity.second.internal);
                        matchFound = true;
                    }
                }
            }
        } while (matchFound);
        return (translated);
    }
    /// <summary>
    /// Take an entity reference string, check whether it contains any infinitely recursive
    /// definition and throw an exception if so. This is done by recursively parsing any entities
    /// found in a entity mapping and adding it to a current stack of used entities; throwing an
    /// exception if it is already being used (recursive).
    /// </summary>
    void XMLEntityMapper::recursive(const std::string &entityReference, ISource::Char type, std::set<std::string> currentEntities)
    {
        BufferSource entitySource(entityReference);
        while (entitySource.more())
        {
            if (entitySource.current() == type)
            {
                std::string mappedEntityName = entitySource.current_to_bytes();
                entitySource.next();
                while (entitySource.more() && entitySource.current() != ';')
                {
                    mappedEntityName += entitySource.current_to_bytes();
                    entitySource.next();
                }
                mappedEntityName += entitySource.current_to_bytes();
                if (currentEntities.find(mappedEntityName) != currentEntities.end())
                {
                    throw SyntaxError("Entity '" + mappedEntityName + "' contains recursive definition which is not allowed.");
                }
                if (!get(mappedEntityName).internal.empty())
                {
                    currentEntities.emplace(mappedEntityName);
                    recursive(get(mappedEntityName).internal, type, currentEntities);
                    currentEntities.erase(mappedEntityName);
                }
            }
            entitySource.next();
        }
    }
} // namespace XMLLib
