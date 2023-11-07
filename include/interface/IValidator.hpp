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
    Error(std::string const &message) : std::runtime_error("IValidator Error: " + message) {}
  };
  // ========================
  // Constructors/destructors
  // ========================
  IValidator() = default;
  IValidator(const IValidator &other) = delete;
  IValidator &operator=(const IValidator &other) = delete;
  IValidator(IValidator &&other) = delete;
  IValidator &operator=(IValidator &&other) = delete;
  virtual ~IValidator() = default;
  // =======================
  // Validate XML using DTD
  // =======================
  virtual void validate(const XNode &xNode) = 0;
};

}// namespace XML_Lib