#pragma once

#include <codecvt>
#include <locale>

namespace XML_Lib {
// ====================
// Internal String Type
// ====================
using String = std::u16string;
#define STR(value) (u##value)
using Char = String::value_type;
// =================================
// Interface for character converter
// =================================
class XML_Converter
{
public:
  // ==================
  // XML_Converter Error
  // ==================
  struct Error : public std::runtime_error
  {
    Error(const std::string &message) : std::runtime_error("XML_Converter Error: " + message) {}
  };
  // ========================
  // Constructors/destructors
  // ========================
  XML_Converter() = default;
  XML_Converter(const XML_Converter &other) = delete;
  XML_Converter &operator=(const XML_Converter &other) = delete;
  XML_Converter(XML_Converter &&other) = delete;
  XML_Converter &operator=(XML_Converter &&other) = delete;
  virtual ~XML_Converter() = default;
  // ==================
  // Conversion methods
  // ==================
  // UTF-8
  std::string toUtf8(char16_t utf16) const;
  std::string toUtf8(const std::u16string &utf16) const;
  // UTF-16
  std::u16string toUtf16(const std::string &utf8) const;
};
}// namespace XML_Lib