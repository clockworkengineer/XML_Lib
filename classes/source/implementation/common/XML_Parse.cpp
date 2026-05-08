//
// Class: XML
//
// Description: XML parser core functionality.
//
// Dependencies: C++20 - Language standard features used.
//

#include "XML.hpp"
#include "XML_Core.hpp"
#include <charconv>

namespace XML_Lib {

/// <summary>
/// Parse  and return XML name.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <returns>XML name.</returns>
std::string parseName(ISource &source)
{
  String name;
  name.reserve(16);
  while (source.more() && validNameChar(source.current())) {
    name += source.current();
    source.next();
  }
  source.ignoreWS();
  if (!validName(name)) { XML_LIB_THROW(SyntaxError(source.getPosition(), "Invalid name '" + toUtf8(name) + "' encountered.")); }
  return toUtf8(name);
}

/// <summary>
/// Parse and return XML entity reference.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <returns>Parsed entity reference value.</returns>
XMLValue parseEntityReference(ISource &source)
{
  std::string unparsed;
  unparsed.reserve(16);
  unparsed += toUtf8(source.current());
  source.next();
  unparsed += parseName(source);
  if (source.current() != ';') { XML_LIB_THROW(SyntaxError(source.getPosition(), "Invalidly formed entity reference.")); }
  unparsed += ';';
  source.next();
  return XMLValue{ unparsed, unparsed };
}

/// <summary>
/// Parse a character reference value (hex/dec) returning its value.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <returns>Character reference value.</returns>
XMLValue parseCharacterReference(ISource &source)
{
  std::string unparsed;
  unparsed.reserve(16);
  unparsed += "&#";
  while (source.more() && source.current() != ';') {
    unparsed += toUtf8(source.current());
    source.next();
  }
  if (source.current() != ';') { XML_LIB_THROW(SyntaxError(source.getPosition(), "Invalidly formed  character reference.")); }
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

/// <summary>
/// Parse character value which can be either be a plain character,
/// character reference or entity reference that maps to a string of
/// characters.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <returns>Character value.</returns>
XMLValue parseCharacter(ISource &source)
{
  if (source.match("&#")) {
    return parseCharacterReference(source);
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

namespace {

XMLValue parseValueImpl(ISource &source, IEntityMapper *entityMapper)
{
  if (source.current() == '\'' || source.current() == '"') {
    std::string unparsed;
    std::string parsed;
    unparsed.reserve(32);
    parsed.reserve(32);

    const Char quote = source.current();
    source.next();
    while (source.more() && source.current() != quote) {
      XMLValue character{ parseCharacter(source) };
      if (entityMapper != nullptr && character.isEntityReference()) {
        XMLValue entityReference{ entityMapper->map(character) };
        unparsed += entityReference.getUnparsed();
        parsed += entityReference.getParsed();
      } else {
        unparsed += character.getUnparsed();
        parsed += character.getParsed();
      }
    }
    source.next();
    source.ignoreWS();
    return XMLValue{ unparsed, parsed, static_cast<char>(quote) };
  }
  XML_LIB_THROW(SyntaxError(source.getPosition(), "Invalid attribute value."));
}

} // namespace

/// <summary>
/// Parse a literal string value and return it.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <param name="entityMapper">Entity mapper.</param>
/// <returns>Literal string value.</returns>
XMLValue parseValue(ISource &source, IEntityMapper &entityMapper)
{
  return parseValueImpl(source, &entityMapper);
}

/// <summary>
/// Parse a literal string value and return it.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <returns>Literal string value.</returns>
XMLValue parseValue(ISource &source)
{
  return parseValueImpl(source, nullptr);
}

/// <summary>
/// Extract body of tag up until '>'.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <returns>Body of tag.</returns>
std::string parseTagBody(ISource &source)
{
  source.ignoreWS();
  String body;
  body.reserve(64);
  while (source.more() && source.current() != '>') {
    body += source.current();
    source.next();
  }
  return toUtf8(body);
}
}// namespace  XML_Lib
