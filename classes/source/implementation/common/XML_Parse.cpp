//
// Class: XML
//
// Description: XML parser core functionality.
//
// Dependencies: C++20 - Language standard features used.
//

#include "XML.hpp"
#include "XML_ParseHelpers.hpp"
#include "XML_SourceHelpers.hpp"
#include "XML_Converter.hpp"
#include "XML_Error.hpp"
#include <charconv>

namespace XML_Lib {

/// @brief
/// Parse  and return XML name.

/// @param source XML source stream.
/// @return XML name.
std::string parseName(ISource &source)
{
  const String name = readName(source);
  ignoreWS(source);
  if (!validName(name)) {
    XML_LIB_THROW(SyntaxError(source.getPosition(), "Invalid name '" + toUtf8(name) + "' encountered."));
  }
  return toUtf8(name);
}

/// @brief
/// Parse and return XML entity reference.

/// @param source XML source stream.
/// @return Parsed entity reference value.
XMLValue parseEntityReference(ISource &source)
{
  const std::string unparsed = readEntityReferenceText(source);
  return XMLValue{ unparsed, unparsed };
}

/// @brief
/// Parse a character reference value (hex/dec) returning its value.
/// Source must be positioned immediately after the consumed \"&#\" prefix.

/// @param source XML source stream.
/// @return Character reference value.
XMLValue parseCharacterReference(ISource &source)
{
  return decodeCharRef(source);
}

/// @brief
/// Parse character value which can be either be a plain character,
/// character reference or entity reference that maps to a string of
/// characters.

/// @param source XML source stream.
/// @return Character value.
XMLValue parseCharacter(ISource &source)
{
  return parseCharacterOrReference(source);
}

namespace {

/// @brief
/// Implementation of parseValueImpl.

XMLValue parseValueImpl(ISource &source, IEntityMapper *entityMapper)
{
  return parseQuotedValue(source, entityMapper);
}

} // namespace

/// @brief
/// Parse a literal string value and return it.

/// @param source XML source stream.
/// @param entityMapper Entity mapper.
/// @return Literal string value.
XMLValue parseValue(ISource &source, IEntityMapper &entityMapper)
{
  return parseValueImpl(source, &entityMapper);
}

/// @brief
/// Parse a literal string value and return it.

/// @param source XML source stream.
/// @return Literal string value.
XMLValue parseValue(ISource &source)
{
  return parseValueImpl(source, nullptr);
}

/// @brief
/// Extract body of tag up until '>'.

/// @param source XML source stream.
/// @return Body of tag.
std::string parseTagBody(ISource &source)
{
  ignoreWS(source);
  return readUntil(source, '>');
}
}// namespace  XML_Lib