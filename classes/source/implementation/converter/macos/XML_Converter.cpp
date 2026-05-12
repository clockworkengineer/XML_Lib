//
// Class: Converter
//
// Description: Convert characters to/from UTF8/UTF16.
//
// Dependencies: C++20 - Language standard features used.
//
// Note: The macOS UTF-8/UTF-16 conversion uses the same std::codecvt_utf8_utf16
// implementation as the Linux converter.  Both platforms ship a compatible
// libc++ / libstdc++ providing <codecvt> and <locale>, so a shared
// implementation is correct.  If a future macOS release requires a
// platform-specific approach (e.g., CFString / ICU), this file is the
// single place to update.
//

#include "XML_Converter.hpp"

#include <codecvt>
#include <locale>
#include <string>

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
