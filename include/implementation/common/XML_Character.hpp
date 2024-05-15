#pragma once

namespace XML_Lib {
// ========================
// XML character validation
// ========================
[[nodiscard]] bool validChar(Char c);
[[nodiscard]] bool validNameStartChar(Char c);
[[nodiscard]] bool validNameChar(Char c);
[[nodiscard]] bool validName(const String &name);
[[nodiscard]] bool validAttributeValue(const XMLValue &value);
}// namespace XML_Lib
