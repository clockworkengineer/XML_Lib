//
// Class: XML_EntityMapper
//
// Description: XML entity mapper.
//
// Dependencies:   C20++ - Language standard features used.
//
// =================
// CLASS DEFINITIONS
// =================
#include "XML_EntityMapper.hpp"
// ====================
// CLASS IMPLEMENTATION
// ====================
// =========
// NAMESPACE
// =========
namespace XML_Lib
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
    /// <summary>
    /// Take an entity reference mapping and make sure it is not recursive by calling 
    /// recurseOverEntityReference() repeatedly on any mapping found at the next level
    /// until no more more are found it or the entity reference repeats in which case 
    /// it will cause and infinite loop when decoding and is an error.
    /// </summary>
    /// <param name="entityReference"></param>
    /// <param name="type"></param>
    void XML_EntityMapper::recurseOverEntityReference(const std::string &entityReference, ISource::Char type)
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
                if (m_currentEntities.count(mappedEntityName) > 0)
                {
                    throw SyntaxError("Entity '" + mappedEntityName + "' contains recursive definition which is not allowed.");
                }
                auto nextMappedName = get(mappedEntityName).internal;
                if (!nextMappedName.empty())
                {
                    m_currentEntities.emplace(mappedEntityName);
                    recurseOverEntityReference(nextMappedName, type);
                    m_currentEntities.erase(mappedEntityName);
                }
            }
            entitySource.next();
        }
    }
    /// <summary>
    /// Grab an entity reference mapping from an external file.
    /// </summary>
    /// <param name="fileName"></param>
    /// <returns></returns>
    std::string XML_EntityMapper::getFileMappingContents(const std::string &fileName)
    {
        std::string content;
        FileSource entitySource{fileName};
        while (entitySource.more())
        {
            content += entitySource.current_to_bytes();
            entitySource.next();
        }
        return (content);
    }
    // ==============
    // PUBLIC METHODS
    // ==============
    /// <summary>
    /// Entity mapper constructor.
    /// Initialise entity mapping table with defaults.
    /// </summary>
    XML_EntityMapper::XML_EntityMapper()
    {
        add("&amp;", XMLEntityMapping{"&#x26;"});
        add("&quot;", XMLEntityMapping{"&#x22;"});
        add("&apos;", XMLEntityMapping{"&#x27;"});
        add("&lt;", XMLEntityMapping{"&#x3C;"});
        add("&gt;", XMLEntityMapping{"&#x3E;"});
    }
    /// <summary>
    /// Entity mapper destructor.
    /// </summary>
    XML_EntityMapper::~XML_EntityMapper()
    {
    }
    /// <summary>
    /// Place entity reference into map.
    /// </summary>
    /// <param name="entityName"></param>
    /// <param name="entityMapping"></param>
    /// <returns></returns>
    void XML_EntityMapper::add(const std::string &entityName, const XMLEntityMapping &entityMapping)
    {
        m_entityMappings.emplace(std::make_pair(entityName, entityMapping));
    }
    /// <summary>
    /// Get entity reference from map.
    /// </summary>
    /// <param name="entityName"></param>
    /// <returns></returns>
    XMLEntityMapping &XML_EntityMapper::get(const std::string &entityName)
    {
        if (!isPresent(entityName))
        {
            add(entityName, XMLEntityMapping{""});
        }
        auto entity = m_entityMappings.find(entityName);
        if (entity != m_entityMappings.end())
        {
            return (entity->second);
        }
        throw XML_Lib::Error("Could not find entity reference in map.");
    }
    /// <summary>
    /// Remove entity reference from map.
    /// </summary>
    /// <param name="entityName"></param>
    void XML_EntityMapper::remove(const std::string &entityName)
    {
        m_entityMappings.erase(entityName);
    }
    /// <summary>
    /// Is an entry for an entity reference present in map.
    /// </summary>
    /// <param name="entityName"></param>
    /// <returns></returns>
    bool XML_EntityMapper::isPresent(const std::string &entityName) const
    {
        return (m_entityMappings.count(entityName) != 0);
    }
    /// <summary>
    /// Get a traversal list of entity reference map.
    /// </summary>
    /// <returns></returns>
    std::map<std::string, XMLEntityMapping> &XML_EntityMapper::getList()
    {
        return (m_entityMappings);
    }
    /// <summary>
    /// Get mapping for an entity reference.
    /// </summary>
    /// <param name="entityReference"></param>
    /// <returns></returns>
    XMLValue XML_EntityMapper::map(const XMLValue &entityReference)
    {
        std::string parsed{entityReference.unparsed};
        if (isPresent(entityReference.unparsed))
        {
            // Internal so from memory.
            auto entityMapping = get(entityReference.unparsed);
            if (!entityMapping.internal.empty())
            {
                parsed = entityMapping.internal;
            }
            else
            // External so from a file.
            // *** TODO Need to add support for external other than file ***
            {
                if (std::filesystem::exists(entityMapping.external.systemID))
                {
                    parsed = getFileMappingContents(entityMapping.external.systemID);
                }
                else
                {
                    throw SyntaxError("Entity '" + entityReference.unparsed + "' source file '" + entityMapping.external.systemID + "' does not exist.");
                }
            }

            return (XMLValue{entityReference.unparsed, parsed});
        }
        return (entityReference);
    }
    /// <summary>
    /// Translate any entity reference found in a string.
    /// </summary>
    /// <param name="toTranslate"></param>
    /// <param name="type"></param>
    /// <returns></returns>
    std::string XML_EntityMapper::translate(const std::string &toTranslate, char type) const
    {
        std::string translated = toTranslate;
        bool matchFound;
        do
        {
            matchFound = false;
            for (auto &entity : m_entityMappings)
            {
                if (entity.first[0] == type)
                {
                    if (size_t position = translated.find(entity.first); position != std::string::npos)
                    {
                        translated.replace(position, entity.first.length(), entity.second.internal);
                        matchFound = true;
                        break;
                    }
                }
            }
        } while (matchFound);
        return (translated);
    }
    /// <summary>
    /// Take an entity reference string, check whether it contains any infinitely
    /// recursive definition and throw an exception if so. This is done by
    /// recursively parsing any entities found in a entity mapping and adding it to
    /// a current set of used entities; throwing an exception if it is already
    /// being used.
    /// </summary>
    /// <param name="entityReference"></param>
    /// <param name="type"></param>
    void XML_EntityMapper::checkForRecursion(const std::string &entityReference)
    {
        if (!m_currentEntities.empty())
        {
            throw Error("EntityMapper internal error.");
        }
        recurseOverEntityReference(entityReference, entityReference[0]);
    }
} // namespace XML_Lib
