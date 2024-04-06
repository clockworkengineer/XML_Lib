//
// Class: XML
//
// Description: XML character validation core functionality.
//
// Dependencies: C++20 - Language standard features used.
//

#include "XML.hpp"
#include "XML_Core.hpp"

namespace XML_Lib {

XMLValue parseCharacterReference(ISource &source);

/// <summary>
/// Check whether a character is valid for XML.
/// </summary>
/// <param name="c">Character to validate.</param>
/// <returns>true then valid otherwise false.</returns>
bool validChar(XML_Lib::Char c)
{
  return ((c == 0x09) || (c == kLineFeed) || (c == kCarriageReturn) || (c >= 0x20 && c <= 0xD7FF)
          || (c >= 0xE000 && c <= 0xFFFD));
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
          || (c >= 0xFDF0 && c <= 0xFFFD));
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
  return ((name.find(u"xmlns") == 0) || (name.find(u"xml-stylesheet") == 0) || (name == u"xml"));
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
  std::transform(localName.begin(), localName.end(), localName.begin(), [](XML_Lib::Char c) {
    return static_cast<XML_Lib::Char>(std::tolower(static_cast<int>(c)));
  });
  if (localName.find(u"xml") == 0 && !(validReservedName(localName))) { return (false); }
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
  // Parsed value to validate
  if (!value.getParsed().empty()) {
    BufferSource source(value.getParsed());
    while (source.more()) {
      if (source.match("&#")) {
        parseCharacterReference(source);
      } else if (source.current() == '&') {
        parseEntityReference(source);
      } else if ((source.current() == '"') || (source.current() == '\'')) {
        if (source.current() == value.getQuote()) {
          return (false);
        } else {
          source.next();
          continue;
        }
      } else if (source.current() == '<') {
        return (false);
      } else {
        source.next();
      }
    }
  }
  return (true);
}

}// namespace  XML_Lib
