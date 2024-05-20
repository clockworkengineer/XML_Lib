//
// Class: XML
//
// Description: XML parser core functionality.
//
// Dependencies: C++20 - Language standard features used..
//

#include "XML.hpp"
#include "XML_Core.hpp"

namespace XML_Lib {

/// <summary>
/// Parse  and return XML name.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <returns>XML name.</returns>
std::string parseName(ISource &source)
{
  String name;
  while (source.more() && validNameChar(source.current())) {
    name += source.current();
    source.next();
  }
  source.ignoreWS();
  if (!validName(name)) { throw SyntaxError(source.getPosition(), "Invalid name '" + toUtf8(name) + "' encountered."); }
  return (toUtf8(name));
}

/// <summary>
/// Parse and return XML entity reference.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <returns>Parsed entity reference value.</returns>
XMLValue parseEntityReference(ISource &source)
{
  std::string unparsed{ toUtf8(source.current()) };
  source.next();
  unparsed += parseName(source);
  if (source.current() != ';') { throw SyntaxError(source.getPosition(), "Invalidly formed entity reference."); }
  unparsed += ';';
  source.next();
  return (XMLValue{ unparsed, unparsed });
}

/// <summary>
/// Parse a character reference value (hex/dec) returning its value.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <returns>Character reference value.</returns>
XMLValue parseCharacterReference(ISource &source)
{
  std::string unparsed{ "&#" };
  while (source.more() && source.current() != ';') {
    unparsed += toUtf8(source.current());
    source.next();
  }
  if (source.current() != ';') { throw SyntaxError(source.getPosition(), "Invalidly formed  character reference."); }
  source.next();
  unparsed += ';';
  std::string reference{ unparsed };
  char *end;
  long result = 10;
  if (reference[2] == 'x') {
    reference = reference.substr(3, reference.size() - 4);
    result = 16;
  } else {
    reference = reference.substr(2, reference.size() - 3);
  }
  result = std::strtol(reference.c_str(), &end, result);
  if (*end == '\0') {
    if (!validChar(static_cast<Char>(result))) {
      throw SyntaxError(source.getPosition(), "Character reference invalid character.");
    }
    return (XMLValue{ unparsed, toUtf8(static_cast<Char>(result)) });
  }
  throw SyntaxError(source.getPosition(), "Cannot convert character reference.");
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
    return (parseCharacterReference(source));
  } else if (source.current() == '&') {
    return (parseEntityReference(source));
  } else if (validChar(source.current())) {
    std::string character{ toUtf8(source.current()) };
    source.next();
    return (XMLValue{ character, character });
  }
  throw SyntaxError(source.getPosition(), "Invalid character value encountered.");
}

/// <summary>
/// Parse literal string value and return it.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <param name="entityMapper">Entity mapper.</param>
/// <returns>Literal string value.</returns>
XMLValue parseValue(ISource &source, IEntityMapper &entityMapper)
{
  if ((source.current() == '\'') || ((source.current() == '"'))) {
    std::string unparsed, parsed;
    const Char quote = source.current();
    source.next();
    while (source.more() && source.current() != quote) {
      if (XMLValue character{ parseCharacter(source) }; character.isEntityReference()) {
        XMLValue entityReference{ entityMapper.map(character) };
        unparsed += entityReference.getUnparsed();
        parsed += entityReference.getParsed();
      } else {
        unparsed += character.getUnparsed();
        parsed += character.getParsed();
      }
    }
    source.next();
    source.ignoreWS();
    return (XMLValue{ unparsed, parsed, static_cast<char>(quote) });
  }
  throw SyntaxError(source.getPosition(), "Invalid attribute value.");
}

/// <summary>
/// Parse literal string value and return it.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <returns>Literal string value.</returns>
XMLValue parseValue(ISource &source)
{
  if ((source.current() == '\'') || ((source.current() == '"'))) {
    std::string unparsed, parsed;
    Char quote = source.current();
    source.next();
    while (source.more() && source.current() != quote) {
      XMLValue character{ parseCharacter(source) };
      unparsed += character.getUnparsed();
      parsed += character.getParsed();
    }
    source.next();
    source.ignoreWS();
    return (XMLValue{ unparsed, parsed, static_cast<char>(quote) });
  }
  throw SyntaxError(source.getPosition(), "Invalid attribute value.");
}

/// <summary>
/// Extract body of tag up until '>'.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <returns>Body of tag.</returns>
std::string parseTagBody(ISource &source)
{
  source.ignoreWS();
  std::string body;
  while (source.more() && source.current() != '>') {
    body += toUtf8(source.current());
    source.next();
  }
  return (body);
}
}// namespace  XML_Lib
