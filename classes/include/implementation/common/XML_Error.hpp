#pragma once

namespace XML_Lib {

// XML error types
struct Error final : std::runtime_error
{
  explicit Error(const std::string_view &message) : std::runtime_error(std::string("XML Error: ").append(message)) {}
};
struct SyntaxError final : std::runtime_error
{
  explicit SyntaxError(const std::string_view &message) : std::runtime_error(std::string("XML Syntax Error: ").append(message)) {}
  explicit SyntaxError(const std::pair<long, long> &position, const std::string_view &message = "")
    : std::runtime_error(std::string("XML Syntax Error [Line: " + std::to_string(position.first)
                         + " Column: " + std::to_string(position.second)+"] ").append(message))
  {}
};
struct ValidationError final : std::runtime_error
{
  explicit ValidationError(const long lineNumber, const std::string_view &message = "")
    : std::runtime_error(std::string("XML Validation Error [Line: " + std::to_string(lineNumber) + "] ").append(message))
  {}
};
}// namespace XML_Lib