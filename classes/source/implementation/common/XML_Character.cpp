//
// Class: XML
//
// Description: XML character validation core functionality.
//
// Dependencies: C++20 - Language standard features used.
//

#include "XML_Character.hpp"
#include "XML_SourceHelpers.hpp"
#include "XML_Value.hpp"
#include "XML_Parse.hpp"
#include "XML_Sources.hpp"

namespace XML_Lib {

XMLValue parseCharacterReference(ISource &source);

/// <summary>
/// Check whether a character is valid for XML.
/// </summary>
/// <param name="c">Character to validate.</param>
/// <returns>True then valid otherwise false.</returns>
bool validChar(const Char c)
{
  if (c >= 0xD800 && c <= 0xDFFF) { return true; }
  return validChar(static_cast<std::uint32_t>(c));
}

bool validChar(const char c)
{
  return validChar(static_cast<std::uint32_t>(static_cast<unsigned char>(c)));
}

bool validChar(const std::uint32_t c)
{
  return c == 0x09 || c == kLineFeed || c == kCarriageReturn || (c >= 0x20 && c <= 0xD7FF)
         || (c >= 0xE000 && c <= 0xFFFD) || (c >= 0x10000 && c <= 0x10FFFF);
}

/// <summary>
/// Check whether character is a valid to start an XML name with.
/// </summary>
/// <param name="c">Character value to validate.</param>
/// <returns>true then valid otherwise false.</returns>
bool validNameStartChar(const Char c)
{
  return c == ':' || c == '_' || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= 0xC0 && c <= 0xD6)
         || (c >= 0xD8 && c <= 0xF6) || (c >= 0xF8 && c <= 0x2FF) || (c >= 0x370 && c <= 0x37D)
         || (c >= 0x37F && c <= 0x1FFF) || (c >= 0x200C && c <= 0x200D) || (c >= 0x2070 && c <= 0x218F)
         || (c >= 0x2C00 && c <= 0x2FEF) || (c >= 0x3001 && c <= 0xD7FF) || (c >= 0xF900 && c <= 0xFDCF)
         || (c >= 0xFDF0 && c <= 0xFFFD);
}

bool validNameStartChar(const char c)
{
  return validNameStartChar(static_cast<Char>(static_cast<unsigned char>(c)));
}

/// <summary>
/// Check whether a character is valid for an XML name.
/// </summary>
/// <param name="c">Name to validate.</param>
/// <returns>true then valid otherwise false.</returns>
bool validNameChar(const Char c)
{
  return validNameStartChar(c) || c == '-' || c == '.' || (c >= '0' && c <= '9') || c == 0xB7
         || (c >= 0x0300 && c <= 0x036F) || (c >= 0x203F && c <= 0x2040);
}

bool validNameChar(const char c)
{
  return validNameChar(static_cast<Char>(static_cast<unsigned char>(c)));
}

/// <summary>
/// Validate XML tag/attribute names.
/// </summary>
/// <param name="name">XML name to check.</param>
/// <returns>true then valid otherwise false.</returns>
bool validName(const String &name)
{
  if (name.empty()) { return false; }
  if (!validNameStartChar(name[0])) { return false; }
  for (auto it = name.begin() + 1; it != name.end(); ++it) {
    if (!validNameChar(*it)) { return false; }
  }
  return true;
}

/// <summary>
/// Make sure that the XML attribute value does not contain any illegal characters.
/// </summary>
/// <param name="value">XML value to check.</param>>
/// <param name="quote">XML value quote type.</param>>
/// <returns>true then contains all legal characters otherwise false.</returns>
bool validAttributeValue(const std::string_view &value, const char quote )
{
  // Parsed value to validate
  if (!value.empty()) {
    BufferSource source {std::string(value)};
    while (source.more()) {
      if (match(source, "&#")) {
        parseCharacterReference(source);
      } else if (source.current() == '&') {
         auto _ = parseEntityReference(source);
      } else if (source.current() != '"' && source.current() != '\'') {
        if (source.current() == '<') { return false; }
        source.next();
      } else {
        if (source.current() == quote) { return false; }
        source.next();
      }
    }
  }
  return true;
}

}// namespace  XML_Lib
