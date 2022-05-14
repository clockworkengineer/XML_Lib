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
//
// XML types/nodes
//
#include "XML_types.hpp"
#include "XML_nodes.hpp"
// =========
// NAMESPACE
// =========
namespace XMLLib
{
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
