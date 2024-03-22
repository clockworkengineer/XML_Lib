#pragma once

#include <string>
#include <cwctype>
#include <cstring>

#include "XML_Converter.hpp"

namespace XML_Lib {
// ===================
// Character constants
// ===================
constexpr char kCarriageReturn{ 0x0D };
constexpr char kLineFeed{ 0x0A };
// ==========================================================
// Interface for reading source stream during XML/DTD parsing
// ==========================================================
class ISource
{
public:
  // =============
  // ISource Error
  // =============
  struct Error : public std::runtime_error
  {
    Error(const std::string &message) : std::runtime_error("ISource Error: " + message) {}
  };
  // ========================
  // Constructors/destructors
  // ========================
  virtual ~ISource() = default;
  // =================
  // Current character
  // =================
  [[nodiscard]] virtual Char current() const = 0;
  // ======================
  // Move to next character
  // ======================
  virtual void next() = 0;
  // =========================================
  // Are there still more characters to read ?
  // =========================================
  [[nodiscard]] virtual bool more() const = 0;
  // ======================
  // Back length characters
  // ======================
  virtual void backup(long length) = 0;
  // ===============================================
  // Current character position within source stream
  // ===============================================
  [[nodiscard]] virtual long position() const = 0;
  // =========================================================
  // Return range of characters as a string from source stream
  // =========================================================
  virtual std::string getRange(long start, long end) = 0;
  // ===================================
  // Reset to beginning of source stream
  // ===================================
  virtual void reset() = 0;
  // =====================================
  // Is the current character whitespace ?
  // =====================================
  [[nodiscard]] bool isWS() const { return (std::iswspace(static_cast<wint_t>(current()))) != 0; }
  // ==================================
  // Ignore whitespace on source stream
  // ==================================
  void ignoreWS()
  {
    while (more() && isWS()) { next(); }
  }
  // =================================================================
  // Is current string a match at the current source stream position ?
  // =================================================================
  bool match(const String &target)
  {
    long index = 0;
    while (more() && current() == target[index]) {
      next();
      if (++index == (long)target.length()) { return (true); }
    }
    backup(index);
    return (false);
  }
  bool match(const char *target)
  {
    long index = 0;
    while (more() && (current() == static_cast<XML_Lib::Char>(target[index]))) {
      next();
      if (++index == (long)std::strlen(target)) { return (true); }
    }
    backup(index);
    return (false);
  }
  // ==================================
  // Get current source stream position
  // ==================================
  std::pair<long, long> getPosition() const { return (std::make_pair(lineNo, columnNo)); }
  // =============================================
  // Read bytes representing the current character
  // =============================================
  std::string current_to_bytes() { return (toUtf8(current())); }
  // ==============================
  // Convert to byte string (UTF-8)
  // ==============================
  std::string to_bytes(const String &from) { return (toUtf8(from)); }
  // ==============================
  // Convert to byte string (UTF-8)
  // ==============================
  std::string to_bytes(const Char from) { return (toUtf8(from)); }
  // ================================
  // Convert from byte string (UTF-8)
  // ================================
  String from_bytes(const std::string &from) { return (toUtf16(from)); }
protected:
  long lineNo = 1;
  long columnNo = 1;
};
}// namespace XML_Lib