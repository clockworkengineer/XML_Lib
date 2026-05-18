//
// Class: DTD_Impl_Parse_External
//
// Description: Parse external XML DTD.
//
// Dependencies: C++20 - Language standard features used.
//

#include "DTD_Impl.hpp"

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
    BufferSource conditionalDTDSource(conditionalDTD);
    parseExternalContent(conditionalDTDSource);
  } else if (conditionalValue == "IGNORE") {
    while (source.more() && !match(source, "]]")) {
      if (match(source, "<![")) {
        parseConditional(source, false);
      } else {
        source.next();
      }
    }
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
  const auto dispatch = [&](auto &&parseFn) {
    BufferSource dtdTranslatedSource(xDTD.getEntityMapper().translate(parseTagBody(source)));
    parseFn(dtdTranslatedSource);
  };
  while (source.more()) {
    if      (match(source, "<!ENTITY"))   { dispatch([&](ISource &s) { parseEntity(s);        }); }
    else if (match(source, "<!ELEMENT"))  { dispatch([&](ISource &s) { parseElement(s);       }); }
    else if (match(source, "<!ATTLIST"))  { dispatch([&](ISource &s) { parseAttributeList(s); }); }
    else if (match(source, "<!NOTATION")) { dispatch([&](ISource &s) { parseNotation(s);      }); }
    else if (match(source, "<!--")) {
      parseComment(source);
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
  if (xDTD.getExternalReference().getType() == "SYSTEM") {
    FileSource dtdFile(xDTD.getExternalReference().getSystemID());
    parseExternalContent(dtdFile);
  } else if (xDTD.getExternalReference().getType() == XMLExternalReference::kPublicID) {
    // Public external DTD currently not supported (Use system id ?)
  }
}

/// <summary>
/// Parse an external reference.
/// </summary>
/// <param name="source">DTD source stream.</param>
/// <returns>External reference.</returns>
XMLExternalReference DTD_Impl::parseExternalReference(ISource &source) const
{
  if (match(source, "SYSTEM")) {
    ignoreWS(source);
    return XMLExternalReference{ "SYSTEM", parseValue(source, xDTD.getEntityMapper()).getParsed(), "" };
  }
  if (match(source, XMLExternalReference::kPublicID)) {
    ignoreWS(source);
    const std::string publicID{ parseValue(source, xDTD.getEntityMapper()).getParsed() };
    const std::string systemID{ parseValue(source, xDTD.getEntityMapper()).getParsed() };
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
