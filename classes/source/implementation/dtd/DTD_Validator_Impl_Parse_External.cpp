//
// Class: DTD_Impl_Parse_External
//
// Description: Parse external XML DTD.
//
// Dependencies: C++20 - Language standard features used.
//

#include "DTD_Impl.hpp"
#include "XML_ParseHelpers.hpp"
#include "implementation/io/XML_FileIO.hpp"

namespace XML_Lib {

/// <summary>
/// Parse conditional DTD (recursively if necessary).
/// </summary>
/// <param name="source">DTD source stream.</param>
/// <param name="includeOn">If set to false then enclosing conditionals treated as ignored.</param>
void DTD_Impl::parseConditional(ISource &source, const bool includeOn)
{
  std::string conditionalValue;
  ignoreWS(source);
  if (includeOn) {
    if (source.current() == '%') {
      conditionalValue = xDTD.getEntityMapper().map(parseEntityReference(source)).getParsed();
    } else if (match(source, "INCLUDE")) {
      conditionalValue = "INCLUDE";
    } else if (match(source, "IGNORE")) {
      conditionalValue = "IGNORE";
    }
  } else {
    conditionalValue = "IGNORE";
  }
  ignoreWS(source);
  if (conditionalValue == "INCLUDE") {
    if (source.current() != '[') { XML_LIB_THROW(SyntaxError(source.getPosition(), "Missing opening '[' from conditional.")); }
    source.next();
    ignoreWS(source);
    std::string conditionalDTD;
    while (source.more() && !match(source, "]]")) {
      if (match(source, "<![")) {
        parseConditional(source);
      } else {
        conditionalDTD += toUtf8(source.current());
        source.next();
      }
    }
    if (conditionalDTD.find_first_not_of(" \t\r\n") != std::string::npos) {
      BufferSource conditionalDTDSource(conditionalDTD);
      parseExternalContent(conditionalDTDSource);
    }
  } else if (conditionalValue == "IGNORE") {
    if (source.current() != '[') { XML_LIB_THROW(SyntaxError(source.getPosition(), "Missing opening '[' from conditional.")); }
    source.next();
    int depth = 1;
    while (source.more() && depth > 0) {
      if (match(source, "<![")) {
        ++depth;
      } else if (match(source, "]]>")) {
        --depth;
      } else {
        source.next();
      }
    }
    if (depth != 0) {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Unclosed IGNORE conditional section."));
    }
    ignoreWS(source);
    return;
  } else {
    XML_LIB_THROW(SyntaxError(source.getPosition(), "Conditional value not INCLUDE or IGNORE."));
  }
  if (source.current() != '>') { XML_LIB_THROW(SyntaxError(source.getPosition(), "Missing '>' terminator.")); }
  source.next();
  ignoreWS(source);
}

/// <summary>
/// Parse external DTD.
/// </summary>
/// <param name="source">DTD source stream.</param>
void DTD_Impl::parseExternalContent(ISource &source)
{
  ignoreWS(source);
  // Optional TextDecl at start of external entity: <?xml ... ?>
  parseTextDecl(source);

  const auto dispatch = [&](auto &&parseFn) {
    BufferSource dtdTranslatedSource(xDTD.getEntityMapper().translate(parseTagBody(source)));
    parseFn(dtdTranslatedSource);
  };
  while (source.more()) {
    ignoreWS(source);
    if (!source.more()) {
      break;
    }
    if      (match(source, "<!ENTITY"))   { dispatch([&](ISource &s) { parseEntity(s);        }); }
    else if (match(source, "<!ELEMENT"))  { dispatch([&](ISource &s) { parseElement(s);       }); }
    else if (match(source, "<!ATTLIST"))  { dispatch([&](ISource &s) { parseAttributeList(s); }); }
    else if (match(source, "<!NOTATION")) { dispatch([&](ISource &s) { parseNotation(s);      }); }
    else if (match(source, "<!--")) {
      parseComment(source);
    } else if (match(source, "<?")) {
      parsePI(source);
      ignoreWS(source);
      continue;
    } else if (source.current() == '%') {
      parseParameterEntityReference(source);
      continue;
    } else if (match(source, "<![")) {
      parseConditional(source);
      continue;
    } else {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Invalid DTD tag."));
    }
    if (source.current() != '>') { XML_LIB_THROW(SyntaxError(source.getPosition(), "Missing '>' terminator.")); }
    source.next();
    ignoreWS(source);
  }
}

/// <summary>
/// Parse externally defined DTD.
/// </summary>
void DTD_Impl::parseExternalReferenceContent()
{
  if (xDTD.getExternalReference().getType() == "SYSTEM" ||
      (xDTD.getExternalReference().getType() == XMLExternalReference::kPublicID &&
       !xDTD.getExternalReference().getSystemID().empty())) {
    std::filesystem::path dtdPath{ xDTD.getExternalReference().getSystemID() };
    if (dtdPath.is_relative() && !baseDirectory.empty()) {
      dtdPath = baseDirectory / dtdPath;
    }
    if (std::filesystem::exists(dtdPath)) {
      const std::string dtdString = XML_FileIO::fromFile(dtdPath);
      if (!dtdString.empty() && dtdString.find_first_not_of(" \t\r\n") != std::string::npos) {
        BufferSource dtdFile(dtdString, BufferSource::kMaxSourceBytes, dtdPath.string());
        parseExternalContent(dtdFile);
      }
    }
  }
}

/// <summary>
/// Parse an external reference.
/// </summary>
/// <param name="source">DTD source stream.</param>
/// <returns>External reference.</returns>
namespace {
bool isValidPubid(const std::string_view &pubid)
{
  for (char c : pubid) {
    if (std::isalnum(static_cast<unsigned char>(c))) continue;
    if (c == ' ' || c == '\r' || c == '\n') continue;
    if (std::string_view("-'()+,./:=?;!*#@$_%").find(c) != std::string_view::npos) continue;
    return false;
  }
  return true;
}
} // namespace

XMLExternalReference DTD_Impl::parseExternalReference(ISource &source, bool systemIdRequired) const
{
  if (match(source, "SYSTEM")) {
    if (!source.more() || !isWS(source.current())) {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Missing whitespace after SYSTEM."));
    }
    ignoreWS(source);
    return XMLExternalReference{ "SYSTEM", parseValue(source, xDTD.getEntityMapper()).getParsed(), "" };
  }
  if (match(source, XMLExternalReference::kPublicID)) {
    if (!source.more() || !isWS(source)) {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Missing whitespace after PUBLIC."));
    }
    ignoreWS(source);
    const Char quote = source.current();
    if (quote != '\'' && quote != '"') {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Invalid public identifier quote."));
    }
    source.next();
    std::string publicID;
    while (source.more() && source.current() != quote) {
      publicID += source.current();
      source.next();
    }
    if (source.current() != quote) {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Missing closing quote on public identifier."));
    }
    source.next();
    if (!isValidPubid(publicID)) {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Invalid character in public identifier."));
    }
    if (source.more() && (source.current() == '\'' || source.current() == '"')) {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Missing whitespace between public identifier and system identifier."));
    }
    ignoreWS(source);
    std::string systemID;
    if (source.more() && (source.current() == '\'' || source.current() == '"')) {
      systemID = parseValue(source, xDTD.getEntityMapper()).getParsed();
    }
    if (systemIdRequired && systemID.empty()) {
      XML_LIB_THROW(SyntaxError(source.getPosition(), "Missing system identifier after public identifier."));
    }
    return XMLExternalReference{ XMLExternalReference::kPublicID, systemID, publicID };
  }
  XML_LIB_THROW(SyntaxError(source.getPosition(), "Invalid external DTD specifier."));
}

/// <summary>
/// Parse externally defined DTD.
/// </summary>
/// <param name="source">DTD source stream.</param>
void DTD_Impl::parseExternal([[maybe_unused]] ISource & source) { parseExternalReferenceContent(); }
}// namespace XML_Lib
