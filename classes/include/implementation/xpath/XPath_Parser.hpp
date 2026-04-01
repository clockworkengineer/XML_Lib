#pragma once

#include "XPath_Lexer.hpp"
#include "XPath_AST.hpp"

namespace XML_Lib {

/// <summary>
/// Parse a token stream into an XPath 1.0 AST.
/// </summary>
/// <param name="tokens">Token stream from xpathTokenize().</param>
/// <returns>Root AST expression node.</returns>
XPathExprPtr xpathParse(const std::vector<XPathToken> &tokens);

}// namespace XML_Lib
