#pragma once

#include <string>
#include <stdexcept>

namespace XML_Lib {

// XML error types
struct Error : public std::runtime_error
{
  explicit Error(const std::string &message) : std::runtime_error("XML Error: " + message) {}
};
struct SyntaxError : public std::runtime_error
{
  explicit SyntaxError(const std::string &message) : std::runtime_error("XML Syntax Error: " + message) {}
  explicit SyntaxError(const std::pair<long, long> &position, const std::string &message = "")
    : std::runtime_error("XML Syntax Error [Line: " + std::to_string(position.first)
                         + " Column: " + std::to_string(position.second) + "] " + message)
  {}
};
struct ValidationError : public std::runtime_error
{
  explicit ValidationError(long lineNumber, const std::string &message = "")
    : std::runtime_error("XML Validation Error [Line: " + std::to_string(lineNumber) + "] " + message)
  {}
};
}// namespace XML_Lib