#pragma once

namespace XML_Lib {
// ==================
// Conversion methods
// ==================
// UTF-8
[[nodiscard]] std::string toUtf8(char16_t utf16);
[[nodiscard]] std::string toUtf8(const std::u16string &utf16);
// UTF-16
[[nodiscard]] std::u16string toUtf16(const std::string &utf8);
}// namespace XML_Lib