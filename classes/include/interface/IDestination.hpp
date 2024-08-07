#pragma once

namespace XML_Lib {

// =======================================================================
// Interface for writing destination stream during XML/DTD_Validator stringification
// =======================================================================
class IDestination
{
public:
  // ==================
  // IDestination Error
  // ==================
  struct Error final : std::runtime_error
  {
    explicit Error(const std::string &message) : std::runtime_error("IDestination Error: " + message) {}
  };
  // ========================
  // Constructors/destructors
  // ========================
  virtual ~IDestination() = default;
  // ========================
  // Add bytes to destination
  // ========================
  virtual void add(const std::string &bytes) = 0;
  virtual void add(Char c) = 0;
  // =============================
  // Clear the current destination
  // =============================
  virtual void clear() = 0;
};
}// namespace XML_Lib