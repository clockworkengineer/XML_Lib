#pragma once
// =================
// LIBRARY NAMESPACE
// =================
namespace XML_Lib {
// ===================================================
// Forward declarations for interfaces/classes/structs
// ===================================================
struct XNodeProlog;
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
  virtual void validate(const XNode &prolog) = 0;
};

}// namespace XML_Lib