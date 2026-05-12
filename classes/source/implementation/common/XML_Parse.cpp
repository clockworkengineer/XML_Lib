//
// Class: XML
//
// Description: XML parser core functionality.
//
// Dependencies: C++20 - Language standard features used.
//

#include "XML.hpp"
#include "XML_Core.hpp"
#include "XML_ParseHelpers.hpp"
#include <charconv>

namespace XML_Lib {

/// <summary>
/// Parse  and return XML name.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <returns>XML name.</returns>
std::string parseName(ISource &source)
{
  const String name = readName(source);
  ignoreWS(source);
  if (!validName(name)) {
    XML_LIB_THROW(SyntaxError(source.getPosition(), "Invalid name '" + toUtf8(name) + "' encountered."));
  }
  return toUtf8(name);
}

/// <summary>
/// Parse and return XML entity reference.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <returns>Parsed entity reference value.</returns>
XMLValue parseEntityReference(ISource &source)
{
  const std::string unparsed = readEntityReferenceText(source);
  return XMLValue{ unparsed, unparsed };
}

/// <summary>
/// Parse a character reference value (hex/dec) returning its value.
/// Source must be positioned immediately after the consumed \"&#\" prefix.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <returns>Character reference value.</returns>
XMLValue parseCharacterReference(ISource &source)
{
  return decodeCharRef(source);
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
  return parseCharacterOrReference(source);
}

namespace {

XMLValue parseValueImpl(ISource &source, IEntityMapper *entityMapper)
{
  return parseQuotedValue(source, entityMapper);
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
  ignoreWS(source);
  return readUntil(source, '>');
}
}// namespace  XML_Lib
