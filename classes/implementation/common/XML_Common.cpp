//
// Class: XML
//
// Description: XML parser core functionality.
//
// Dependencies:   C++20 - Language standard features used.
//

#include "XML_Core.hpp"

namespace XML_Lib {

/// <summary>
/// Check whether a character is valid for XML.
/// </summary>
/// <param name="c">Character to validate.</param>
/// <returns>true then valid otherwise false.</returns>
bool validChar(XML_Lib::Char c)
{
  return ((c == 0x09) || (c == kLineFeed) || (c == kCarriageReturn) || (c >= 0x20 && c <= 0xD7FF)
          || (c >= 0xE000 && c <= 0xFFFD) || (c >= 0x10000 && c <= 0x10FFFF));
}

/// <summary>
/// Check whether character is a valid to start an XML name with.
/// </summary>
/// <param name="c">Character value to validate.</param>
/// <returns>true then valid otherwise false.</returns>
bool validNameStartChar(XML_Lib::Char c)
{
  return ((c == ':') || (c == '_') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= 0xC0 && c <= 0xD6)
          || (c >= 0xD8 && c <= 0xF6) || (c >= 0xF8 && c <= 0x2FF) || (c >= 0x370 && c <= 0x37D)
          || (c >= 0x37F && c <= 0x1FFF) || (c >= 0x200C && c <= 0x200D) || (c >= 0x2070 && c <= 0x218F)
          || (c >= 0x2C00 && c <= 0x2FEF) || (c >= 0x3001 && c <= 0xD7FF) || (c >= 0xF900 && c <= 0xFDCF)
          || (c >= 0xFDF0 && c <= 0xFFFD) || (c >= 0x10000 && c <= 0xEFFFF));
}

/// <summary>
/// Check whether a character is valid for an XML name.
/// </summary>
/// <param name="c">Name to validate.</param>
/// <returns>true then valid otherwise false.</returns>
bool validNameChar(XML_Lib::Char c)
{
  return (validNameStartChar(c) || (c == '-') || (c == '.') || (c >= '0' && c <= '9') || (c == 0xB7)
          || (c >= 0x0300 && c <= 0x036F) || (c >= 0x203F && c <= 0x2040));
}

/// <summary>
/// Check name that starts with xml is a valid reserved name.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <returns>true then valid otherwise false.</returns>
bool validReservedName(const XML_Lib::String &name)
{
  return ((name.find(U"xmlns") == 0) || (name.find(U"xml-stylesheet") == 0) || (name == U"xml"));
}

/// <summary>
/// Validate XML tag/attribute names.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <returns>true then valid otherwise false.</returns>
bool validName(const XML_Lib::String &name)
{
  XML_Lib::String localName{ name };
  if (localName.empty()) { return (false); }
  std::transform(localName.begin(), localName.end(), localName.begin(), ::tolower);
  if (localName.find(U"xml") == 0 && !(validReservedName(localName))) { return (false); }
  if (!validNameStartChar(localName[0])) { return (false); }
  for (auto it = localName.begin() + 1; it != localName.end(); it++) {
    if (!validNameChar(*it)) { return (false); }
  }
  return (true);
}

/// <summary>
/// Make sure that XML attribute value does not contain any illegal characters.
/// </summary>
/// <param name="">.</param>
/// <returns>true then contains all legal characters otherwise false.</returns>
bool validAttributeValue(const XMLValue &value)
{
  BufferSource valueSource(value.getParsed());
  while (valueSource.more()) {
    if (valueSource.match(U"&#")) {
      parseCharacterReference(valueSource);
    } else if (valueSource.current() == '&') {
      parseEntityReference(valueSource);
    } else if ((valueSource.current() == '<') || (valueSource.current() == '"') || (valueSource.current() == '\'')) {
      return (false);
    } else {
      valueSource.next();
    }
  }
  return (true);
}

/// <summary>
/// Parse  and return XML name.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <returns>XML name.</returns>
std::string parseName(ISource &source)
{
  XML_Lib::String name;
  while (source.more() && validNameChar(source.current())) {
    name += source.current();
    source.next();
  }
  source.ignoreWS();
  if (!validName(name)) {
    throw SyntaxError(source.getPosition(), "Invalid name '" + source.to_bytes(name) + "' encountered.");
  }
  return (source.to_bytes(name));
}

/// <summary>
/// Parse and return XML entity reference.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <returns>Parsed entity reference value.</returns>
XMLValue parseEntityReference(ISource &source)
{
  std::string unparsed{ source.current_to_bytes() };
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
    unparsed += source.current_to_bytes();
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
    if (!validChar(result)) { throw SyntaxError(source.getPosition(), "Character reference invalid character."); }
    return (XMLValue{ unparsed, source.to_bytes(result) });
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
  if (source.match(U"&#")) {
    return (parseCharacterReference(source));
  } else if (source.current() == '&') {
    return (parseEntityReference(source));
  } else if (validChar(source.current())) {
    std::string character{ source.current_to_bytes() };
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
    XML_Lib::Char quote = source.current();
    source.next();
    while (source.more() && source.current() != quote) {
      XMLValue character{ parseCharacter(source) };
      if (character.isEntityReference()) {
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
    return (XMLValue{ unparsed, parsed });
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
    XML_Lib::Char quote = source.current();
    source.next();
    while (source.more() && source.current() != quote) {
      XMLValue character{ parseCharacter(source) };
      unparsed += character.getUnparsed();
      parsed += character.getParsed();
    }
    source.next();
    source.ignoreWS();
    return (XMLValue{ unparsed, parsed });
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
    body += source.current_to_bytes();
    source.next();
  }
  return (body);
}

/// <summary>
/// Split a string into a vector of strings using the passed in delimeter.
/// </summary>
/// <param name="stringToSplit">String to split up.</param>
/// <param name="delimeter">Character delimeter to split on.</param>
/// <returns>Vector of split strings.</returns>
std::vector<std::string> splitString(const std::string &stringToSplit, char delimeter)
{
  std::stringstream sourceStream(stringToSplit);
  std::string splitOffItem;
  std::vector<std::string> splitStrings;
  while (std::getline(sourceStream, splitOffItem, delimeter)) { splitStrings.push_back(splitOffItem); }
  return splitStrings;
}

/// <summary>
/// Trimm whitespace from beginning and end of a string.
/// </summary>
/// <param name="stringToTrimm">String to trimm.</param>
std::string trimmString(const std::string &stringToTrimm)
{
  std::string trimmedString{ stringToTrimm };
  trimmedString.erase(trimmedString.begin(),
    std::find_if(trimmedString.begin(), trimmedString.end(), [](unsigned char ch) { return !std::iswspace(ch); }));
  trimmedString.erase(
    std::find_if(trimmedString.rbegin(), trimmedString.rend(), [](unsigned char ch) { return !std::iswspace(ch); })
      .base(),
    trimmedString.end());
  return (trimmedString);
}

/// <summary>
/// Trimm whitespace from beginning and end of a string.
/// </summary>
/// <param name="stringToTrimm">String to trimm.</param>
std::string toUpperString(const std::string &stringToUpper)
{
  std::string upperCaseString{ stringToUpper };
  std::transform(upperCaseString.begin(), upperCaseString.end(), upperCaseString.begin(), [](unsigned int c) {
    return static_cast<char>(std::toupper(c));
  });
  return (upperCaseString);
}
}// namespace  XML_Lib
