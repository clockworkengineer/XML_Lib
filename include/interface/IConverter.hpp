#pragma once

#include <codecvt>
#include <locale>

namespace XML_Lib {
// ====================
// Internal String Type
// ====================
using String = std::u32string;
using Char = String::value_type;
// =================================
// Interface for character converter
// =================================
class IConverter
{
public:
  // ==================
  // IConverter Error
  // ==================
  struct Error : public std::runtime_error
  {
    Error(std::string const &message) : std::runtime_error("IConverter Error: " + message) {}
  };
  // ========================
  // Constructors/destructors
  // ========================
  IConverter() = default;
  IConverter(const IConverter &other) = delete;
  IConverter &operator=(const IConverter &other) = delete;
  IConverter(IConverter &&other) = delete;
  IConverter &operator=(IConverter &&other) = delete;
  virtual ~IConverter() = default;
  // ==================
  // Conversion methods
  // ==================
  // UTF-8
  std::string toUtf8(char16_t utf16) const;
  std::string toUtf8(const std::u16string &utf16) const;
  std::string toUtf8(char32_t utf32) const;
  std::string toUtf8(const std::u32string &utf32) const;
  // UTF-16
  std::u16string toUtf16(const std::string &utf8) const;
  std::u16string toUtf32(const std::u32string &utf32) const;
  // UTF-32
  std::u32string toUtf32(const std::string &utf8) const;
  std::u32string toUtf32(const std::u16string &utf16) const;
};
}// namespace XML_Lib