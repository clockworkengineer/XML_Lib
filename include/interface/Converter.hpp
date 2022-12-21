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

protected:
  std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> m_UTF16;
  std::wstring_convert<std::codecvt_utf8_utf16<XML_Lib::Char>, XML_Lib::Char> m_UTF8;
};
}// namespace XML_Lib