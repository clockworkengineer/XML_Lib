#pragma once

namespace XML_Lib {
// ===========
// XML utility
// ===========
std::vector<std::string> splitString(const std::string &stringToSplit, char delimeter);
std::string trimmString(const std::string &stringToTrimm);
std::string toUpperString(const std::string &stringToUpper);
}// namespace XML_Lib
