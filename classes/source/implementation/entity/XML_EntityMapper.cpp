//
// Class: XML_EntityMapper & Sub-components
//
// Description: XML entity reference mapper with SOLID modular sub-components.
//
// Dependencies: C++20 - Language standard features used.
//

#include "XML_Core.hpp"
#include "entity/XML_EntityMapper.hpp"
#include "entity/XML_EntityMapperHelpers.hpp"
#include "interface/IEntityResolver.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <vector>

namespace XML_Lib {

static constexpr std::size_t kMaxExternalEntityFileSize{ 5ULL * 1024ULL * 1024ULL }; // 5 MB

// ==========================================
// EntityStorage Implementation
// ==========================================

EntityStorage::EntityStorage()
{
  resetToDefault();
}

void EntityStorage::resetToDefault()
{
  entityMappings.clear();
  entityMappings.reserve(16);
  entityMappings.emplace("&amp;", XML_EntityMapping{ "&#x26;" });
  entityMappings.emplace("&quot;", XML_EntityMapping{ "&#x22;" });
  entityMappings.emplace("&apos;", XML_EntityMapping{ "&#x27;" });
  entityMappings.emplace("&lt;", XML_EntityMapping{ "&#x3C;" });
  entityMappings.emplace("&gt;", XML_EntityMapping{ "&#x3E;" });
}

bool EntityStorage::isPresent(const std::string_view &entityName) const
{
  return findEntityMapping(entityMappings, entityName) != nullptr;
}

bool EntityStorage::isInternal(const std::string_view &entityName) const
{
  if (const auto *entity = findEntityMapping(entityMappings, entityName)) { return entity->isInternal(); }
  return false;
}

bool EntityStorage::isExternal(const std::string_view &entityName) const
{
  if (const auto *entity = findEntityMapping(entityMappings, entityName)) { return entity->isExternal(); }
  return false;
}

bool EntityStorage::isNotation(const std::string_view &entityName) const
{
  if (const auto *entity = findEntityMapping(entityMappings, entityName)) { return entity->isNotation(); }
  return false;
}

const std::string &EntityStorage::getInternal(const std::string_view &entityName) const
{
  if (const auto *entity = findEntityMapping(entityMappings, entityName); entity && entity->isInternal()) {
    return entity->getInternal();
  }
  XML_LIB_THROW(XML_EntityMapper::Error(std::string("Internal entity reference not found for '").append(entityName) + "'."));
}

const std::string &EntityStorage::getNotation(const std::string_view &entityName) const
{
  if (const auto *entity = findEntityMapping(entityMappings, entityName); entity && entity->isNotation()) {
    return entity->getNotation();
  }
  XML_LIB_THROW(XML_EntityMapper::Error(std::string("Notation entity reference not found for '").append(entityName) + "'."));
}

const XMLExternalReference &EntityStorage::getExternal(const std::string_view &entityName) const
{
  if (const auto *entity = findEntityMapping(entityMappings, entityName); entity && entity->isExternal()) {
    return entity->getExternal();
  }
  XML_LIB_THROW(XML_EntityMapper::Error(std::string("External entity reference not found for '").append(entityName) + "'."));
}

void EntityStorage::setInternal(const std::string_view &entityName, const std::string_view &internal)
{
  getEntityMapping(entityName).setInternal(internal);
}

void EntityStorage::setNotation(const std::string_view &entityName, const std::string_view &notation)
{
  getEntityMapping(entityName).setNotation(notation);
}

void EntityStorage::setExternal(const std::string_view &entityName, const XMLExternalReference &external)
{
  getEntityMapping(entityName).setExternal(external);
}

XML_EntityMapping &EntityStorage::getEntityMapping(const std::string_view &entityName)
{
  return ensureEntityMapping(entityMappings, entityName);
}

// ==========================================
// XXESecurityPolicy Implementation
// ==========================================

void XXESecurityPolicy::setExternalEntityPolicy(bool allowExternal, IEntityResolver *resolver)
{
  allowExternalEntities = allowExternal;
  entityResolver = resolver;
}

std::string XXESecurityPolicy::getCachedFileMapping(const std::string_view &fileName) const
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

// ==========================================
// EntityRecursionChecker Implementation
// ==========================================

void EntityRecursionChecker::recurseOverEntityReference(const std::string_view &entityName,
  const Char type,
  std::set<std::string> &currentEntities)
{
  static constexpr std::size_t kMaxRecursiveEntityResolutionDepth{ 512 };
  if (currentEntities.size() > kMaxRecursiveEntityResolutionDepth) {
    XML_LIB_THROW(SyntaxError("Entity resolution depth exceeds maximum allowed."));
  }

  BufferSource entitySource{ std::string(entityName) };
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
      if (auto nextMappedName = storage.getEntityMapping(mappedEntityName).getInternal(); !nextMappedName.empty()) {
        currentEntities.emplace(mappedEntityName);
        recurseOverEntityReference(nextMappedName, type, currentEntities);
        currentEntities.erase(mappedEntityName);
      }
    }
    entitySource.next();
  }
}

void EntityRecursionChecker::checkRecursiveEntity(const std::string_view &entityName,
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
      if (auto nextMappedName = storage.getEntityMapping(mappedEntityName).getInternal(); !nextMappedName.empty()) {
        currentEntities.emplace(mappedEntityName);
        recurseOverEntityReference(nextMappedName, entityName[0], currentEntities);
        currentEntities.erase(mappedEntityName);
      }
    }
    expandedSource.next();
  }
}

void EntityRecursionChecker::checkForRecursion()
{
  std::set<std::string> currentEntities{};
  for (const auto &[fst, snd] : storage.getMappings()) {
    recurseOverEntityReference(fst, fst[0], currentEntities);
  }
}

// ==========================================
// EntityExpanderEngine Implementation
// ==========================================

void EntityExpanderEngine::invalidateTranslationCache() const
{
  translationCacheValid = false;
  translationCandidates.clear();
}

const std::vector<std::pair<std::string_view, const XML_EntityMapping *>> &EntityExpanderEngine::getTranslationCandidates(char type) const
{
  if (!translationCacheValid || translationType != type) {
    translationCandidates = buildTranslationCandidates(storage.getMappings(), type);
    translationType = type;
    translationCacheValid = true;
  }
  return translationCandidates;
}

XMLValue EntityExpanderEngine::map(const XMLValue &entityReference)
{
  if (const auto *entityMapping = findEntityMapping(storage.getMappings(), entityReference.getUnparsed())) {
    std::string parsed{ entityReference.getUnparsed() };
    if (!entityMapping->getInternal().empty()) {
      parsed = entityMapping->getInternal();
    } else if (entityMapping->isExternal()) {
      const auto &extRef = entityMapping->getExternal();
      const auto &systemID = extRef.getSystemID();
      if (!security.allowExternal() && security.getResolver() == nullptr) {
        XML_LIB_THROW(SyntaxError("External entity resolution is disabled. "
          "Set ParseOptions::allowExternalEntities = true or supply an IEntityResolver."));
      }
      bool resolved = false;
      if (security.getResolver() != nullptr) {
        const std::string publicID = extRef.isPublic() ? extRef.getPublicID() : "";
        if (auto result = security.getResolver()->resolve(systemID, publicID)) {
          parsed = std::move(*result);
          resolved = true;
        }
      }
      if (!resolved) {
        if (std::filesystem::exists(systemID)) {
          parsed = security.getCachedFileMapping(systemID);
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

std::string EntityExpanderEngine::translate(const std::string_view &toTranslate, const char type) const
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
        translated.append(security.getCachedFileMapping(mapping->getExternal().getSystemID()));
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

// ==========================================
// Composite XML_EntityMapper Implementation
// ==========================================

XML_EntityMapper::XML_EntityMapper()
  : storage(), security(), recursionChecker(storage), expanderEngine(storage, security)
{
}

XML_EntityMapper::~XML_EntityMapper() noexcept = default;

bool XML_EntityMapper::isPresent(const std::string_view &entityName) const
{
  return storage.isPresent(entityName);
}

bool XML_EntityMapper::isInternal(const std::string_view &entityName)
{
  return storage.isInternal(entityName);
}

bool XML_EntityMapper::isExternal(const std::string_view &entityName)
{
  return storage.isExternal(entityName);
}

bool XML_EntityMapper::isNotation(const std::string_view &entityName)
{
  return storage.isNotation(entityName);
}

const std::string &XML_EntityMapper::getInternal(const std::string_view &entityName)
{
  return storage.getInternal(entityName);
}

const std::string &XML_EntityMapper::getNotation(const std::string_view &entityName)
{
  return storage.getNotation(entityName);
}

const XMLExternalReference &XML_EntityMapper::getExternal(const std::string_view &entityName)
{
  return storage.getExternal(entityName);
}

void XML_EntityMapper::setInternal(const std::string_view &entityName, const std::string_view &internal)
{
  storage.setInternal(entityName, internal);
  expanderEngine.invalidateTranslationCache();
}

void XML_EntityMapper::setNotation(const std::string_view &entityName, const std::string_view &notation)
{
  storage.setNotation(entityName, notation);
  expanderEngine.invalidateTranslationCache();
}

void XML_EntityMapper::setExternal(const std::string_view &entityName, const XMLExternalReference &external)
{
  storage.setExternal(entityName, external);
  expanderEngine.invalidateTranslationCache();
}

void XML_EntityMapper::reset()
{
  storage.resetToDefault();
  expanderEngine.invalidateTranslationCache();
}

XMLValue XML_EntityMapper::map(const XMLValue &entityReference)
{
  return expanderEngine.map(entityReference);
}

std::string XML_EntityMapper::translate(const std::string_view &toTranslate, const char type) const
{
  return expanderEngine.translate(toTranslate, type);
}

void XML_EntityMapper::checkRecursiveEntity(const std::string_view &entityName, const std::string &expanded, std::set<std::string> &currentEntities)
{
  recursionChecker.checkRecursiveEntity(entityName, expanded, currentEntities);
}

void XML_EntityMapper::checkForRecursion()
{
  recursionChecker.checkForRecursion();
}

void XML_EntityMapper::setExternalEntityPolicy(bool allowExternal, IEntityResolver *resolver)
{
  security.setExternalEntityPolicy(allowExternal, resolver);
}

} // namespace XML_Lib