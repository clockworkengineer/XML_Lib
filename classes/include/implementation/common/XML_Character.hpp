#pragma once

namespace XML_Lib {
// ====================
// Internal String Type
// ====================
using String = std::u16string;
using Char = String::value_type;
// ========================
// XML character validation
// ========================
[[nodiscard]] bool validChar(Char c);
[[nodiscard]] bool validNameStartChar(Char c);
[[nodiscard]] bool validNameChar(Char c);
[[nodiscard]] bool validName(const String &name);
[[nodiscard]] bool validAttributeValue(const std::string &value, char quote);
}// namespace XML_Lib
