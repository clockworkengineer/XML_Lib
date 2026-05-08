#pragma once

#include <string>
#include <string_view>
#include <exception>

namespace XML_Lib {
#ifdef XML_LIB_NO_EXCEPTIONS
struct Error { explicit Error(const std::string_view &message) : message(message) {} std::string message; };
struct SyntaxError { explicit SyntaxError(const std::string_view &message) : message(std::string("XML Syntax Error: ").append(message)) {} explicit SyntaxError(const std::pair<long, long> &position, const std::string_view &message = "") : message(std::string("XML Syntax Error [Line: " + std::to_string(position.first) + " Column: " + std::to_string(position.second) + "] ").append(message)) {} std::string message; };
struct ValidationError { explicit ValidationError(const long lineNumber, const std::string_view &message = "") : message(std::string("XML Validation Error [Line: " + std::to_string(lineNumber) + "] ").append(message)) {} std::string message; };

template<typename E> [[noreturn]] inline void xml_lib_throw(const E &error) { (void)error; std::terminate(); }
#define XML_LIB_THROW(exception) XML_Lib::xml_lib_throw(exception)
#else
#include <stdexcept>
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

template<typename E> [[noreturn]] inline void xml_lib_throw(const E &error) { (void)error; std::terminate(); }
#define XML_LIB_THROW(exception) throw exception
#endif
} // namespace XML_Lib
