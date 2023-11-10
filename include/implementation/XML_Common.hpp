#pragma once

#include <stdexcept>
#include <filesystem>
#include <map>
#include <set>
#include <sstream>

#include "XML_Sources.hpp"
#include "XML_Destinations.hpp"

#include "IEntityMapper.hpp"

namespace XML_Lib {
// ==============
// XML validation
// ==============
bool validChar(XML_Lib::Char c);
bool validNameStartChar(XML_Lib::Char c);
bool validNameChar(XML_Lib::Char c);
bool validReservedName(const XML_Lib::String &name);
bool validName(const XML_Lib::String &name);
bool validAttributeValue(const XMLValue &value);
// ===========
// XML parsing
// ===========
XMLValue parseEntityReference(ISource &source);
std::string parseName(ISource &source);
XMLValue parseCharacterReference(ISource &source);
XMLValue parseCharacter(ISource &source);
XMLValue parseValue(ISource &source);
XMLValue parseValue(ISource &source, IEntityMapper &entityMapper);
std::string parseTagBody(ISource &source);
// ===========
// XML utility
// ===========
std::vector<std::string> splitString(const std::string &stringToSplit, char delimeter);
std::string trimmString(const std::string &stringToTrimm);
std::string toUpperString(const std::string &stringToUpper);
}// namespace XML_Lib
