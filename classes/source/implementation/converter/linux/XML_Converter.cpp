//
// Class: Converter
//
// Description: Convert characters to/from UTF8/UTF16.
//
// Dependencies: C++20 - Language standard features used.
//

#include "XML_Converter.hpp"

#include <codecvt>
#include <locale>
#include <string>

namespace XML_Lib {

static std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> utf16Convert;

/// @brief
/// Convert to UTF-8 strings.

std::string toUtf8(const char16_t utf16) { return toUtf8(std::u16string(1, utf16)); }
/// @brief
/// Convert a UTF-16 string to UTF-8.

std::string toUtf8(const std::u16string &utf16) { return utf16Convert.to_bytes(utf16); }
/// @brief
/// Convert to UTF-16 strings.

std::u16string toUtf16(const std::string &utf8) { return utf16Convert.from_bytes(utf8); }

}// namespace XML_Lib
