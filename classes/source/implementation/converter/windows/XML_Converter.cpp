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

#include <string>
#include <string_view>

namespace XML_Lib {

// ============================================================
// Windows API for converting between byte and wide characters.
// ============================================================
int WideCharToBytes(const wchar_t *wideString, const int wideStringLength, char *bytes = nullptr, const int length = 0)
{
  return WideCharToMultiByte(CP_UTF8, 0, wideString, wideStringLength, bytes, length, nullptr, nullptr);
}
/// @brief
/// Implementation of BytesToWideChar.

int BytesToWideChar(const char *bytes, const int length, wchar_t *sideString = nullptr, const int wideStringLength = 0)
{
  return MultiByteToWideChar(CP_UTF8, 0, bytes, length, sideString, wideStringLength);
}

/// @brief
/// Implementation of toUtf8.

std::string toUtf8(std::u16string_view utf16)
{
  const std::wstring wideString{ utf16.begin(), utf16.end() };
  std::string bytes(WideCharToBytes(&wideString[0], static_cast<int>(wideString.length())), 0);
  WideCharToBytes(&wideString[0], -1, &bytes[0], static_cast<int>(bytes.length()));
  return bytes;
}
/// @brief
/// Convert to UTF-16 strings.

std::u16string toUtf16(std::string_view utf8)
{
  std::wstring wideString(BytesToWideChar(utf8.data(), static_cast<int>(utf8.length())), 0);
  BytesToWideChar(utf8.data(), static_cast<int>(utf8.length()), &wideString[0], static_cast<int>(wideString.length()));
  return std::u16string{ wideString.begin(), wideString.end() };
}
}// namespace XML_Lib