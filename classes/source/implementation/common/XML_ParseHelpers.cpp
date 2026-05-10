#include "XML_ParseHelpers.hpp"
#include "XML_Core.hpp"
#include "XML_Utility.hpp"
#include <charconv>

namespace XML_Lib {

String readName(ISource &source)
{
  String name;
  name.reserve(16);
  while (source.more() && validNameChar(source.current())) {
    name += source.current();
    source.next();
  }
  return name;
}

std::string readUntil(ISource &source, Char terminator)
{
  String buffer;
  buffer.reserve(64);
  while (source.more() && source.current() != terminator) {
    buffer += source.current();
    source.next();
  }
  return toUtf8(buffer);
}

std::string readEntityReferenceText(ISource &source)
{
  const std::string entityPrefix = toUtf8(source.current());
  source.next();
  const std::string name = parseName(source);

  if (source.current() != ';') {
    XML_LIB_THROW(SyntaxError(source.getPosition(), "Invalidly formed entity reference."));
  }

  const std::string unparsed = entityPrefix + name + ';';
  source.next();
  return unparsed;
}

XMLValue parseCharacterOrReference(ISource &source)
{
  if (source.match("&#")) {
    std::string unparsed{"&#"};
    while (source.more() && source.current() != ';') {
      unparsed += toUtf8(source.current());
      source.next();
    }
    if (source.current() != ';') {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Invalidly formed  character reference."));
    }
    source.next();
    unparsed += ';';

    const bool isHex = unparsed.size() > 3 && unparsed[2] == 'x';
    const size_t valueStart = isHex ? 3 : 2;
    const size_t valueLength = unparsed.size() - valueStart - 1;
    const std::string_view digits{ unparsed.data() + valueStart, valueLength };

    long result = 0;
    const auto [ptr, ec] = std::from_chars(digits.data(), digits.data() + digits.size(), result, isHex ? 16 : 10);
    if (digits.empty()) {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Character reference invalid character."));
    }
    if (ec == std::errc() && ptr == digits.data() + digits.size()) {
      if (result < 0 || !validChar(static_cast<Char>(result))) {
        XML_LIB_THROW(SyntaxError(source.getPosition(), "Character reference invalid character."));
      }
      return XMLValue{ unparsed, toUtf8(static_cast<Char>(result)) };
    }
    XML_LIB_THROW(SyntaxError(source.getPosition(), "Cannot convert character reference."));
  }
  if (source.current() == '&') {
    return parseEntityReference(source);
  }
  if (validChar(source.current())) {
    const std::string character{ toUtf8(source.current()) };
    source.next();
    return XMLValue{ character, character };
  }
  XML_LIB_THROW(SyntaxError(source.getPosition(), "Invalid character value encountered."));
}

void appendTextSegment(std::string &unparsed,
                       std::string &parsed,
                       const XMLValue &character,
                       IEntityMapper *entityMapper)
{
  if (entityMapper != nullptr && character.isEntityReference()) {
    const XMLValue mapped = entityMapper->map(character);
    unparsed += mapped.getUnparsed();
    parsed += mapped.getParsed();
    return;
  }
  unparsed += character.getUnparsed();
  parsed += character.getParsed();
}

XMLValue parseQuotedValue(ISource &source, IEntityMapper *entityMapper)
{
  const Char quote = source.current();
  if (quote != '\'' && quote != '"') {
    XML_LIB_THROW(SyntaxError(source.getPosition(), "Invalid attribute value."));
  }

  std::string unparsed;
  std::string parsed;
  unparsed.reserve(32);
  parsed.reserve(32);

  source.next();
  while (source.more() && source.current() != quote) {
    const XMLValue character = parseCharacterOrReference(source);
    appendTextSegment(unparsed, parsed, character, entityMapper);
  }

  if (source.current() != quote) {
    XML_LIB_THROW(SyntaxError(source.getPosition(), "Invalid attribute value."));
  }
  source.next();
  source.ignoreWS();

  return XMLValue{ unparsed, parsed, static_cast<char>(quote) };
}

} // namespace XML_Lib
