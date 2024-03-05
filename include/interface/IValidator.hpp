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
  // =======================
  // Validate XML using DTD
  // =======================
  virtual void validate(const XNode &xNode) = 0;
};

}// namespace XML_Lib