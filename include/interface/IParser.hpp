
#pragma once

#include <string>
#include <stdexcept>

namespace XML_Lib {

// ====================
// Forward declarations
// ====================

class ISource;
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
  struct Error : public std::runtime_error
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
};
}// namespace XML_Lib