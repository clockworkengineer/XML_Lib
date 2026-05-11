#pragma once

#include "XML_Core.hpp"
#include <string>

namespace XML_Lib {

String readName(ISource &source);
std::string readUntil(ISource &source, Char terminator);
std::string readEntityReferenceText(ISource &source);
XMLValue decodeCharRef(ISource &source);
XMLValue parseCharacterOrReference(ISource &source);
void appendTextSegment(std::string &unparsed,
                       std::string &parsed,
                       const XMLValue &character,
                       IEntityMapper *entityMapper);
XMLValue parseQuotedValue(ISource &source, IEntityMapper *entityMapper);

} // namespace XML_Lib
