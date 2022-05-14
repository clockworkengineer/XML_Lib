#pragma once
//
// C++ STL
//
#include <stdexcept>
#include <filesystem>
#include <map>
#include <set>
#include <sstream>
//
// Source interface
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
    // struct SyntaxError : public std::exception
    // {
    // public:
    //     SyntaxError(const std::string &description = "")
    //     {
    //         errorMessage = "XML Syntax Error: " + description;
    //     }
    //     SyntaxError(ISource &source, const std::string &description = "")
    //     {
    //         errorMessage = "XML Syntax Error [Line: " + std::to_string(source.getLineNo()) +
    //                        " Column: " + std::to_string(source.getColumnNo()) + "] " + description;
    //     }
    //     virtual const char *what() const throw()
    //     {
    //         return (errorMessage.c_str());
    //     }

    // private:
    //     std::string errorMessage;
    // };
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
        virtual std::string translate(const std::string &toTranslate, char type = '%') const = 0;
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
    XMLValue parseEntityReference(ISource &source);
    std::string parseName(ISource &source);
    XMLValue parseCharacterReference(ISource &source);
    XMLValue parseCharacter(ISource &source);
    XMLValue parseValue(ISource &source);
    XMLValue parseValue(ISource &source, IXMLEntityMapper &entityMapper);
    std::string parseTagBody(ISource &source);
    //
    // XML utility
    //
    std::vector<std::string> splitString(std::string stringToSplit, char delimeter);
    void trimmString(std::string &stringToTrimm);
    void toUpperString(std::string &stringToUpper);
} // namespace XMLLib
//
// XML XNodes
//
#include "XML_nodes.hpp"
