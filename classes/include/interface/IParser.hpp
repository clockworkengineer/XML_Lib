
#pragma once

#include <string>
#include <stdexcept>

namespace XML_Lib {

// ====================
// Forward declarations
// ====================

struct XNode;

// =========================
// Interface for XML parser
// =========================
class IParser
{
public:
  // =============
  // IParser Error
  // =============
  struct Error final : std::runtime_error
  {
    explicit Error(const std::string &message) : std::runtime_error("IParser Error: " + message) {}
  };
  // ========================
  // Constructors/destructors
  // ========================
  virtual ~IParser() = default;
  // =================
  // Parse XML source
  // =================
  virtual XNode parse(ISource &source) = 0;
  // ==============
  // XML Validation
  // ==============
  virtual bool canValidate() = 0;
  virtual void validate(XNode &prolog) = 0;
};
}// namespace XML_Lib
