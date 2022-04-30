#pragma once
//
// C++ STL
//
#include <stdexcept>
#include <filesystem>
#include <algorithm>
#include <map>
#include <set>
//
// XML character constants
//
#define kCarriageReturn 0x0D
#define kLineFeed 0x0A
//
// Source/Destination interfaces
//
#include "ISource.hpp"
#include "IDestination.hpp"
//
// XML Sources
//
#include "XML_sources.hpp"
// =========
// NAMESPACE
// =========
namespace H4
{
    //
    // XML syntax error
    //
    struct XMLSyntaxError : public std::exception
    {
    public:
        XMLSyntaxError(const std::string &description = "")
        {
            errorMessage = "XML Syntax Error: " + description;
        }
        XMLSyntaxError(ISource &xmlSource, const std::string &description = "")
        {
            errorMessage = "XML Syntax Error [Line: " + std::to_string(xmlSource.getLineNo()) +
                           " Column: " + std::to_string(xmlSource.getColumnNo()) + "] " + description;
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
    struct XMLValidationError : public std::exception
    {
    public:
        XMLValidationError(long m_lineNumber, const std::string &description = "")
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
    //
    // XML value
    //
    struct XMLValue
    {
    public:
        std::string unparsed;
        std::string parsed;
        XMLValue operator+=(const XMLValue &rhs)
        {
            parsed += rhs.parsed;
            unparsed += rhs.unparsed;
            return (*this);
        }
        bool isEntityReference() const
        {
            return (unparsed[0] == '&' && unparsed[1] != '#' && unparsed.back() == ';');
        }
        bool isCharacterReference() const
        {
            return (unparsed[0] == '&' && unparsed[1] == '#' && unparsed.back() == ';');
        }
    };
    //
    // XML Attribute
    //
    struct XMLAttribute
    {
    public:
        XMLAttribute() {}
        XMLAttribute(const std::string &name, const XMLValue &value) : name(name), value(value)
        {
        }
        std::string name;
        XMLValue value;
    };
    //
    // XML External reference
    //
    struct XMLExternalReference
    {
    public:
        std::string type;
        std::string systemID;
        std::string publicID;
    };
    //
    // XML Entity mapping
    //
    struct XMLEntityMapping
    {
    public:
        std::string internal;
        XMLExternalReference external;
        std::string notation;
    };
    //
    // XML Entity mapper interface
    //
    class IXMLEntityMapper
    {
    public:
        virtual ~IXMLEntityMapper() {}
        virtual void add(const std::string &entityName, XMLEntityMapping &entityMapping) = 0;
        virtual XMLEntityMapping &get(const std::string &entityName) = 0;
        virtual void remove(const std::string &entityName) = 0;
        virtual bool isPresent(const std::string &entityName) const = 0;
        virtual std::map<std::string, XMLEntityMapping> &getList() = 0;
        virtual void map(XMLValue &entityReference) = 0;
        virtual std::string translate(const std::string &toTranslate, char type = '%') const  = 0;
        virtual void recursive(const std::string &entityName, ISource::Char type, std::set<std::string> names = {}) = 0;
    };
    //
    // XML validator interface
    //
    struct XMLNodeElement;
    class IXMLValidator
    {
    public:
        virtual ~IXMLValidator() {}
        virtual void validate(XMLNodeElement &prolog) = 0;
    };
    //
    // XML validation
    //
    bool validChar(ISource::Char ch);
    bool validNameStartChar(ISource::Char ch);
    bool validNameChar(ISource::Char ch);
    bool validReservedName(const ISource::String &name);
    bool validName(ISource::String name);
    bool validAttributeValue(XMLValue &value);
    //
    // XML parsing
    //
    XMLValue parseEntityReference(ISource &xmlSource);
    std::string parseName(ISource &xmlSource);
    XMLValue parseCharacterReference(ISource &xmlSource);
    XMLValue parseCharacter(ISource &xmlSource);
    XMLValue parseValue(ISource &xmlSource);
    XMLValue parseValue(ISource &xmlSource, IXMLEntityMapper &entityMapper);
    std::string parseTagBody(ISource &xmlSource);
    //
    // XML utility
    //
    std::vector<std::string> splitString(std::string stringToSplit, char delimeter);
    void trimmString(std::string &stringToTrimm);
    void toUpperString(std::string &stringToUpper);
} // namespace H4
//
// XML XNodes
//
#include "XML_nodes.hpp"
