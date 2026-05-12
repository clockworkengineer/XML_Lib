#pragma once

namespace XML_Lib {
// Forward declarations
struct XMLValue;
class ISource;
class IEntityMapper;
// ===========
// XML parsing
// ===========
[[nodiscard]] XMLValue parseEntityReference(ISource &source);
[[nodiscard]] std::string parseName(ISource &source);
[[nodiscard]] XMLValue parseCharacter(ISource &source);
[[nodiscard]] XMLValue parseValue(ISource &source);
[[nodiscard]] XMLValue parseValue(ISource &source, IEntityMapper &entityMapper);
[[nodiscard]] std::string parseTagBody(ISource &source);
}// namespace XML_Lib