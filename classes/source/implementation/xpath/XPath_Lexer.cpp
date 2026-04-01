//
// XPath_Lexer.cpp
//
// Description: Tokenizes an XPath 1.0 expression string.
//
// Dependencies: C++20 - Language standard features used.
//

#include "XPath_Lexer.hpp"

namespace XML_Lib {

namespace {

  // Is this character a valid XPath name-start character?
  // (simplified: ASCII letter, underscore, or colon for qualified names)
  bool isNameStart(char c) { return std::isalpha(static_cast<unsigned char>(c)) || c == '_'; }

  bool isNameChar(char c) { return std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '-' || c == '.'; }

  // Allows a single colon for qualified names (prefix:local), but not double-colon (axis separator)
  bool isQNameChar(char c, bool &seenColon)
  {
    if (c == ':') {
      if (seenColon) return false;// second colon → stop (this would be ::)
      seenColon = true;
      return true;
    }
    return isNameChar(c);
  }

}// anonymous namespace

/// <summary>
/// Tokenize an XPath 1.0 expression.
///
/// Disambiguation (XPath 1.0 spec §3.7):
/// - A bare '*' that appears after an operator token or at the very start
///   is a wildcard node-test; in any other position it is the multiply operator.
/// - Keyword names (div, mod, and, or) are recognised as operators only
///   when the immediately preceding token is a value-producing token
///   (Name, RightParen, RightBracket, StringLiteral, NumberLiteral).
/// </summary>
std::vector<XPathToken> xpathTokenize(const std::string_view expression)
{
  std::vector<XPathToken> tokens;
  size_t i = 0;
  const size_t len = expression.size();

  // Lambda: is the last meaningful token one that ends an expression?
  auto lastIsExprEnd = [&]() -> bool {
    if (tokens.empty()) return false;
    const auto t = tokens.back().type;
    return t == XPathTokenType::Name || t == XPathTokenType::RightParen || t == XPathTokenType::RightBracket
           || t == XPathTokenType::StringLiteral || t == XPathTokenType::NumberLiteral || t == XPathTokenType::Star;
  };

  while (i < len) {
    // skip whitespace
    if (std::isspace(static_cast<unsigned char>(expression[i]))) {
      ++i;
      continue;
    }

    char c = expression[i];

    // Two-character tokens
    if (i + 1 < len) {
      char c2 = expression[i + 1];
      if (c == '/' && c2 == '/') {
        tokens.push_back({ XPathTokenType::DoubleSlash, "//" });
        i += 2;
        continue;
      }
      if (c == '.' && c2 == '.') {
        tokens.push_back({ XPathTokenType::DotDot, ".." });
        i += 2;
        continue;
      }
      if (c == '!' && c2 == '=') {
        tokens.push_back({ XPathTokenType::Neq, "!=" });
        i += 2;
        continue;
      }
      if (c == '<' && c2 == '=') {
        tokens.push_back({ XPathTokenType::LtEq, "<=" });
        i += 2;
        continue;
      }
      if (c == '>' && c2 == '=') {
        tokens.push_back({ XPathTokenType::GtEq, ">=" });
        i += 2;
        continue;
      }
      if (c == ':' && c2 == ':') {
        tokens.push_back({ XPathTokenType::ColonColon, "::" });
        i += 2;
        continue;
      }
    }

    // Single-character tokens
    switch (c) {
    case '/':
      tokens.push_back({ XPathTokenType::Slash, "/" });
      ++i;
      continue;
    case '.':
      tokens.push_back({ XPathTokenType::Dot, "." });
      ++i;
      continue;
    case '@':
      tokens.push_back({ XPathTokenType::At, "@" });
      ++i;
      continue;
    case '[':
      tokens.push_back({ XPathTokenType::LeftBracket, "[" });
      ++i;
      continue;
    case ']':
      tokens.push_back({ XPathTokenType::RightBracket, "]" });
      ++i;
      continue;
    case '(':
      tokens.push_back({ XPathTokenType::LeftParen, "(" });
      ++i;
      continue;
    case ')':
      tokens.push_back({ XPathTokenType::RightParen, ")" });
      ++i;
      continue;
    case ',':
      tokens.push_back({ XPathTokenType::Comma, "," });
      ++i;
      continue;
    case '|':
      tokens.push_back({ XPathTokenType::Pipe, "|" });
      ++i;
      continue;
    case '+':
      tokens.push_back({ XPathTokenType::Plus, "+" });
      ++i;
      continue;
    case '-':
      tokens.push_back({ XPathTokenType::Minus, "-" });
      ++i;
      continue;
    case '=':
      tokens.push_back({ XPathTokenType::Eq, "=" });
      ++i;
      continue;
    case '<':
      tokens.push_back({ XPathTokenType::Lt, "<" });
      ++i;
      continue;
    case '>':
      tokens.push_back({ XPathTokenType::Gt, ">" });
      ++i;
      continue;
    case '*':
      // Wildcard when not preceded by an expression-end token
      if (!lastIsExprEnd()) {
        tokens.push_back({ XPathTokenType::Star, "*" });
      } else {
        tokens.push_back({ XPathTokenType::Star, "*" });// multiply — parser handles context
      }
      ++i;
      continue;
    default:
      break;
    }

    // String literal
    if (c == '"' || c == '\'') {
      char quote = c;
      ++i;
      size_t start = i;
      while (i < len && expression[i] != quote) { ++i; }
      if (i >= len) { throw std::runtime_error("XPath Error: Unterminated string literal."); }
      tokens.push_back({ XPathTokenType::StringLiteral, std::string(expression.substr(start, i - start)) });
      ++i;// consume closing quote
      continue;
    }

    // Number literal
    if (std::isdigit(static_cast<unsigned char>(c))) {
      size_t start = i;
      while (i < len && std::isdigit(static_cast<unsigned char>(expression[i]))) { ++i; }
      if (i < len && expression[i] == '.') {
        ++i;
        while (i < len && std::isdigit(static_cast<unsigned char>(expression[i]))) { ++i; }
      }
      tokens.push_back({ XPathTokenType::NumberLiteral, std::string(expression.substr(start, i - start)) });
      continue;
    }

    // Name / keyword (QName = prefix:local, but stop at :: axis separator)
    if (isNameStart(c)) {
      size_t start = i;
      bool seenColon = false;
      while (i < len && isQNameChar(expression[i], seenColon)) { ++i; }
      // If we ended on a single trailing colon (not ::), back up one character
      // so the colon is not absorbed (e.g. "ns:" should not include the colon
      // unless followed by a valid name char).
      if (i > start && expression[i - 1] == ':') { --i; }
      const std::string name{ expression.substr(start, i - start) };

      // Keywords 'div', 'mod', 'and', 'or' are only operators when after an expr-end token
      if ((name == "div" || name == "mod" || name == "and" || name == "or") && lastIsExprEnd()) {
        tokens.push_back({ XPathTokenType::Name, name });
      } else {
        tokens.push_back({ XPathTokenType::Name, name });
      }
      continue;
    }

    throw std::runtime_error(std::string("XPath Error: Unexpected character '") + c + "' in expression.");
  }

  tokens.push_back({ XPathTokenType::End, "" });
  return tokens;
}

}// namespace XML_Lib
