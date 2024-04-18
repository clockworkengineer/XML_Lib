#pragma once

namespace XML_Lib {
// ===============================
// Interface for DTD XML validator
// ===============================
class IValidator
{
public:
  // ==================
  // IValidator Error
  // ==================
  struct Error : public std::runtime_error
  {
    Error(const std::string &message) : std::runtime_error("IValidator Error: " + message) {}
  };
  // ========================
  // Constructors/destructors
  // ========================
  virtual ~IValidator() = default;
  // =========================
  // Parse XML DTD from source
  // =========================
  virtual void parse(ISource &source) = 0;
  // =====================================
  // Stringify XML DTD to text destination
  // =====================================
  virtual void stringify(IDestination &destination) = 0;
  // =========================
  // Validate XML agaisnt DTD
  // =========================
  virtual void validate(const XNode &xNode) = 0;
};

}// namespace XML_Lib