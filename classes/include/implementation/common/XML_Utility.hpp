#pragma once

namespace XML_Lib {
// ===========
// XML utility
// ===========
[[nodiscard]] std::vector<std::string> splitString(const std::string &target, char delimiter);
[[nodiscard]] std::string trimString(const std::string &target);
[[nodiscard]] std::string toUpperString(const std::string &target);
[[nodiscard]] std::string toLowerString(const std::string &target);
}// namespace XML_Lib
