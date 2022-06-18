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
    struct Error : public std::exception
    {
        explicit Error(const std::string &description = "")
        {
            errorMessage = "XML Error: " + description;
        }
        [[nodiscard]] const char *what() const noexcept override
        {
            return (errorMessage.c_str());
        }
    private:
        std::string errorMessage;
    };
    // ================
    // XML Syntax Error
    // ================
    struct SyntaxError : public std::exception
    {
        explicit SyntaxError(const std::string &description = "")
        {
            errorMessage = "XML Syntax Error: " + description;
        }
        explicit SyntaxError(ISource &source, const std::string &description = "")
        {
            errorMessage = "XML Syntax Error [Line: " + std::to_string(source.getLineNo()) +
                           " Column: " + std::to_string(source.getColumnNo()) + "] " + description;
        }
        [[nodiscard]] const char *what() const noexcept override
        {
            return (errorMessage.c_str());
        }
    private:
        std::string errorMessage;
    };
    // ====================
    // XML Validation Error
    // ====================
    struct ValidationError : public std::exception
    {
        explicit ValidationError(long lineNumber, const std::string &description = "")
        {
            errorMessage = "XML Validation Error [Line: " + std::to_string(lineNumber) + "] " + description;
        }
        [[nodiscard]] const char *what() const noexcept override
        {
            return (errorMessage.c_str());
        }
    private:
        std::string errorMessage;
    };
} // namespace XMLLib