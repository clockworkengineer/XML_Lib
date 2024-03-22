#pragma once

#include <string>

namespace XML_Lib {
// ====================
// Internal String Type
// ====================
using String = std::u16string;
using Char = String::value_type;
// ==================
// Conversion methods
// ==================
// UTF-8
std::string toUtf8(char16_t utf16);
std::string toUtf8(const std::u16string &utf16);
// UTF-16
std::u16string toUtf16(const std::string &utf8);

}// namespace XML_Lib