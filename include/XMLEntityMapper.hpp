#pragma once
//
// C++ STL
//
#include <string>
#include <map>
#include <filesystem>
#include <set>
// =========
// NAMESPACE
// =========
namespace XMLLib
{
    //
    // XML Entity mapper
    //
    class XMLEntityMapper : public IXMLEntityMapper
    {
    public:
        XMLEntityMapper()
        {
            m_enityMappings["&amp;"].internal = "&#x26;";
            m_enityMappings["&quot;"].internal = "&#x22;";
            m_enityMappings["&apos;"].internal = "&#x27;";
            m_enityMappings["&lt;"].internal = "&#x3C;";
            m_enityMappings["&gt;"].internal = "&#x3E;";
        }
        void add(const std::string &entityName, XMLEntityMapping &entityMapping) override
        {
            m_enityMappings[entityName] = entityMapping;
        }
        XMLEntityMapping &get(const std::string &entityName) override
        {
            return (m_enityMappings[entityName]);
        }
        void remove(const std::string &entityName) override
        {
            m_enityMappings.erase(entityName);
        }
        bool isPresent(const std::string &entityName) const override
        {
            return (m_enityMappings.count(entityName) != 0);
        }
        std::map<std::string, XMLEntityMapping> &getList() override
        {
            return (m_enityMappings);
        }
        void map(XMLValue &entityReference) override
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
        std::string translate(const std::string &toTranslate, char type = '%') const override
        {
            std::string translated = toTranslate;
            bool matchFound;
            do
            {
                matchFound = false;
                for (auto entity : m_enityMappings)
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
        /// definition and throw an exception if so. This is done by recurively parsing any entities
        /// found in a entiity mapping and adding it to a current stack of used entities; throwing an
        /// exception if it is already being used (recursive).
        /// </summary>
        void recursive(const std::string &entityReference, ISource::Char type, std::set<std::string> currentEntities) override
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
    private:
        std::map<std::string, XMLEntityMapping> m_enityMappings;
    };
} // namespace XMLLib
