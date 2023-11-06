//
// Class: DTD_impl
//
// Description: Parse external XML DTD.
//
// Dependencies:   C++20 - Language standard features used.
//

#include "DTD_Impl.hpp"

namespace XML_Lib {

/// <summary>
/// Parse conditional DTD (recursively if necessary).
/// </summary>
/// <param name="source">DTD source stream.</param>
/// <param name="includeOn">If set to false then enclosing conditionals treated as ignored.</param>
void DTD_Impl::parseConditional(ISource &source, bool includeOn)
{
  std::string conditionalValue;
  source.ignoreWS();
  if (includeOn) {
    if (source.current() == '%') {
      conditionalValue = xDTD.getEntityMapper().map(parseEntityReference(source)).getParsed();
    } else if (source.match(U"INCLUDE")) {
      conditionalValue = "INCLUDE";
    } else if (source.match(U"IGNORE")) {
      conditionalValue = "IGNORE";
    }
  } else {
    conditionalValue = "IGNORE";
  }
  source.ignoreWS();
  if (conditionalValue == "INCLUDE") {
    if (source.current() != '[') { throw SyntaxError(source.getPosition(), "Missing opening '[' from conditional."); }
    source.next();
    source.ignoreWS();
    std::string conditionalDTD;
    while (source.more() && !source.match(U"]]")) {
      if (source.match(U"<![")) {
        parseConditional(source);
      } else {
        conditionalDTD += source.current_to_bytes();
        source.next();
      }
    }
    BufferSource conditionalDTDSource(conditionalDTD);
    parseExternalContent(conditionalDTDSource);
  } else if (conditionalValue == "IGNORE") {
    while (source.more() && !source.match(U"]]")) {
      if (source.match(U"<![")) {
        parseConditional(source, false);
      } else {
        source.next();
      }
    }
  } else {
    throw SyntaxError(source.getPosition(), "Conditional value not INCLUDE or IGNORE.");
  }
  if (source.current() != '>') { throw SyntaxError(source.getPosition(), "Missing '>' terminator."); }
  source.next();
  source.ignoreWS();
}

/// <summary>
/// Parse external DTD.
/// </summary>
/// <param name="source">DTD source stream.</param>
void DTD_Impl::parseExternalContent(ISource &source)
{
  while (source.more()) {
    if (source.match(U"<!ENTITY")) {
      BufferSource dtdTranslatedSource(xDTD.getEntityMapper().translate(parseTagBody(source)));
      parseEntity(dtdTranslatedSource);
    } else if (source.match(U"<!ELEMENT")) {
      BufferSource dtdTranslatedSource(xDTD.getEntityMapper().translate(parseTagBody(source)));
      parseElement(dtdTranslatedSource);
    } else if (source.match(U"<!ATTLIST")) {
      BufferSource dtdTranslatedSource(xDTD.getEntityMapper().translate(parseTagBody(source)));
      parseAttributeList(dtdTranslatedSource);
    } else if (source.match(U"<!NOTATION")) {
      BufferSource dtdTranslatedSource(xDTD.getEntityMapper().translate(parseTagBody(source)));
      parseNotation(dtdTranslatedSource);
    } else if (source.match(U"<!--")) {
      parseComment(source);
    } else if (source.current() == '%') {
      parseParameterEntityReference(source);
      continue;
    } else if (source.match(U"<![")) {
      parseConditional(source);
      continue;
    } else {
      throw SyntaxError(source.getPosition(), "Invalid DTD tag.");
    }
    if (source.current() != '>') { throw SyntaxError(source.getPosition(), "Missing '>' terminator."); }
    source.next();
    source.ignoreWS();
  }
}

/// <summary>
/// Parse externally defined DTD into XDTD.
/// </summary>
void DTD_Impl::parseExternalReferenceContent()
{
  if (xDTD.getExternalReference().getType() == "SYSTEM") {
    FileSource dtdFile(xDTD.getExternalReference().getSystemID());
    parseExternalContent(dtdFile);
  } else if (xDTD.getExternalReference().getType() == "PUBLIC") {
    // Public external DTD currently not supported (Use system id ?)
  }
}

/// <summary>
/// Parse an external reference.
/// </summary>
/// <param name="source">DTD source stream.</param>
/// <returns>External reference.</returns>
XMLExternalReference DTD_Impl::parseExternalReference(ISource &source)
{
  if (source.match(U"SYSTEM")) {
    source.ignoreWS();
    return (XMLExternalReference{ "SYSTEM", parseValue(source, xDTD.getEntityMapper()).getParsed(), "" });
  } else if (source.match(U"PUBLIC")) {
    source.ignoreWS();
    std::string publicID{ parseValue(source, xDTD.getEntityMapper()).getParsed() };
    std::string systemID{ parseValue(source, xDTD.getEntityMapper()).getParsed() };
    return (XMLExternalReference{ "PUBLIC", systemID, publicID });
  }
  throw SyntaxError(source.getPosition(), "Invalid external DTD specifier.");
}

/// <summary>
/// Parse externally defined DTD.
/// </summary>
/// <param name="source">DTD source stream.</param>
void DTD_Impl::parseExternal(ISource & /*source*/) { parseExternalReferenceContent(); }
}// namespace XML_Lib
