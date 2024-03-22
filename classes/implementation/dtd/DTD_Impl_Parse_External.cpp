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
      conditionalValue = dtdRoot.getEntityMapper().map(parseEntityReference(source)).getParsed();
    } else if (source.match("INCLUDE")) {
      conditionalValue = "INCLUDE";
    } else if (source.match("IGNORE")) {
      conditionalValue = "IGNORE";
    }
  } else {
    conditionalValue = "IGNORE";
  }
  source.ignoreWS();
  if (conditionalValue == "INCLUDE") {
    if (source.current() != '[') {
      throw XML::SyntaxError(source.getPosition(), "Missing opening '[' from conditional.");
    }
    source.next();
    source.ignoreWS();
    std::string conditionalDTD;
    while (source.more() && !source.match("]]")) {
      if (source.match("<![")) {
        parseConditional(source);
      } else {
        conditionalDTD += toUtf8(source.current());
        source.next();
      }
    }
    BufferSource conditionalDTDSource(conditionalDTD);
    parseExternalContent(conditionalDTDSource);
  } else if (conditionalValue == "IGNORE") {
    while (source.more() && !source.match("]]")) {
      if (source.match("<![")) {
        parseConditional(source, false);
      } else {
        source.next();
      }
    }
  } else {
    throw XML::SyntaxError(source.getPosition(), "Conditional value not INCLUDE or IGNORE.");
  }
  if (source.current() != '>') { throw XML::SyntaxError(source.getPosition(), "Missing '>' terminator."); }
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
    if (source.match("<!ENTITY")) {
      BufferSource dtdTranslatedSource(dtdRoot.getEntityMapper().translate(parseTagBody(source)));
      parseEntity(dtdTranslatedSource);
    } else if (source.match("<!ELEMENT")) {
      BufferSource dtdTranslatedSource(dtdRoot.getEntityMapper().translate(parseTagBody(source)));
      parseElement(dtdTranslatedSource);
    } else if (source.match("<!ATTLIST")) {
      BufferSource dtdTranslatedSource(dtdRoot.getEntityMapper().translate(parseTagBody(source)));
      parseAttributeList(dtdTranslatedSource);
    } else if (source.match("<!NOTATION")) {
      BufferSource dtdTranslatedSource(dtdRoot.getEntityMapper().translate(parseTagBody(source)));
      parseNotation(dtdTranslatedSource);
    } else if (source.match("<!--")) {
      parseComment(source);
    } else if (source.current() == '%') {
      parseParameterEntityReference(source);
      continue;
    } else if (source.match("<![")) {
      parseConditional(source);
      continue;
    } else {
      throw XML::SyntaxError(source.getPosition(), "Invalid DTD tag.");
    }
    if (source.current() != '>') { throw XML::SyntaxError(source.getPosition(), "Missing '>' terminator."); }
    source.next();
    source.ignoreWS();
  }
}

/// <summary>
/// Parse externally defined DTD into XDTD.
/// </summary>
void DTD_Impl::parseExternalReferenceContent()
{
  if (dtdRoot.getExternalReference().getType() == "SYSTEM") {
    FileSource dtdFile(dtdRoot.getExternalReference().getSystemID());
    parseExternalContent(dtdFile);
  } else if (dtdRoot.getExternalReference().getType() == "PUBLIC") {
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
  if (source.match("SYSTEM")) {
    source.ignoreWS();
    return (XMLExternalReference{ "SYSTEM", parseValue(source, dtdRoot.getEntityMapper()).getParsed(), "" });
  } else if (source.match("PUBLIC")) {
    source.ignoreWS();
    std::string publicID{ parseValue(source, dtdRoot.getEntityMapper()).getParsed() };
    std::string systemID{ parseValue(source, dtdRoot.getEntityMapper()).getParsed() };
    return (XMLExternalReference{ "PUBLIC", systemID, publicID });
  }
  throw XML::SyntaxError(source.getPosition(), "Invalid external DTD specifier.");
}

/// <summary>
/// Parse externally defined DTD.
/// </summary>
/// <param name="source">DTD source stream.</param>
void DTD_Impl::parseExternal(ISource & /*source*/) { parseExternalReferenceContent(); }
}// namespace XML_Lib
