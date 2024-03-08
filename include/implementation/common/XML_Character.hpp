#pragma once

namespace XML_Lib {
// ========================
// XML character validation
// ========================
bool validChar(XML_Lib::Char c);
bool validNameStartChar(XML_Lib::Char c);
bool validNameChar(XML_Lib::Char c);
bool validName(const XML_Lib::String &name);
bool validAttributeValue(const XMLValue &value);
}// namespace XML_Lib
