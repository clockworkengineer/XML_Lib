#pragma once
// =======
// C++ STL
// =======
#include <codecvt>
#include <locale>
// =================
// LIBRARY NAMESPACE
// =================
namespace XML_Lib {
// ====================
// Internal String Type
// ====================
using String = std::u32string;
using Char = String::value_type;
// ===============
// Converter class
// ===============
class Converter
{
public:
  // ========================
  // Constructors/destructors
  // ========================
  Converter() = default;
  Converter(const Converter &other) = delete;
  Converter &operator=(const Converter &other) = delete;
  Converter(Converter &&other) = delete;
  Converter &operator=(Converter &&other) = delete;
  virtual ~Converter() = default;
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