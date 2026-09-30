#pragma once

#include "common/XML_Error.hpp"
#include <stdexcept>
#include <string>
#include <string_view>

namespace XML_Lib {
// ==================
// Conversion methods
// ==================
// UTF-8
[[nodiscard]] std::string toUtf8(std::u16string_view utf16);
[[nodiscard]] inline std::string toUtf8(const char16_t utf16)
{
  if (utf16 <= 0x7F) {
    return std::string(1, static_cast<char>(utf16));
  } else if (utf16 <= 0x7FF) {
    std::string s;
    s.reserve(2);
    s.push_back(static_cast<char>(0xC0 | (utf16 >> 6)));
    s.push_back(static_cast<char>(0x80 | (utf16 & 0x3F)));
    return s;
  } else if (utf16 >= 0xD800 && utf16 <= 0xDFFF) {
    XML_LIB_THROW(std::range_error("Unpaired UTF-16 surrogate encountered."));
  } else {
    std::string s;
    s.reserve(3);
    s.push_back(static_cast<char>(0xE0 | (utf16 >> 12)));
    s.push_back(static_cast<char>(0x80 | ((utf16 >> 6) & 0x3F)));
    s.push_back(static_cast<char>(0x80 | (utf16 & 0x3F)));
    return s;
  }
}

// UTF-16
[[nodiscard]] std::u16string toUtf16(std::string_view utf8);
}// namespace XML_Lib