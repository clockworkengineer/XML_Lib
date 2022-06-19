#pragma once
//
// C++ STL
//
#include <string>
#include <stdexcept>
//
// Source inteface
//
#include "ISource.hpp"
// =========
// NAMESPACE
// =========
namespace XMLLib
{
    // =========
    // XML Error
    // =========
    struct Error : public std::runtime_error
    {
        Error(std::string const &message) : std::runtime_error("XML Error: " + message)
        {
        }
    };
    // ================
    // XML Syntax Error
    // ================
    struct SyntaxError : public std::runtime_error
    {
        SyntaxError(const std::string &message)
            : std::runtime_error("XML Syntax Error: " + message)
        {
        }
        explicit SyntaxError(ISource &source, const std::string &message = "")
            : std::runtime_error("XML Syntax Error [Line: " + std::to_string(source.getLineNo()) +
                                 " Column: " + std::to_string(source.getColumnNo()) + "] " + message)
        {
        }
    };
    // ====================
    // XML Validation Error
    // ====================
    struct ValidationError : public std::runtime_error
    {
        explicit ValidationError(long lineNumber, const std::string &message = "")
            : std::runtime_error("XML Validation Error [Line: " + std::to_string(lineNumber) + "] " + message)
        {
        }
    };
} // namespace XMLLib