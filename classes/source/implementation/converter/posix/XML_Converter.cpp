//
// Class: Converter
//
// Description: Convert characters to/from UTF8/UTF16 for POSIX/Standard platforms.
//
// Dependencies: C++20 - Language standard features used.
//

#include "XML_Converter.hpp"

#include <codecvt>
#include <locale>
#include <string>

namespace XML_Lib {

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

static std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> utf16Convert;

/// @brief
/// Convert a UTF-16 string to UTF-8.

std::string toUtf8(const std::u16string &utf16) { return utf16Convert.to_bytes(utf16); }

/// @brief
/// Convert to UTF-16 strings.

std::u16string toUtf16(const std::string &utf8) { return utf16Convert.from_bytes(utf8); }

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

}// namespace XML_Lib
