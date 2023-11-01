#pragma once

#include <set>
#include <map>

namespace XML_Lib {
// ====================
// FORWARD DECLARATIONS
// ====================
#include "IEntityMapper_Forward.hpp"
// ===========================
// XML Entity mapper interface
// ===========================
class IEntityMapper
{
public:
  // ========================
  // Constructors/destructors
  // ========================
  IEntityMapper() = default;
  IEntityMapper(const IEntityMapper &other) = delete;
  IEntityMapper &operator=(const IEntityMapper &other) = delete;
  IEntityMapper(IEntityMapper &&other) = delete;
  IEntityMapper &operator=(IEntityMapper &&other) = delete;
  virtual ~IEntityMapper() = default;
  // ================================
  // Entity reference get/set details
  // ================================
  virtual const std::string &getInternal(const std::string &entityName) = 0;
  virtual const std::string &getNotation(const std::string &entityName) = 0;
  virtual const XMLExternalReference &getExternal(const std::string &entityName) = 0;
  virtual void setInternal(const std::string &entityName, const std::string &internal) = 0;
  virtual void setNotation(const std::string &entityName, const std::string &notation) = 0;
  virtual void setExternal(const std::string &entityName, const XMLExternalReference &external) = 0;
  // ===========================================
  // Is entity reference mapping entry present ?
  // ===========================================
  [[nodiscard]] virtual bool isPresent(const std::string &entityName) const = 0;
  // ===================================
  // Get mapping for an entity reference
  // ===================================
  virtual XMLValue map(const XMLValue &entityReference) = 0;
  // ==========================================
  // Translate any entity reference in a string
  // ==========================================
  [[nodiscard]] virtual std::string translate(const std::string &toTranslate, char type = '%') const = 0;
  // ==============================================
  // Check for a recursive entity reference mapping
  // ==============================================
  virtual void checkForRecursion() = 0;
};
//
}// namespace XML_Lib