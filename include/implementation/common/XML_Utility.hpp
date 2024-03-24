#pragma once

namespace XML_Lib {
// ===========
// XML utility
// ===========
std::vector<std::string> splitString(const std::string &target, char delimeter);
std::string trimmString(const std::string &target);
std::string toUpperString(const std::string &target);
std::string toLowerString(const std::string &target);
}// namespace XML_Lib
