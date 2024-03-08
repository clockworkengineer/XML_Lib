#pragma once

#include <stdexcept>
#include <filesystem>
#include <map>
#include <set>
#include <sstream>

#include "XML_Sources.hpp"
#include "XML_Destinations.hpp"

namespace XML_Lib {
// ===========
// XML parsing
// ===========
XMLValue parseEntityReference(ISource &source);
std::string parseName(ISource &source);
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
