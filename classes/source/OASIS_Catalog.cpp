//
// Class: OASIS_Catalog
//
// Description: OASIS XML Catalogs 1.1 resolver.
//
// Dependencies: C++20 - Language standard features used.
//

#include "OASIS_Catalog.hpp"
#include "XMLReader.hpp"
#include "implementation/io/XML_FileIO.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>

namespace XML_Lib {

OASIS_Catalog::OASIS_Catalog(std::string_view catalogXml, const std::filesystem::path &basePath)
{
  loadCatalog(catalogXml, basePath);
}

OASIS_Catalog OASIS_Catalog::fromFile(const std::filesystem::path &catalogFilePath)
{
  OASIS_Catalog catalog;
  catalog.loadCatalogFile(catalogFilePath);
  return catalog;
}

void OASIS_Catalog::loadCatalogFile(const std::filesystem::path &catalogFilePath)
{
  std::ifstream file(catalogFilePath, std::ios::binary);
  if (!file.is_open()) { return; }
  std::ostringstream ss;
  ss << file.rdbuf();
  loadCatalog(ss.str(), catalogFilePath.parent_path());
}

void OASIS_Catalog::loadCatalog(std::string_view catalogXml, const std::filesystem::path &basePath)
{
  if (catalogXml.empty()) { return; }

  XMLReader reader(catalogXml);
  while (reader.read()) {
    if (reader.nodeType() == XMLReader::NodeType::ElementStart) {
      const auto tag = reader.name();
      // Handle <system systemId="..." uri="..."/>
      if (tag == "system" || tag.ends_with(":system")) {
        const auto sysId = reader.getAttribute("systemId");
        const auto uri = reader.getAttribute("uri");
        if (!sysId.empty() && !uri.empty()) {
          std::string resolvedUri(uri);
          if (!basePath.empty() && !resolvedUri.starts_with("http://") && !resolvedUri.starts_with("https://")
              && !std::filesystem::path(resolvedUri).is_absolute()) {
            resolvedUri = (basePath / resolvedUri).string();
          }
          systemEntries[std::string(sysId)] = std::move(resolvedUri);
        }
      }
      // Handle <public publicId="..." uri="..."/>
      else if (tag == "public" || tag.ends_with(":public")) {
        const auto pubId = reader.getAttribute("publicId");
        const auto uri = reader.getAttribute("uri");
        if (!pubId.empty() && !uri.empty()) {
          std::string resolvedUri(uri);
          if (!basePath.empty() && !resolvedUri.starts_with("http://") && !resolvedUri.starts_with("https://")
              && !std::filesystem::path(resolvedUri).is_absolute()) {
            resolvedUri = (basePath / resolvedUri).string();
          }
          publicEntries[std::string(pubId)] = std::move(resolvedUri);
        }
      }
      // Handle <rewriteSystem systemIdStartString="..." rewritePrefix="..."/>
      else if (tag == "rewriteSystem" || tag.ends_with(":rewriteSystem")) {
        const auto startStr = reader.getAttribute("systemIdStartString");
        const auto prefix = reader.getAttribute("rewritePrefix");
        if (!startStr.empty() && !prefix.empty()) {
          std::string resolvedPrefix(prefix);
          if (!basePath.empty() && !resolvedPrefix.starts_with("http://") && !resolvedPrefix.starts_with("https://")
              && !std::filesystem::path(resolvedPrefix).is_absolute()) {
            resolvedPrefix = (basePath / resolvedPrefix).string();
          }
          rewriteSystemEntries.push_back({ std::string(startStr), std::move(resolvedPrefix) });
        }
      }
      // Handle <rewriteURI uriStartString="..." rewritePrefix="..."/>
      else if (tag == "rewriteURI" || tag.ends_with(":rewriteURI")) {
        const auto startStr = reader.getAttribute("uriStartString");
        const auto prefix = reader.getAttribute("rewritePrefix");
        if (!startStr.empty() && !prefix.empty()) {
          std::string resolvedPrefix(prefix);
          if (!basePath.empty() && !resolvedPrefix.starts_with("http://") && !resolvedPrefix.starts_with("https://")
              && !std::filesystem::path(resolvedPrefix).is_absolute()) {
            resolvedPrefix = (basePath / resolvedPrefix).string();
          }
          rewriteURIEntries.push_back({ std::string(startStr), std::move(resolvedPrefix) });
        }
      }
      // Handle <nextCatalog catalog="..."/>
      else if (tag == "nextCatalog" || tag.ends_with(":nextCatalog")) {
        const auto nextCat = reader.getAttribute("catalog");
        if (!nextCat.empty()) {
          const auto nextPath = basePath.empty() ? std::filesystem::path(nextCat) : basePath / nextCat;
          loadCatalogFile(nextPath);
        }
      }
    }
  }
}

void OASIS_Catalog::addSystemMapping(std::string_view systemId, std::string_view uriOrPath)
{
  systemEntries[std::string(systemId)] = std::string(uriOrPath);
}

void OASIS_Catalog::addPublicMapping(std::string_view publicId, std::string_view uriOrPath)
{
  publicEntries[std::string(publicId)] = std::string(uriOrPath);
}

void OASIS_Catalog::addRewriteSystem(std::string_view systemIdStartString, std::string_view rewritePrefix)
{
  rewriteSystemEntries.push_back({ std::string(systemIdStartString), std::string(rewritePrefix) });
}

void OASIS_Catalog::addRewriteURI(std::string_view uriStartString, std::string_view rewritePrefix)
{
  rewriteURIEntries.push_back({ std::string(uriStartString), std::string(rewritePrefix) });
}

void OASIS_Catalog::addMemoryContent(std::string_view identifier, std::string_view content)
{
  memoryContents[std::string(identifier)] = std::string(content);
}

std::optional<std::string> OASIS_Catalog::resolveSystem(const std::string_view &systemId) const
{
  const std::string sId(systemId);
  const auto it = systemEntries.find(sId);
  if (it != systemEntries.end()) { return it->second; }

  // Longest matching prefix for rewriteSystem
  const RewriteRule *bestMatch = nullptr;
  for (const auto &rule : rewriteSystemEntries) {
    if (sId.starts_with(rule.prefix)) {
      if (!bestMatch || rule.prefix.length() > bestMatch->prefix.length()) {
        bestMatch = &rule;
      }
    }
  }

  if (bestMatch) {
    return bestMatch->replacement + sId.substr(bestMatch->prefix.length());
  }

  return std::nullopt;
}

std::optional<std::string> OASIS_Catalog::resolvePublic(const std::string_view &publicId,
                                                      const std::string_view &systemId) const
{
  if (!systemId.empty()) {
    auto sysMatch = resolveSystem(systemId);
    if (sysMatch.has_value()) { return sysMatch; }
  }

  const std::string pId(publicId);
  const auto it = publicEntries.find(pId);
  if (it != publicEntries.end()) { return it->second; }

  return std::nullopt;
}

std::optional<std::string> OASIS_Catalog::resolveURI(const std::string_view &uri) const
{
  const std::string u(uri);
  const RewriteRule *bestMatch = nullptr;
  for (const auto &rule : rewriteURIEntries) {
    if (u.starts_with(rule.prefix)) {
      if (!bestMatch || rule.prefix.length() > bestMatch->prefix.length()) {
        bestMatch = &rule;
      }
    }
  }

  if (bestMatch) {
    return bestMatch->replacement + u.substr(bestMatch->prefix.length());
  }

  return std::nullopt;
}

std::optional<std::string> OASIS_Catalog::fetchContent(const std::string &resolvedPathOrId) const
{
  // 1. Check in-memory contents cache
  const auto memIt = memoryContents.find(resolvedPathOrId);
  if (memIt != memoryContents.end()) {
    return memIt->second;
  }

  // 2. Check if file exists on disk
  std::error_code ec;
  if (std::filesystem::exists(resolvedPathOrId, ec)) {
    std::ifstream file(resolvedPathOrId, std::ios::binary);
    if (file.is_open()) {
      std::ostringstream ss;
      ss << file.rdbuf();
      return ss.str();
    }
  }

  // 3. Return resolved string itself (e.g. URI or virtual path)
  return resolvedPathOrId;
}

std::optional<std::string> OASIS_Catalog::resolve(const std::string_view &systemId,
                                                 const std::string_view &publicId)
{
  if (!systemId.empty()) {
    auto resSys = resolveSystem(systemId);
    if (resSys.has_value()) {
      return fetchContent(resSys.value());
    }
  }

  if (!publicId.empty()) {
    auto resPub = resolvePublic(publicId, systemId);
    if (resPub.has_value()) {
      return fetchContent(resPub.value());
    }
  }

  if (!systemId.empty()) {
    auto resUri = resolveURI(systemId);
    if (resUri.has_value()) {
      return fetchContent(resUri.value());
    }
  }

  return std::nullopt;
}

} // namespace XML_Lib
