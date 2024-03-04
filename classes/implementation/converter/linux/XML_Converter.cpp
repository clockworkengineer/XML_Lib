//
// Class: Converter
//
// Description: Convert characters to/from UTF8/UTF16/UTF32.
//
// Dependencies:   C++20 - Language standard features used.
//

#include <codecvt>
#include <locale>

#include "XML_Converter.hpp"

namespace XML_Lib {

static std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> utf16Convert;
static std::wstring_convert<std::codecvt_utf8_utf16<char32_t>, char32_t> utf32Convert;

/// <summary>
/// Convert to UTF-8 strings.
/// </summary>
std::string XML_Converter::toUtf8(char16_t utf16) const { return (toUtf8(std::u16string(utf16, 1))); }
std::string XML_Converter::toUtf8(const std::u16string &utf16) const { return (utf16Convert.to_bytes(utf16)); }
std::string XML_Converter::toUtf8(char32_t utf32) const { return (utf32Convert.to_bytes(utf32)); }
std::string XML_Converter::toUtf8(const std::u32string &utf32) const { return (utf32Convert.to_bytes(utf32)); }

/// <summary>
/// Convert to UTF-16 strings.
/// </summary>
std::u16string XML_Converter::toUtf16(const std::string &utf8) const { return (utf16Convert.from_bytes(utf8)); }

/// <summary>
/// Convert to UTF-32 strings.
/// </summary>
std::u32string XML_Converter::toUtf32(const std::string &utf8) const { return (utf32Convert.from_bytes(utf8)); }
std::u32string XML_Converter::toUtf32(const std::u16string &utf16) const { return (toUtf32(toUtf8(utf16))); }
}// namespace XML_Lib