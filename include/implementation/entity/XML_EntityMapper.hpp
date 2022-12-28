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
  // Get entity reference mapping entry
  XMLEntityMapping &get(const std::string &entityName) override;
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
  // =================
  // PRIVATE VARIABLES
  // =================
  std::set<std::string> m_currentEntities;
  std::map<std::string, XMLEntityMapping> m_entityMappings;
};
}// namespace XML_Lib
