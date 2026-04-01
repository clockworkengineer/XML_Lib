#pragma once

namespace XML_Lib {

// =====================================================
// XPath token types
// =====================================================
enum class XPathTokenType : uint8_t {
  Slash,// /
  DoubleSlash,// //
  Dot,// .
  DotDot,// ..
  At,// @
  ColonColon,// ::
  LeftBracket,// [
  RightBracket,// ]
  LeftParen,// (
  RightParen,// )
  Comma,// ,
  Pipe,// |
  Star,// *
  Plus,// +
  Minus,// -
  Eq,// =
  Neq,// !=
  Lt,// <
  Gt,// >
  LtEq,// <=
  GtEq,// >=
  Name,// identifier or keyword
  StringLiteral,// "..." or '...'
  NumberLiteral,// digits
  End// end of input
};

struct XPathToken
{
  XPathTokenType type{ XPathTokenType::End };
  std::string value;// populated for Name, StringLiteral, NumberLiteral
};

/// <summary>
/// Tokenize an XPath 1.0 expression into a flat token stream.
/// </summary>
/// <param name="expression">XPath expression string.</param>
/// <returns>Vector of tokens ending with an End token.</returns>
std::vector<XPathToken> xpathTokenize(std::string_view expression);

}// namespace XML_Lib
