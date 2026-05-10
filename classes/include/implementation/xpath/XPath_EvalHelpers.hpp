#pragma once

#include "XPath_Impl.hpp"

namespace XML_Lib {

void appendNodeStringValue(const Node &node, std::string &out);
[[nodiscard]] std::string nodeStringValue(const Node &node);
[[nodiscard]] std::string_view nodeNameView(const Node &node);
[[nodiscard]] std::string_view nodeLocalNameView(const Node &node);
[[nodiscard]] bool matchNodeName(const Node &node, const std::string_view &nameTest);
[[nodiscard]] double stringToNumber(std::string_view s);
[[nodiscard]] std::string resultToString(const XPathResult &r);
[[nodiscard]] std::string_view resultToStringView(const XPathResult &r, std::string &scratch);

} // namespace XML_Lib
