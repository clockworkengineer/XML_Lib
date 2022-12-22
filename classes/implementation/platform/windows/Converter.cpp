//
// Class: Converter
//
// Description: Convert characters to/from UTF8 and UTF16.
//
// Dependencies:  Windows - character conversion API.
//
// =======
// Windows
// =======
#include "Windows.h"
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
// =======================
// PUBLIC STATIC VARIABLES
// =======================
// ===============
// PRIVATE METHODS
// ===============
static std::wstring_convert<std::codecvt_utf8_utf16<char32_t>, char32_t> m_UTF32;
// ============================================================
// Windows API for converting between byte and wide characters.
// ============================================================
int WideCharToBytes(wchar_t *wideString, int wideStringLength, char *bytes = nullptr, int length = 0)
{
  return (WideCharToMultiByte(CP_UTF8, 0, wideString, wideStringLength, bytes, length, nullptr, nullptr));
}
int BytesToWideChar(const char *bytes, int length, wchar_t *sideString = nullptr, int wideStringLength = 0)
{
  return (MultiByteToWideChar(CP_UTF8, 0, bytes, length, sideString, wideStringLength));
}
// ==============
// PUBLIC METHODS
// ==============
/// <summary>
/// Convert utf8 <-> utf16 strings.
/// </summary>
std::u16string Converter::toUtf16(const std::string &utf8) const
{
  std::wstring wideString(BytesToWideChar(utf8.c_str(), static_cast<int>(utf8.length())), 0);
  BytesToWideChar(utf8.c_str(), static_cast<int>(utf8.length()), &wideString[0], static_cast<int>(wideString.length()));
  return (std::u16string{ wideString.begin(), wideString.end() });
}
std::string Converter::toUtf8(const std::u16string &utf16) const
{
  std::wstring wideString{ utf16.begin(), utf16.end() };
  std::string bytes(WideCharToBytes(&wideString[0], static_cast<int>(wideString.length())), 0);
  WideCharToBytes(&wideString[0], -1, &bytes[0], static_cast<int>(bytes.length()));
  return bytes;
}
/// <summary>
/// Convert utf8 <-> utf32 strings.
/// </summary>
// std::u32string Converter::toUtf32(const std::string &utf8) const
// {
//   std::wstring wideString(BytesToWideChar(utf8.c_str(), static_cast<int>(utf8.length())), 0);
//   BytesToWideChar(utf8.c_str(), static_cast<int>(utf8.length()), &wideString[0],
//   static_cast<int>(wideString.length())); return (std::u32string{ wideString.begin(), wideString.end() });
// }
// std::string Converter::toUtf8(const std::u32string &utf32) const
// {
//   std::wstring wideString{ utf32.begin(), utf32.end() };
//   std::string bytes(WideCharToBytes(&wideString[0], static_cast<int>(wideString.length())), 0);
//   WideCharToBytes(&wideString[0], -1, &bytes[0], static_cast<int>(bytes.length()));
//   return bytes;
// }
//std::string Converter::toUtf8(char32_t utf32) const { return (m_UTF32.to_bytes(utf32)); }
/// <summary>
/// Convert utf8 <-> utf32 strings.
/// </summary>
std::u32string Converter::toUtf32(const std::string &utf8) const { return (m_UTF32.from_bytes(utf8)); }
std::string Converter::toUtf8(const std::u32string &utf32) const { return (m_UTF32.to_bytes(utf32)); }
std::string Converter::toUtf8(char32_t utf32) const { return (m_UTF32.to_bytes(utf32)); }
}// namespace XML_Lib