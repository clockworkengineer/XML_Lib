//
// Class: XML_EntityMapper
//
// Description: XML entity reference mapper.
//
// Dependencies: C++20 - Language standard features used.
//

#include "XML.hpp"
#include "XML_Core.hpp"

namespace XML_Lib {

/// <summary>
/// Initialise entity mapping table with defaults.
/// </summary>
void  XML_EntityMapper::resetToDefault()
{
  entityMappings.clear();
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
std::string XML_EntityMapper::getFileMappingContents(const std::string_view &fileName)
{
  std::string content;
  FileSource entitySource{ fileName };
  while (entitySource.more()) {
    content += toUtf8(entitySource.current());
    entitySource.next();
  }
  return content;
}

/// <summary>
/// Get entity reference from the map.
/// </summary>
/// <param name="entityName">Entity reference name.</param>
/// <returns>Reference to entity mapping in the internal map.</returns>
XML_EntityMapping &XML_EntityMapper::getEntityMapping(const std::string_view &entityName)
{
  if (!isPresent(entityName)) { entityMappings.emplace(entityName, XML_EntityMapping()); }
  if (const auto entity = entityMappings.find(std::string(entityName)); entity != entityMappings.end()) { return entity->second; }
  throw Error("Could not find entity reference in map.");
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
  return entityMappings.contains(std::string(entityName));
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
    // Internal so from memory.
    if (const auto entityMapping = getEntityMapping(entityReference.getUnparsed()); !entityMapping.getInternal().empty()) {
      parsed = entityMapping.getInternal();
    } else
    // External so from a file.
    // *** TODO Need to add support for external other than file ***
    {
      if (std::filesystem::exists(entityMapping.getExternal().getSystemID())) {
        parsed = getFileMappingContents(entityMapping.getExternal().getSystemID());
      } else {
        throw SyntaxError("Entity '" + entityReference.getUnparsed() + "' source file '"
                          + entityMapping.getExternal().getSystemID() + "' does not exist.");
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
  std::string translated { toTranslate} ;
  bool matchFound;
  do {
    matchFound = false;
    for (const auto &[fst, snd] : entityMappings) {
      if (fst[0] == type) {
        if (const size_t position = translated.find(fst); position != std::string::npos) {
          translated.replace(position, fst.length(), snd.getInternal());
          matchFound = true;
          break;
        }
      }
    }
  } while (matchFound);
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
void XML_EntityMapper::checkForRecursion()
{
  std::set<std::string> currentEntities{};
  for (auto &[fst, snd] : entityMappings) {
    recurseOverEntityReference(fst, fst[0], currentEntities);
  }
}
}// namespace XML_Lib
