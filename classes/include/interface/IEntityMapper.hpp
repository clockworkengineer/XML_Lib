#pragma once

namespace XML_Lib {

// ===================================================
// Forward declarations for interfaces/classes/structs
// ===================================================
struct XMLExternalReference;
struct  XMLValue;

// ===========================
// XML Entity mapper interface
// ===========================
class IEntityMapper
{
public:
  // ==================
  // IEntityMapper Error
  // ==================
  struct Error final : std::runtime_error
  {
    explicit Error(const std::string_view &message) : std::runtime_error(std::string("IEntityMapper Error: ").append(message)) {}
  };
  // ========================
  // Constructors/destructors
  // ========================
  virtual ~IEntityMapper() = default;
  // ================================
  // Entity reference get/set details
  // ================================
  virtual bool isInternal(const std::string_view &entityName) = 0;
  virtual bool isExternal(const std::string_view &entityName) = 0;
  virtual bool isNotation(const std::string_view &entityName) = 0;
  virtual const std::string &getInternal(const std::string_view &entityName) = 0;
  virtual const std::string &getNotation(const std::string_view &entityName) = 0;
  virtual const XMLExternalReference &getExternal(const std::string_view &entityName) = 0;
  virtual void setInternal(const std::string_view &entityName, const std::string_view &internal) = 0;
  virtual void setNotation(const std::string_view &entityName, const std::string_view &notation) = 0;
  virtual void setExternal(const std::string_view &entityName, const XMLExternalReference &external) = 0;
  // ===========================================
  // Is entity reference mapping entry present ?
  // ===========================================
  [[nodiscard]] virtual bool isPresent(const std::string_view &entityName) const = 0;
  // ===================================
  // Get mapping for an entity reference
  // ===================================
  virtual XMLValue map(const XMLValue &entityReference) = 0;
  // ==========================================
  // Translate any entity reference in a string
  // ==========================================
  [[nodiscard]] virtual std::string translate(const std::string_view &toTranslate, char type = '%') const = 0;
  // ==============================================
  // Check for a recursive entity reference mapping
  // ==============================================
  virtual void checkForRecursion() = 0;
  // ====================================
  // Reset entity mapper to default state
  // ====================================
  virtual void reset() = 0;
};
//
}// namespace XML_Lib