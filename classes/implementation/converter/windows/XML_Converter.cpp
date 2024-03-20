//
// Class: Converter
//
// Description: Convert characters to/from UTF8/UTF16/UTF32.
//
// Dependencies:  Windows - character conversion API.
//
// =======
// Windows
// =======
#include "Windows.h"

// ++++++++++++++++++++++++++++++++++++++++++++
// Switch off C++17 warning for conversion code
// ++++++++++++++++++++++++++++++++++++++++++++
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#include "XML_Converter.hpp"

namespace XML_Lib {

static std::wstring_convert<std::codecvt_utf8_utf16<char32_t>, char32_t> utf32Convert;
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

/// <summary>
/// Convert to UTF-8 strings.
/// </summary>
std::string XML_Converter::toUtf8(char16_t utf16) const { return (toUtf8(std::u16string(1, utf16))); }
std::string XML_Converter::toUtf8(const std::u16string &utf16) const
{
  std::wstring wideString{ utf16.begin(), utf16.end() };
  std::string bytes(WideCharToBytes(&wideString[0], static_cast<int>(wideString.length())), 0);
  WideCharToBytes(&wideString[0], -1, &bytes[0], static_cast<int>(bytes.length()));
  return bytes;
}
/// <summary>
/// Convert to UTF-16 strings.
/// </summary>
std::u16string XML_Converter::toUtf16(const std::string &utf8) const
{
  std::wstring wideString(BytesToWideChar(utf8.c_str(), static_cast<int>(utf8.length())), 0);
  BytesToWideChar(utf8.c_str(), static_cast<int>(utf8.length()), &wideString[0], static_cast<int>(wideString.length()));
  return (std::u16string{ wideString.begin(), wideString.end() });
}
}// namespace XML_Lib