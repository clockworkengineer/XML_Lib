#pragma once

namespace XML_Lib {
// ========================
// XML character validation
// ========================
bool validChar(Char c);
bool validNameStartChar(Char c);
bool validNameChar(Char c);
bool validName(const String &name);
bool validAttributeValue(const XMLValue &value);
}// namespace XML_Lib
