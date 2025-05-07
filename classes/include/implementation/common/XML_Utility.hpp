#pragma once

namespace XML_Lib {
// ===========
// XML utility
// ===========
[[nodiscard]] std::vector<std::string> splitString(const std::string_view &target, char delimiter);
[[nodiscard]] std::string trimString(const std::string_view &target);
[[nodiscard]] std::string toUpperString(const std::string_view &target);
[[nodiscard]] std::string toLowerString(const std::string_view &target);
}// namespace XML_Lib
