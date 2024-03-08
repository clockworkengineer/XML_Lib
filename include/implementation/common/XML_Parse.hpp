#pragma once

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
}// namespace XML_Lib