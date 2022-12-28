#pragma once
// =======
// C++ STL
// =======
#include <set>
#include <map>
// =================
// LIBRARY NAMESPACE
// =================
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
  // ==================================
  // Get entity reference mapping entry
  // ==================================
  virtual XMLEntityMapping &get(const std::string &entityName) = 0;
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