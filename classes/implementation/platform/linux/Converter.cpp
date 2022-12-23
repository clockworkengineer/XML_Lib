//
// Class: Converter
//
// Description: Convert characters to/from UTF8 and UTF16.
//
// Dependencies:   C20++ - Language standard features used.
//
// =======
// C++ STL
// =======
#include <codecvt>
#include <locale>
// =================
// CLASS DEFINITIONS
// =================
#include "Converter.hpp"
// ====================
// CLASS IMPLEMENTATION
// ====================
// =================
// LIBRARY NAMESPACE
// =================
namespace XML_Lib {
// ===========================
// PRIVATE TYPES AND CONSTANTS
// ===========================
// ==========================
// PUBLIC TYPES AND CONSTANTS
// ==========================
// ========================
// PRIVATE STATIC VARIABLES
// ========================
static std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> m_UTF16;
static std::wstring_convert<std::codecvt_utf8_utf16<char32_t>, char32_t> m_UTF32;
// =======================
// PUBLIC STATIC VARIABLES
// =======================
// ===============
// PRIVATE METHODS
// ===============
// ==============
// PUBLIC METHODS
// ==============
/// <summary>
/// Convert utf8 <-> utf16 strings.
/// </summary>
std::u16string Converter::toUtf16(const std::string &utf8) const { return (m_UTF16.from_bytes(utf8)); }
std::string Converter::toUtf8(const std::u16string &utf16) const { return (m_UTF16.to_bytes(utf16)); }
/// <summary>
/// Convert utf8/utf16 <-> utf32 strings.
/// </summary>
std::u32string Converter::toUtf32(const std::string &utf8) const { return (m_UTF32.from_bytes(utf8)); }
std::u32string toUtf32(const std::u16string &utf16) const {}
std::string Converter::toUtf8(const std::u32string &utf32) const { return (m_UTF32.to_bytes(utf32)); }
}// namespace XML_Lib
