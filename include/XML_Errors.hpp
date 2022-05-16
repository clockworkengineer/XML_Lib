#pragma once
//
// C++ STL
//
#include <string>
#include <stdexcept>
//
// ISource inteface
//
#include "ISource.hpp"
// =========
// NAMESPACE
// =========
namespace XMLLib
{
    //
    // XML syntax error
    //
    struct SyntaxError : public std::exception
    {
    public:
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
    //
    // XML validation error
    //
    struct ValidationError : public std::exception
    {
    public:
        explicit ValidationError(long m_lineNumber, const std::string &description = "")
        {
            errorMessage = "XML Validation Error [Line: " + std::to_string(m_lineNumber) + "] " + description;
        }
        [[nodiscard]] const char *what() const noexcept override
        {
            return (errorMessage.c_str());
        }

    private:
        std::string errorMessage;
    };
} // namespace XMLLib