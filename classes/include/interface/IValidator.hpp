#pragma once

namespace XML_Lib {

// ===================================================
// Forward declarations for interfaces/classes/structs
// ===================================================
struct XNode;

// =========================================
// Interface for DTD_Validator XML validator
// =========================================
class IValidator
{
public:
  // ==================
  // IValidator Error
  // ==================
  struct Error final : std::runtime_error
  {
    explicit Error(const std::string_view &message) : std::runtime_error(std::string("IValidator Error: ").append(message)) {}
  };
  // ========================
  // Constructors/destructors
  // ========================
  virtual ~IValidator() = default;
  // =========================
  // Parse XML DTD_Validator from source
  // =========================
  virtual void parse(ISource &source) = 0;
  // =====================================
  // Stringify XML DTD_Validator to text destination
  // =====================================
  virtual void stringify(IDestination &destination) = 0;
  // =========================
  // Validate XML against DTD_Validator
  // =========================
  virtual void validate(const XNode &xNode) = 0;
};

}// namespace XML_Lib