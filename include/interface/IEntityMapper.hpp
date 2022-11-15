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
// ===================================================
// Forward declarations for interfaces/classes/structs
// ===================================================
struct XMLEntityMapping;
struct XMLValue;
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
  // ============================
  // Add entity reference mapping
  // ============================
  virtual void add(const std::string &entityName, const XMLEntityMapping &entityMapping) = 0;
  // ==================================
  // Get entity reference mapping entry
  // ==================================
  virtual XMLEntityMapping &get(const std::string &entityName) = 0;
  // ===============================
  // Remove entity reference mapping
  // ===============================
  virtual void remove(const std::string &entityName) = 0;
  // ===========================================
  // Is entity reference mapping entry present ?
  // ===========================================
  [[nodiscard]] virtual bool isPresent(const std::string &entityName) const = 0;
  // =================================
  // Get entity reference mapping list
  // =================================
  virtual std::map<std::string, XMLEntityMapping> &getList() = 0;
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
  virtual void checkForRecursion(const std::string &entityName) = 0;
};
//
}// namespace XML_Lib