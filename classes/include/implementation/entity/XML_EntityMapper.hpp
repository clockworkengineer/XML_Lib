#pragma once

#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "interface/IEntityMapper.hpp"
#include "entity/XML_EntityMapping.hpp"

namespace XML_Lib {

/// Transparent hash/equality functors for std::string_view lookups.
struct EntityMapHash {
  using is_transparent = void;
  [[nodiscard]] size_t operator()(const std::string_view &sv) const noexcept { return std::hash<std::string_view>{}(sv); }
};

struct EntityMapEq {
  using is_transparent = void;
  [[nodiscard]] bool operator()(const std::string_view &lhs, const std::string_view &rhs) const noexcept { return lhs == rhs; }
};

using EntityMappingMap = std::unordered_map<std::string, XML_EntityMapping, EntityMapHash, EntityMapEq>;

/// @brief Sub-component responsible for entity storage, lookup, and registration (SRP).
class EntityStorage
{
public:
  EntityStorage();
  void resetToDefault();
  [[nodiscard]] bool isPresent(const std::string_view &entityName) const;
  [[nodiscard]] bool isInternal(const std::string_view &entityName) const;
  [[nodiscard]] bool isExternal(const std::string_view &entityName) const;
  [[nodiscard]] bool isNotation(const std::string_view &entityName) const;
  [[nodiscard]] bool isFromExternalSubset(const std::string_view &entityName) const;
  void setFromExternalSubset(const std::string_view &entityName, bool val);
  [[nodiscard]] const std::string &getInternal(const std::string_view &entityName) const;
  [[nodiscard]] const std::string &getNotation(const std::string_view &entityName) const;
  [[nodiscard]] const XMLExternalReference &getExternal(const std::string_view &entityName) const;
  void setInternal(const std::string_view &entityName, const std::string_view &internal);
  void setNotation(const std::string_view &entityName, const std::string_view &notation);
  void setExternal(const std::string_view &entityName, const XMLExternalReference &external);
  [[nodiscard]] XML_EntityMapping &getEntityMapping(const std::string_view &entityName);
  [[nodiscard]] const EntityMappingMap &getMappings() const { return entityMappings; }

private:
  EntityMappingMap entityMappings;
};

/// @brief Sub-component responsible for XXE security policy and external file loading (SRP).
class XXESecurityPolicy
{
public:
  XXESecurityPolicy() = default;
  void setExternalEntityPolicy(bool allowExternal, IEntityResolver *resolver);
  void setBaseDirectory(const std::filesystem::path &baseDir) { baseDirectory = baseDir; }
  [[nodiscard]] const std::filesystem::path &getBaseDirectory() const { return baseDirectory; }
  [[nodiscard]] bool allowExternal() const { return allowExternalEntities; }
  [[nodiscard]] IEntityResolver *getResolver() const { return entityResolver; }
  [[nodiscard]] std::string getCachedFileMapping(const std::string_view &fileName) const;

private:
  bool allowExternalEntities{ false };
  IEntityResolver *entityResolver{ nullptr };
  std::filesystem::path baseDirectory;
  mutable std::unordered_map<std::string, std::string> externalFileCache;
};

/// @brief Sub-component responsible for entity reference recursion checking (SRP).
class EntityRecursionChecker
{
public:
  explicit EntityRecursionChecker(EntityStorage &storage) : storage(storage) {}
  void recurseOverEntityReference(const std::string_view &entityName, Char type, std::set<std::string> &currentEntities);
  void checkRecursiveEntity(const std::string_view &entityName, const std::string &expanded, std::set<std::string> &currentEntities);
  void checkForRecursion();

private:
  EntityStorage &storage;
};

/// @brief Sub-component responsible for entity reference substitution and string translation (SRP).
class EntityExpanderEngine
{
public:
  EntityExpanderEngine(EntityStorage &storage, XXESecurityPolicy &security)
    : storage(storage), security(security) {}
  void invalidateTranslationCache() const;
  [[nodiscard]] XMLValue map(const XMLValue &entityReference);
  [[nodiscard]] std::string translate(const std::string_view &toTranslate, char type = '%') const;

private:
  [[nodiscard]] const std::vector<std::pair<std::string_view, const XML_EntityMapping *>> &getTranslationCandidates(char type) const;

  EntityStorage &storage;
  XXESecurityPolicy &security;
  mutable std::vector<std::pair<std::string_view, const XML_EntityMapping *>> translationCandidates;
  mutable char translationType{ '\0' };
  mutable bool translationCacheValid{ false };
};

/// @brief Composite XML entity mapper orchestrating segregated sub-components.
struct XML_EntityMapper final : IEntityMapper
{
  // EntityMapper Error
  struct Error final : std::runtime_error
  {
    explicit Error(const std::string &message) : std::runtime_error("EntityMapper Error: " + message) {}
  };

  // Constructors/Destructors
  XML_EntityMapper();
  XML_EntityMapper(const XML_EntityMapper &other) = delete;
  XML_EntityMapper &operator=(const XML_EntityMapper &other) = delete;
  XML_EntityMapper(XML_EntityMapper &&other) = delete;
  XML_EntityMapper &operator=(XML_EntityMapper &&other) = delete;
  ~XML_EntityMapper() noexcept override;

  // IEntityRegistry interface
  [[nodiscard]] bool isPresent(const std::string_view &entityName) const override;
  [[nodiscard]] bool isInternal(const std::string_view &entityName) override;
  [[nodiscard]] bool isExternal(const std::string_view &entityName) override;
  [[nodiscard]] bool isNotation(const std::string_view &entityName) override;
  [[nodiscard]] bool isFromExternalSubset(const std::string_view &entityName) const override;
  void setFromExternalSubset(const std::string_view &entityName, bool val) override;
  [[nodiscard]] const std::string &getInternal(const std::string_view &entityName) override;
  [[nodiscard]] const std::string &getNotation(const std::string_view &entityName) override;
  [[nodiscard]] const XMLExternalReference &getExternal(const std::string_view &entityName) override;
  void setInternal(const std::string_view &entityName, const std::string_view &internal) override;
  void setNotation(const std::string_view &entityName, const std::string_view &notation) override;
  void setExternal(const std::string_view &entityName, const XMLExternalReference &external) override;
  void reset() override;

  // IEntityExpander interface
  [[nodiscard]] XMLValue map(const XMLValue &entityReference) override;
  [[nodiscard]] std::string translate(const std::string_view &toTranslate, char type = '%') const override;
  void checkRecursiveEntity(const std::string_view &entityName, const std::string &expanded, std::set<std::string> &currentEntities) override;
  void checkForRecursion() override;

  // ISecurityPolicyManager interface
  void setExternalEntityPolicy(bool allowExternal, IEntityResolver *resolver) override;
  void setBaseDirectory(const std::filesystem::path &baseDir) override { security.setBaseDirectory(baseDir); }

private:
  EntityStorage storage;
  XXESecurityPolicy security;
  EntityRecursionChecker recursionChecker;
  EntityExpanderEngine expanderEngine;
};

} // namespace XML_Lib
