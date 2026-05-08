//
// Class: XML_EntityMapper
//
// Description: XML entity reference mapper.
//
// Dependencies: C++20 - Language standard features used.
//

#include "XML.hpp"
#include "XML_Core.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <vector>

namespace XML_Lib {

/// <summary>
/// Initialise entity mapping table with defaults.
/// </summary>
void  XML_EntityMapper::resetToDefault()
{
  entityMappings.clear();
  entityMappings.reserve(16);
  entityMappings.emplace("&amp;", XML_EntityMapping{ "&#x26;" });
  entityMappings.emplace("&quot;", XML_EntityMapping{ "&#x22;" });
  entityMappings.emplace("&apos;", XML_EntityMapping{ "&#x27;" });
  entityMappings.emplace("&lt;", XML_EntityMapping{ "&#x3C;" });
  entityMappings.emplace("&gt;", XML_EntityMapping{ "&#x3E;" });
}

/// <summary>
/// Take an entity reference mapping and make sure it is not recursive by calling
/// recurseOverEntityReference() repeatedly on any mapping found at the next level.
/// Until no more are found it or the entity reference repeats, in which case
/// it will cause and infinite loop when decoding and is an error.
/// </summary>
/// <param name="entityName">Entity mapping name.</param>
/// <param name="type">Entity mapping type.</param>
/// <param name="currentEntities">Currently, defined entities.</param>
void XML_EntityMapper::recurseOverEntityReference(const std::string_view &entityName,
  const Char type,
  std::set<std::string> &currentEntities)
{
  BufferSource entitySource { std::string(entityName) };
  while (entitySource.more()) {
    if (entitySource.current() == type) {
      std::string mappedEntityName = toUtf8(entitySource.current());
      entitySource.next();
      while (entitySource.more() && entitySource.current() != ';') {
        mappedEntityName += toUtf8(entitySource.current());
        entitySource.next();
      }
      mappedEntityName += toUtf8(entitySource.current());
      if (currentEntities.contains(mappedEntityName)) {
        throw SyntaxError("Entity '" + mappedEntityName + "' contains recursive definition which is not allowed.");
      }
      if (auto nextMappedName = getEntityMapping(mappedEntityName).getInternal(); !nextMappedName.empty()) {
        currentEntities.emplace(mappedEntityName);
        recurseOverEntityReference(nextMappedName, type, currentEntities);
        currentEntities.erase(mappedEntityName);
      }
    }
    entitySource.next();
  }
}

/// <summary>
/// Grab an entity reference mapping from an external file.
/// </summary>
/// <param name="fileName"></param>
/// <returns>String containing the contents of entity reference mapping file.</returns>
std::string XML_EntityMapper::getFileMappingContents(const std::string_view &fileName) const
{
  const std::string key{ fileName };
  if (const auto it = externalFileCache.find(key); it != externalFileCache.end()) {
    return it->second;
  }

  std::ifstream file(key, std::ios::binary);
  if (!file) {
    throw SyntaxError(std::string("Entity '") + key + "' source file does not exist.");
  }

  std::string content;
  file.seekg(0, std::ios::end);
  const auto size = file.tellg();
  if (size > 0) {
    content.reserve(static_cast<size_t>(size));
    file.seekg(0, std::ios::beg);
  }
  content.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
  externalFileCache.emplace(key, content);
  return externalFileCache.at(key);
}

/// <summary>
/// Get entity reference from the map.
/// </summary>
/// <param name="entityName">Entity reference name.</param>
/// <returns>Reference to entity mapping in the internal map.</returns>
XML_EntityMapping &XML_EntityMapper::getEntityMapping(const std::string_view &entityName)
{
  if (auto entity = entityMappings.find(entityName); entity != entityMappings.end()) {
    return entity->second;
  }
  const auto [it, inserted] = entityMappings.emplace(std::string(entityName), XML_EntityMapping());
  return it->second;
}

/// <summary>
/// Initialise entity mapping table with defaults.
/// </summary>
void XML_EntityMapper::reset()
{
  resetToDefault();
}

/// <summary>
/// Entity mapper constructor.
/// </summary>
XML_EntityMapper::XML_EntityMapper() { resetToDefault(); }

/// <summary>
/// Entity mapper destructor.
/// </summary>
XML_EntityMapper::~XML_EntityMapper() = default;

/// <summary>
/// Is an entry for an entity reference present in the map?
/// </summary>
/// <param name="entityName">.</param>
/// <returns></returns>
bool XML_EntityMapper::isPresent(const std::string_view &entityName) const
{
  return entityMappings.find(entityName) != entityMappings.end();
}

/// <summary>
/// Get mapping for an entity reference.
/// </summary>
/// <param name="entityReference">.</param>
/// <returns></returns>
XMLValue XML_EntityMapper::map(const XMLValue &entityReference)
{
  if (isPresent(entityReference.getUnparsed())) {
    std::string parsed{ entityReference.getUnparsed() };
    auto &entityMapping = getEntityMapping(entityReference.getUnparsed());
    if (!entityMapping.getInternal().empty()) {
      parsed = entityMapping.getInternal();
    } else if (entityMapping.isExternal()) {
      const auto &systemID = entityMapping.getExternal().getSystemID();
      if (std::filesystem::exists(systemID)) {
        parsed = getFileMappingContents(systemID);
      } else {
        throw SyntaxError("Entity '" + entityReference.getUnparsed() + "' source file '"
                          + systemID + "' does not exist.");
      }
    }
    return XMLValue{ entityReference.getUnparsed(), parsed };
  }
  throw SyntaxError("Entity '" + entityReference.getUnparsed() + "' does not exist.");
}
/// <summary>
/// Translate any entity reference to be found in a string.
/// </summary>
/// <param name="toTranslate">Source string containing references to be translated.</param>
/// <param name="type">Entity reference type.</param>
/// <returns>Translated string.</returns>
std::string XML_EntityMapper::translate(const std::string_view &toTranslate, const char type) const
{
  if (toTranslate.empty()) { return std::string{}; }

  std::vector<std::pair<std::string_view, const XML_EntityMapping *>> candidates;
  candidates.reserve(entityMappings.size());
  for (const auto &[key, mapping] : entityMappings) {
    if (!key.empty() && key[0] == type) {
      candidates.emplace_back(std::string_view(key), &mapping);
    }
  }
  if (candidates.empty()) { return std::string(toTranslate); }

  std::sort(candidates.begin(), candidates.end(), [](const auto &a, const auto &b) {
    return a.first.size() > b.first.size();
  });

  std::string translated;
  translated.reserve(toTranslate.size());
  size_t pos = 0;
  while (pos < toTranslate.size()) {
    if (toTranslate[pos] != type) {
      translated.push_back(toTranslate[pos]);
      ++pos;
      continue;
    }

    bool replaced = false;
    for (const auto &[key, mapping] : candidates) {
      if (toTranslate.size() - pos >= key.size() && toTranslate.substr(pos, key.size()) == key) {
        if (mapping->isInternal()) {
          translated.append(mapping->getInternal());
        } else if (mapping->isExternal()) {
          translated.append(getFileMappingContents(mapping->getExternal().getSystemID()));
        } else {
          translated.append(key);
        }
        pos += key.size();
        replaced = true;
        break;
      }
    }
    if (!replaced) {
      translated.push_back(toTranslate[pos]);
      ++pos;
    }
  }
  return translated;
}

/// <summary>
/// Determine entity type
/// </summary>
bool XML_EntityMapper::isInternal(const std::string_view &entityName) { return getEntityMapping(entityName).isInternal(); }
bool XML_EntityMapper::isExternal(const std::string_view &entityName) { return getEntityMapping(entityName).isExternal(); }
bool XML_EntityMapper::isNotation(const std::string_view &entityName) { return getEntityMapping(entityName).isNotation(); }

/// <summary>
/// Get entity mapping values.
/// </summary>
const std::string &XML_EntityMapper::getInternal(const std::string_view &entityName)
{
  if (const auto entity = getEntityMapping(entityName); entity.isInternal()) {
    return getEntityMapping(entityName).getInternal();
  }
  throw Error(std::string("Internal entity reference not found for '").append(entityName)+"'.");
}
const std::string &XML_EntityMapper::getNotation(const std::string_view &entityName)
{
  if (const auto entity = getEntityMapping(entityName); entity.isNotation()) {
    return getEntityMapping(entityName).getNotation();
  }
  throw Error(std::string("Notation entity reference not found for '").append(entityName)+"'.");
}
const XMLExternalReference &XML_EntityMapper::getExternal(const std::string_view &entityName)
{
  if (const auto entity = getEntityMapping(entityName); entity.isExternal()) {
    return getEntityMapping(entityName).getExternal();
  }
  throw Error(std::string("External entity reference not found for '").append(entityName)+"'.");
}

/// <summary>
/// Set entity mapping values.
/// </summary>
void XML_EntityMapper::setInternal(const std::string_view &entityName, const std::string_view &internal)
{
  getEntityMapping(entityName).setInternal(internal);
}
void XML_EntityMapper::setNotation(const std::string_view &entityName, const std::string_view &notation)
{
  getEntityMapping(entityName).setNotation(notation);
}
void XML_EntityMapper::setExternal(const std::string_view &entityName, const XMLExternalReference &external)
{
  getEntityMapping(entityName).setExternal(external);
}

/// <summary>
/// Take an entity reference string, check whether it contains any infinitely
/// recursive definition and throw an exception if so. This is done by
/// recursively parsing any entities found in an entity mapping and adding it to
/// a current set of used entities; throwing an exception if it is already
/// being used.
/// </summary>
void XML_EntityMapper::checkRecursiveEntity(const std::string_view &entityName,
  const std::string &expanded,
  std::set<std::string> &currentEntities)
{
  BufferSource expandedSource{ expanded };
  while (expandedSource.more()) {
    if (expandedSource.current() == entityName[0]) {
      std::string mappedEntityName{ toUtf8(expandedSource.current()) };
      expandedSource.next();
      while (expandedSource.more() && expandedSource.current() != ';') {
        mappedEntityName += toUtf8(expandedSource.current());
        expandedSource.next();
      }
      mappedEntityName += toUtf8(expandedSource.current());
      if (currentEntities.contains(mappedEntityName)) {
        throw SyntaxError("Entity '" + mappedEntityName + "' contains recursive definition which is not allowed.");
      }
      if (auto nextMappedName = getEntityMapping(mappedEntityName).getInternal(); !nextMappedName.empty()) {
        currentEntities.emplace(mappedEntityName);
        recurseOverEntityReference(nextMappedName, entityName[0], currentEntities);
        currentEntities.erase(mappedEntityName);
      }
    }
    expandedSource.next();
  }
}

void XML_EntityMapper::checkForRecursion()
{
  std::set<std::string> currentEntities{};
  for (const auto &[fst, snd] : entityMappings) {
    recurseOverEntityReference(fst, fst[0], currentEntities);
  }
}
}// namespace XML_Lib
