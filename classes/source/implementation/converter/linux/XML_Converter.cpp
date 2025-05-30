//
// Class: Converter
//
// Description: Convert characters to/from UTF8/UTF16.
//
// Dependencies: C++20 - Language standard features used.
//

#include <codecvt>
#include <locale>

#include "../../../../../include/implementation/converter/XML_Converter.hpp"

namespace XML_Lib {

static std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> utf16Convert;

/// <summary>
/// Convert to UTF-8 strings.
/// </summary>
std::string toUtf8(const char16_t utf16) { return toUtf8(std::u16string(1, utf16)); }
std::string toUtf8(const std::u16string &utf16) { return utf16Convert.to_bytes(utf16); }
/// <summary>
/// Convert to UTF-16 strings.
/// </summary>
std::u16string toUtf16(const std::string &utf8) { return utf16Convert.from_bytes(utf8); }

}// namespace XML_Lib
