#pragma once

namespace XML_Lib {
// =======================
// XML validator interface
// =======================
class IValidator
{
public:
  // ========================
  // Constructors/destructors
  // ========================
  IValidator() = default;
  IValidator(const IValidator &other) = delete;
  IValidator &operator=(const IValidator &other) = delete;
  IValidator(IValidator &&other) = delete;
  IValidator &operator=(IValidator &&other) = delete;
  virtual ~IValidator() = default;
  // ========================
  // Validate XML against DTD
  // ========================
  virtual void validate(const XNode &xNode) = 0;
};

}// namespace XML_Lib