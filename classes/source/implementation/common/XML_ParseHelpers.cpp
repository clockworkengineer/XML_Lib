#include "XML_ParseHelpers.hpp"
#include "XML_Parse.hpp"
#include "XML_SourceHelpers.hpp"
#include "XML_Converter.hpp"
#include "XML_Error.hpp"
#include "XML_Utility.hpp"
#include "implementation/parser/Default_Parser.hpp"
#include <charconv>

namespace XML_Lib {

/// Decode a character reference from source, where source is already positioned
/// immediately after the consumed \"&#\" prefix. Reads digits/letters up to ';',
/// validates, and returns an XMLValue with the original &#...; unparsed form and
/// the decoded UTF-8 character as the parsed form.
namespace {

[[nodiscard]] bool isHighSurrogate(Char c)
{
  return c >= 0xD800 && c <= 0xDBFF;
}

[[nodiscard]] bool isLowSurrogate(Char c)
{
  return c >= 0xDC00 && c <= 0xDFFF;
}

[[nodiscard]] std::u16string toUtf16CodePoint(std::uint32_t codePoint)
{
  if (codePoint <= 0xFFFF) {
    return std::u16string{ static_cast<char16_t>(codePoint) };
  }
  codePoint -= 0x10000;
  const char16_t high = static_cast<char16_t>(0xD800 + ((codePoint >> 10) & 0x3FF));
  const char16_t low = static_cast<char16_t>(0xDC00 + (codePoint & 0x3FF));
  return std::u16string{ high, low };
}

} // namespace

XMLValue decodeCharRef(ISource &source)
{
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
    if (result < 0 || !validChar(static_cast<std::uint32_t>(result))) {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Character reference invalid character."));
    }
    return XMLValue{ unparsed, toUtf8(toUtf16CodePoint(static_cast<std::uint32_t>(result))) };
  }
  XML_LIB_THROW(SyntaxError(source.getPosition(), "Cannot convert character reference."));
}

/// @brief
/// Implementation of readName.

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

/// @brief
/// Implementation of readUntil.

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

/// @brief
/// Implementation of readEntityReferenceText.

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

/// @brief
/// Implementation of parseCharacterOrReference.

XMLValue parseCharacterOrReference(ISource &source)
{
  if (match(source, "&#")) {
    return decodeCharRef(source);
  }
  if (source.current() == '&') {
    return parseEntityReference(source);
  }
  if (isHighSurrogate(source.current())) {
    const Char high = source.current();
    source.next();
    if (!source.more() || !isLowSurrogate(source.current())) {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Invalid character value encountered."));
    }
    const Char low = source.current();
    const std::u16string utf16{ high, low };
    const std::uint32_t codePoint = 0x10000u + ((static_cast<std::uint32_t>(high) - 0xD800u) << 10)
                                + (static_cast<std::uint32_t>(low) - 0xDC00u);
    if (!validChar(codePoint)) {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Invalid character value encountered."));
    }
    source.next();
    const std::string character = toUtf8(utf16);
    return XMLValue{ character, character };
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
    const std::string entName = character.getUnparsed();
    if (Default_Parser::isStandalone() && entityMapper->isFromExternalSubset(entName)) {
      XML_LIB_THROW(SyntaxError("Standalone document must not reference entity '" + entName + "' declared in external subset."));
    }
    if (entityMapper->isExternal(entName)) {
      XML_LIB_THROW(SyntaxError("Attribute values must not contain references to external entities."));
    }
    const XMLValue mapped = entityMapper->map(character);
    std::string expandedText = mapped.getParsed();
    size_t depth = 0;
    size_t searchPos = 0;
    while (searchPos < expandedText.size() && depth < 64) {
      const size_t ampPos = expandedText.find('&', searchPos);
      if (ampPos == std::string::npos) break;
      const size_t semiPos = expandedText.find(';', ampPos);
      if (semiPos == std::string::npos) break;
      const std::string ref = expandedText.substr(ampPos, semiPos - ampPos + 1);
      if (ref.starts_with("&#") || ref == "&amp;" || ref == "&lt;" || ref == "&gt;" || ref == "&quot;" || ref == "&apos;") {
        searchPos = semiPos + 1;
        continue;
      }
      if (Default_Parser::isStandalone() && entityMapper->isFromExternalSubset(ref)) {
        XML_LIB_THROW(SyntaxError("Standalone document must not reference entity '" + ref + "' declared in external subset."));
      }
      if (entityMapper->isExternal(ref)) {
        XML_LIB_THROW(SyntaxError("Attribute values must not contain references to external entities."));
      }
      if (entityMapper->isPresent(ref)) {
        ++depth;
        const XMLValue subMapped = entityMapper->map(XMLValue{ ref, ref });
        expandedText.replace(ampPos, semiPos - ampPos + 1, subMapped.getParsed());
      } else {
        XML_LIB_THROW(SyntaxError("Entity '" + ref + "' does not exist."));
      }
    }
    unparsed += mapped.getUnparsed();
    parsed += expandedText;
    return;
  }
  unparsed += character.getUnparsed();
  parsed += character.getParsed();
}

/// @brief
/// Implementation of parseQuotedValue.

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

  return XMLValue{ unparsed, parsed, static_cast<char>(quote) };
}

void parseTextDecl(ISource &source)
{
  if (!match(source, "<?xml")) { return; }
  if (!isWS(source)) {
    XML_LIB_THROW(SyntaxError(source.getPosition(), "Whitespace required after '<?xml' in text declaration."));
  }
  ignoreWS(source);
  if (match(source, "version")) {
    ignoreWS(source);
    if (!match(source, "=")) { XML_LIB_THROW(SyntaxError(source.getPosition(), "Missing '=' after version in text declaration.")); }
    ignoreWS(source);
    const std::string ver = parseQuotedValue(source, nullptr).getParsed();
    if (ver == "1.1") {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "XML 1.0 document cannot reference an XML 1.1 entity."));
    }
    if (ver != "1.0") {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Unsupported XML version '" + ver + "' in text declaration."));
    }
    if (!isWS(source)) {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Whitespace required after version in text declaration."));
    }
    ignoreWS(source);
  }
  if (!match(source, "encoding")) {
    XML_LIB_THROW(SyntaxError(source.getPosition(), "Text declaration must contain an encoding declaration."));
  }
  ignoreWS(source);
  if (!match(source, "=")) { XML_LIB_THROW(SyntaxError(source.getPosition(), "Missing '=' after encoding in text declaration.")); }
  ignoreWS(source);
  parseQuotedValue(source, nullptr);
  ignoreWS(source);
  if (match(source, "standalone")) {
    XML_LIB_THROW(SyntaxError(source.getPosition(), "Standalone declaration not allowed in text declaration."));
  }
  if (!match(source, "?>")) {
    XML_LIB_THROW(SyntaxError(source.getPosition(), "Unclosed text declaration."));
  }
  ignoreWS(source);
}

} // namespace XML_Lib