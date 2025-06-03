//
// Class: Converter
//
// Description: Convert characters to/from UTF8/UTF16.
//
// Dependencies:  Windows - character conversion API.
//

#include "Windows.h"

#include "XML.hpp"
#include "XML_Converter.hpp"

namespace XML_Lib {

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
std::string toUtf8(char16_t utf16) { return (toUtf8(std::u16string(1, utf16))); }
std::string toUtf8(const std::u16string &utf16)
{
  std::wstring wideString{ utf16.begin(), utf16.end() };
  std::string bytes(WideCharToBytes(&wideString[0], static_cast<int>(wideString.length())), 0);
  WideCharToBytes(&wideString[0], -1, &bytes[0], static_cast<int>(bytes.length()));
  return bytes;
}
/// <summary>
/// Convert to UTF-16 strings.
/// </summary>
std::u16string toUtf16(const std::string &utf8)
{
  std::wstring wideString(BytesToWideChar(utf8.c_str(), static_cast<int>(utf8.length())), 0);
  BytesToWideChar(utf8.c_str(), static_cast<int>(utf8.length()), &wideString[0], static_cast<int>(wideString.length()));
  return (std::u16string{ wideString.begin(), wideString.end() });
}
}// namespace XML_Lib