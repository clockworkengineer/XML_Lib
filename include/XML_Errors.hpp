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
        SyntaxError(const std::string &description = "")
        {
            errorMessage = "XML Syntax Error: " + description;
        }
        SyntaxError(ISource &source, const std::string &description = "")
        {
            errorMessage = "XML Syntax Error [Line: " + std::to_string(source.getLineNo()) +
                           " Column: " + std::to_string(source.getColumnNo()) + "] " + description;
        }
        virtual const char *what() const throw()
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
        ValidationError(long m_lineNumber, const std::string &description = "")
        {
            errorMessage = "XML Validation Error [Line: " + std::to_string(m_lineNumber) + "] " + description;
        }
        virtual const char *what() const throw()
        {
            return (errorMessage.c_str());
        }

    private:
        std::string errorMessage;
    };
} // namespace XMLLib