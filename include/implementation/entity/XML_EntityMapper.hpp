#pragma once
// =======
// C++ STL
// =======
#include <string>
#include <map>
#include <filesystem>
#include <set>
// ===
// XML
// ===
#include "XML_Core.hpp"
// ===========================
// XML Entity Mapper interface
// ===========================
#include "IEntityMapper.hpp"
// =================
// LIBRARY NAMESPACE
// =================
namespace XML_Lib {
// ================
// CLASS DEFINITION
// ================
class XML_EntityMapper : public IEntityMapper
{
public:
  // ==========================
  // PUBLIC TYPES AND CONSTANTS
  // ==========================
  // ==================
  // XML Entity mapping
  // ==================
  struct XMLEntityMapping
  {
    explicit XMLEntityMapping(const std::string &internal) : internal(internal) {}
    std::string internal{};
    XMLExternalReference external{ "" };
    std::string notation{};
  };
  // ======================
  // CONSTRUCTOR/DESTRUCTOR
  // ======================
  XML_EntityMapper();
  XML_EntityMapper(const XML_EntityMapper &other) = delete;
  XML_EntityMapper &operator=(const XML_EntityMapper &other) = delete;
  XML_EntityMapper(XML_EntityMapper &&other) = delete;
  XML_EntityMapper &operator=(XML_EntityMapper &&other) = delete;
  ~XML_EntityMapper();
  // ==============
  // PUBLIC METHODS
  // ==============
  // Is entity reference mapping entry present ?
  [[nodiscard]] bool isPresent(const std::string &entityName) const override;
  // Entity reference get/set details
  const std::string &getInternal(const std::string &entityName) override;
  const std::string &getNotation(const std::string &entityName) override;
  const XMLExternalReference &getExternal(const std::string &entityName) override;
  void setInternal(const std::string &entityName, const std::string &internal) override;
  void setNotation(const std::string &entityName, const std::string &notation) override;
  void setExternal(const std::string &entityName, const XMLExternalReference &external) override;
  //  Get mapping for an entity reference
  XMLValue map(const XMLValue &entityReference) override;
  // Translate any entity reference in a string
  [[nodiscard]] std::string translate(const std::string &toTranslate, char type = '%') const override;
  // Check for a recursive entity reference mapping
  void checkForRecursion() override;
  // ================
  // PUBLIC VARIABLES
  // ================
private:
  // ===========================
  // PRIVATE TYPES AND CONSTANTS
  // ===========================
  // ===========================================
  // DISABLED CONSTRUCTORS/DESTRUCTORS/OPERATORS
  // ===========================================
  // ===============
  // PRIVATE METHODS
  // ===============
  void recurseOverEntityReference(const std::string &entityReference, XML_Lib::Char type);
  std::string getFileMappingContents(const std::string &fileName);
  // Get entity reference mapping entry
  XMLEntityMapping &getEntityMapping(const std::string &entityName);
  // =================
  // PRIVATE VARIABLES
  // =================
  std::set<std::string> m_currentEntities;
  std::map<std::string, XMLEntityMapping> m_entityMappings;
};
}// namespace XML_Lib
