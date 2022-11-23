#pragma once
// =======
// C++ STL
// =======
#include <string>
#include <stdexcept>
// =================
// LIBRARY NAMESPACE
// =================
namespace XML_Lib {
// =========
// XML Error
// =========
struct Error : public std::runtime_error
{
  Error(std::string const &message) : std::runtime_error("XML Error: " + message) {}
};
// ================
// XML Syntax Error
// ================
struct SyntaxError : public std::runtime_error
{
  SyntaxError(const std::string &message) : std::runtime_error("XML Syntax Error: " + message) {}
  explicit SyntaxError(const std::pair<long, long> &position, const std::string &message = "")
    : std::runtime_error("XML Syntax Error [Line: " + std::to_string(position.first)
                         + " Column: " + std::to_string(position.second) + "] " + message)
  {}
};
// ====================
// XML Validation Error
// ====================
struct ValidationError : public std::runtime_error
{
  explicit ValidationError(long lineNumber, const std::string &message = "")
    : std::runtime_error("XML Validation Error [Line: " + std::to_string(lineNumber) + "] " + message)
  {}
};
}// namespace XML_Lib