//
// Class: XML_EntityMapper
//
// Description: XML entity reference mapper.
//
// Dependencies: C++20 - Language standard features used.
//

#include "XML_Core.hpp"
#include "entity/XML_EntityMapperHelpers.hpp"
#include "interface/IEntityResolver.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <vector>

namespace XML_Lib {

/// @brief
/// Initialise entity mapping table with defaults.

void  XML_EntityMapper::resetToDefault()
{
  entityMappings.clear();  invalidateTranslationCache();  entityMappings.reserve(16);
  entityMappings.emplace("&amp;", XML_EntityMapping{ "&#x26;" });
  entityMappings.emplace("&quot;", XML_EntityMapping{ "&#x22;" });
  entityMappings.emplace("&apos;", XML_EntityMapping{ "&#x27;" });
  entityMappings.emplace("&lt;", XML_EntityMapping{ "&#x3C;" });
  entityMappings.emplace("&gt;", XML_EntityMapping{ "&#x3E;" });
}

/// @brief
/// Take an entity reference mapping and make sure it is not recursive by calling
/// recurseOverEntityReference() repeatedly on any mapping found at the next level.
/// Until no more are found it or the entity reference repeats, in which case
/// it will cause and infinite loop when decoding and is an error.

/// @param entityName Entity mapping name.
/// @param type Entity mapping type.
/// @param currentEntities Currently, defined entities.
void XML_EntityMapper::recurseOverEntityReference(const std::string_view &entityName,
  const Char type,
  std::set<std::string> &currentEntities)
{
  static constexpr std::size_t kMaxRecursiveEntityResolutionDepth{ 512 };
  if (currentEntities.size() > kMaxRecursiveEntityResolutionDepth) {
    XML_LIB_THROW(SyntaxError("Entity resolution depth exceeds maximum allowed."));
  }

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
        XML_LIB_THROW(SyntaxError("Entity '" + mappedEntityName + "' contains recursive definition which is not allowed."));
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

static constexpr std::size_t kMaxExternalEntityFileSize{ 5ULL * 1024ULL * 1024ULL }; // 5 MB

/// @brief
/// Grab an entity reference mapping from an external file.

/// @param fileName 
/// @return String containing the contents of entity reference mapping file.
std::string XML_EntityMapper::getFileMappingContents(const std::string_view &fileName) const
{
  return getCachedFileMapping(fileName);
}

/// @brief
/// Implementation of XML_EntityMapper::getCachedFileMapping.

std::string XML_EntityMapper::getCachedFileMapping(const std::string_view &fileName) const
{
  if (fileName.empty()) {
    XML_LIB_THROW(SyntaxError("External entity file name is empty."));
  }

  const std::string key{ fileName };
  if (const auto it = externalFileCache.find(key); it != externalFileCache.end()) {
    return it->second;
  }

  std::ifstream file(key, std::ios::binary);
  if (!file) {
    XML_LIB_THROW(SyntaxError(std::string("Entity '") + key + "' source file does not exist."));
  }

  file.seekg(0, std::ios::end);
  const auto size = file.tellg();
  if (size < 0) {
    XML_LIB_THROW(SyntaxError(std::string("Entity '") + key + "' source file cannot be read."));
  }
  if (static_cast<std::size_t>(size) > kMaxExternalEntityFileSize) {
    XML_LIB_THROW(SyntaxError("External entity file size exceeds maximum allowed."));
  }

  std::string content;
  if (size > 0) {
    content.reserve(static_cast<size_t>(size));
    file.seekg(0, std::ios::beg);
  }
  content.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
  externalFileCache.emplace(key, content);
  return externalFileCache.at(key);
}

XML_EntityMapping &XML_EntityMapper::getEntityMapping(const std::string_view &entityName)
{
  return ensureEntityMapping(entityMappings, entityName);
}

/// @brief
/// Initialise entity mapping table with defaults.

void XML_EntityMapper::reset()
{
  resetToDefault();
}

/// @brief
/// Entity mapper constructor.

XML_EntityMapper::XML_EntityMapper() { resetToDefault(); }

/// @brief
/// Entity mapper destructor.

XML_EntityMapper::~XML_EntityMapper() noexcept = default;

/// @brief
/// Implementation of XML_EntityMapper::invalidateTranslationCache.

void XML_EntityMapper::invalidateTranslationCache() const
{
  translationCacheValid = false;
  translationCandidates.clear();
}

const std::vector<std::pair<std::string_view, const XML_EntityMapping *>> &XML_EntityMapper::getTranslationCandidates(char type) const
{
  if (!translationCacheValid || translationType != type) {
    translationCandidates = buildTranslationCandidates(entityMappings, type);
    translationType = type;
    translationCacheValid = true;
  }
  return translationCandidates;
}

/// @brief
/// Is an entry for an entity reference present in the map?

/// @param entityName .
/// @return 
bool XML_EntityMapper::isPresent(const std::string_view &entityName) const
{
  return findEntityMapping(entityMappings, entityName) != nullptr;
}

/// @brief
/// Implementation of XML_EntityMapper::setExternalEntityPolicy.

void XML_EntityMapper::setExternalEntityPolicy(bool allowExternal, IEntityResolver *resolver)
{
  allowExternalEntities = allowExternal;
  entityResolver = resolver;
}

/// @brief
/// Implementation of XML_EntityMapper::map.

XMLValue XML_EntityMapper::map(const XMLValue &entityReference)
{
  if (const auto *entityMapping = findEntityMapping(entityMappings, entityReference.getUnparsed())) {
    std::string parsed{ entityReference.getUnparsed() };
    if (!entityMapping->getInternal().empty()) {
      parsed = entityMapping->getInternal();
    } else if (entityMapping->isExternal()) {
      const auto &extRef = entityMapping->getExternal();
      const auto &systemID = extRef.getSystemID();
      if (!allowExternalEntities && entityResolver == nullptr) {
        XML_LIB_THROW(SyntaxError("External entity resolution is disabled. "
          "Set ParseOptions::allowExternalEntities = true or supply an IEntityResolver."));
      }
      bool resolved = false;
      if (entityResolver != nullptr) {
        const std::string publicID = extRef.isPublic() ? extRef.getPublicID() : "";
        if (auto result = entityResolver->resolve(systemID, publicID)) {
          parsed = std::move(*result);
          resolved = true;
        }
      }
      if (!resolved) {
        if (std::filesystem::exists(systemID)) {
          parsed = getFileMappingContents(systemID);
        } else {
          XML_LIB_THROW(SyntaxError("Entity '" + entityReference.getUnparsed() + "' source file '"
                            + systemID + "' does not exist."));
        }
      }
    }
    return XMLValue{ entityReference.getUnparsed(), parsed };
  }
  XML_LIB_THROW(SyntaxError("Entity '" + entityReference.getUnparsed() + "' does not exist."));
}
/// @brief
/// Translate any entity reference to be found in a string.

/// @param toTranslate Source string containing references to be translated.
/// @param type Entity reference type.
/// @return Translated string.
std::string XML_EntityMapper::translate(const std::string_view &toTranslate, const char type) const
{
  if (toTranslate.empty()) { return std::string{}; }

  const auto &candidates = getTranslationCandidates(type);
  if (candidates.empty()) { return std::string(toTranslate); }

  std::string translated;
  translated.reserve(toTranslate.size());
  size_t pos = 0;
  while (pos < toTranslate.size()) {
    if (toTranslate[pos] != type) {
      translated.push_back(toTranslate[pos]);
      ++pos;
      continue;
    }

    if (const auto match = matchEntityPrefix(candidates, toTranslate, pos); match.has_value()) {
      const auto &[key, mapping] = *match;
      if (mapping->isInternal()) {
        translated.append(mapping->getInternal());
      } else if (mapping->isExternal()) {
        translated.append(getFileMappingContents(mapping->getExternal().getSystemID()));
      } else {
        translated.append(key);
      }
      pos += key.size();
      continue;
    }

    translated.push_back(toTranslate[pos]);
    ++pos;
  }
  return translated;
}

/// @brief
/// Determine entity type

bool XML_EntityMapper::isInternal(const std::string_view &entityName)
{
  if (const auto *entity = findEntityMapping(entityMappings, entityName)) { return entity->isInternal(); }
  return false;
}

/// @brief
/// Implementation of XML_EntityMapper::isExternal.

bool XML_EntityMapper::isExternal(const std::string_view &entityName)
{
  if (const auto *entity = findEntityMapping(entityMappings, entityName)) { return entity->isExternal(); }
  return false;
}

/// @brief
/// Implementation of XML_EntityMapper::isNotation.

bool XML_EntityMapper::isNotation(const std::string_view &entityName)
{
  if (const auto *entity = findEntityMapping(entityMappings, entityName)) { return entity->isNotation(); }
  return false;
}

/// @brief
/// Get entity mapping values.

const std::string &XML_EntityMapper::getInternal(const std::string_view &entityName)
{
  if (const auto *entity = findEntityMapping(entityMappings, entityName); entity && entity->isInternal()) {
    return entity->getInternal();
  }
  XML_LIB_THROW(Error(std::string("Internal entity reference not found for '").append(entityName)+"'."));
}
const std::string &XML_EntityMapper::getNotation(const std::string_view &entityName)
{
  if (const auto *entity = findEntityMapping(entityMappings, entityName); entity && entity->isNotation()) {
    return entity->getNotation();
  }
  XML_LIB_THROW(Error(std::string("Notation entity reference not found for '").append(entityName)+"'."));
}
const XMLExternalReference &XML_EntityMapper::getExternal(const std::string_view &entityName)
{
  if (const auto *entity = findEntityMapping(entityMappings, entityName); entity && entity->isExternal()) {
    return entity->getExternal();
  }
  XML_LIB_THROW(Error(std::string("External entity reference not found for '").append(entityName)+"'."));
}

/// @brief
/// Set entity mapping values.

void XML_EntityMapper::setInternal(const std::string_view &entityName, const std::string_view &internal)
{
  getEntityMapping(entityName).setInternal(internal);
  invalidateTranslationCache();
}

/// @brief
/// Implementation of XML_EntityMapper::setNotation.

void XML_EntityMapper::setNotation(const std::string_view &entityName, const std::string_view &notation)
{
  getEntityMapping(entityName).setNotation(notation);
  invalidateTranslationCache();
}

/// @brief
/// Implementation of XML_EntityMapper::setExternal.

void XML_EntityMapper::setExternal(const std::string_view &entityName, const XMLExternalReference &external)
{
  getEntityMapping(entityName).setExternal(external);
  invalidateTranslationCache();
}

/// @brief
/// Take an entity reference string, check whether it contains any infinitely
/// recursive definition and throw an exception if so. This is done by
/// recursively parsing any entities found in an entity mapping and adding it to
/// a current set of used entities; throwing an exception if it is already
/// being used.

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
        XML_LIB_THROW(SyntaxError("Entity '" + mappedEntityName + "' contains recursive definition which is not allowed."));
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

/// @brief
/// Implementation of XML_EntityMapper::checkForRecursion.

void XML_EntityMapper::checkForRecursion()
{
  std::set<std::string> currentEntities{};
  for (const auto &[fst, snd] : entityMappings) {
    recurseOverEntityReference(fst, fst[0], currentEntities);
  }
}
}// namespace XML_Lib