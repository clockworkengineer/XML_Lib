#pragma once

#include "XML_ExternalReference.hpp"
#include "entity/XML_EntityMapping.hpp"

#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace XML_Lib {

/// Find an existing entity mapping without inserting a default entry.
template <typename EntityMap>
const XML_EntityMapping *findEntityMapping(const EntityMap &entityMappings, const std::string_view &entityName)
{
  if constexpr (std::is_same_v<typename EntityMap::key_type, std::string>) {
    if (const auto it = entityMappings.find(entityName); it != entityMappings.end()) {
      return &it->second;
    }
  }
  return nullptr;
}

/// Ensure an entity mapping exists, inserting a default mapping if required.
template <typename EntityMap>
XML_EntityMapping &ensureEntityMapping(EntityMap &entityMappings, const std::string_view &entityName)
{
  if constexpr (std::is_same_v<typename EntityMap::key_type, std::string>) {
    if (auto it = entityMappings.find(entityName); it != entityMappings.end()) {
      return it->second;
    }
  }
  const auto [it, inserted] = entityMappings.emplace(std::string(entityName), XML_EntityMapping());
  return it->second;
}

/// Build a sorted list of translation candidates for a specific entity prefix type.
template <typename EntityMap>
std::vector<std::pair<std::string_view, const XML_EntityMapping *>> buildTranslationCandidates(
  const EntityMap &entityMappings,
  const char type)
{
  std::vector<std::pair<std::string_view, const XML_EntityMapping *>> candidates;
  candidates.reserve(entityMappings.size());
  for (const auto &[key, mapping] : entityMappings) {
    if (!key.empty() && key[0] == type) {
      candidates.emplace_back(std::string_view(key), &mapping);
    }
  }
  std::sort(candidates.begin(), candidates.end(), [](const auto &a, const auto &b) {
    return a.first.size() > b.first.size();
  });
  return candidates;
}

std::optional<std::pair<std::string_view, const XML_EntityMapping *>> matchEntityPrefix(
  const std::vector<std::pair<std::string_view, const XML_EntityMapping *>> &candidates,
  const std::string_view &text,
  const size_t pos);

} // namespace XML_Lib
