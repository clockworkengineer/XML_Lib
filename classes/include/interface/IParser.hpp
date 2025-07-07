
#pragma once

namespace XML_Lib {

// ====================
// Forward declarations
// ====================

struct Node;

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
    explicit Error(const std::string_view &message) : std::runtime_error(std::string("IParser Error: ").append(message)) {}
  };
  // ========================
  // Constructors/destructors
  // ========================
  virtual ~IParser() = default;
  // =================
  // Parse XML source
  // =================
  virtual Node parse(ISource &source) = 0;
  // ==============
  // XML Validation
  // ==============
  virtual bool canValidate() = 0;
  virtual void validate(Node &prolog) = 0;
};
}// namespace XML_Lib
