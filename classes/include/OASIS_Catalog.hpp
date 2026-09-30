#pragma once

#include "XML_Core.hpp"
#include "interface/IEntityResolver.hpp"

#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace XML_Lib {

/// @brief OASIS XML Catalogs 1.1 compliant resolver and offline entity mapper.
///
/// Implements IEntityResolver to map SYSTEM, PUBLIC, and schema identifiers to
/// local cached files, relative paths, or in-memory contents without network access.
class OASIS_Catalog : public IEntityResolver
{
public:
  OASIS_Catalog() = default;

  /// @brief Construct and parse catalog rules from an XML string.
  explicit OASIS_Catalog(std::string_view catalogXml, const std::filesystem::path &basePath = {});

  /// @brief Factory to load and parse an OASIS XML catalog file.
  static OASIS_Catalog fromFile(const std::filesystem::path &catalogFilePath);

  virtual ~OASIS_Catalog() override = default;

  /// @brief Load catalog rules from an XML string.
  void loadCatalog(std::string_view catalogXml, const std::filesystem::path &basePath = {});

  /// @brief Load catalog rules from a catalog XML file on disk.
  void loadCatalogFile(const std::filesystem::path &catalogFilePath);

  /// @brief Register an exact SYSTEM identifier mapping.
  void addSystemMapping(std::string_view systemId, std::string_view uriOrPath);

  /// @brief Register an exact PUBLIC identifier mapping.
  void addPublicMapping(std::string_view publicId, std::string_view uriOrPath);

  /// @brief Register a rewrite rule for SYSTEM identifiers.
  void addRewriteSystem(std::string_view systemIdStartString, std::string_view rewritePrefix);

  /// @brief Register a rewrite rule for general URIs.
  void addRewriteURI(std::string_view uriStartString, std::string_view rewritePrefix);

  /// @brief Register in-memory content directly under an identifier.
  void addMemoryContent(std::string_view identifier, std::string_view content);

  /// @brief Resolve an external entity reference (IEntityResolver contract).
  [[nodiscard]] std::optional<std::string>
  resolve(const std::string_view &systemId, const std::string_view &publicId) override;

  /// @brief Resolve a SYSTEM identifier to its destination path or URI.
  [[nodiscard]] std::optional<std::string>
  resolveSystem(const std::string_view &systemId) const;

  /// @brief Resolve a PUBLIC identifier to its destination path or URI.
  [[nodiscard]] std::optional<std::string>
  resolvePublic(const std::string_view &publicId, const std::string_view &systemId = {}) const;

  /// @brief Resolve a general URI via rewriteURI rules.
  [[nodiscard]] std::optional<std::string>
  resolveURI(const std::string_view &uri) const;

private:
  struct RewriteRule
  {
    std::string prefix;
    std::string replacement;
  };

  [[nodiscard]] std::optional<std::string> fetchContent(const std::string &resolvedPathOrId) const;

  std::unordered_map<std::string, std::string> systemEntries;
  std::unordered_map<std::string, std::string> publicEntries;
  std::vector<RewriteRule> rewriteSystemEntries;
  std::vector<RewriteRule> rewriteURIEntries;
  std::unordered_map<std::string, std::string> memoryContents;
};

} // namespace XML_Lib
