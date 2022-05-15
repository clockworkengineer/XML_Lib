#pragma once
//
// C++ STL
//
#include <set>
#include <map>
//
// ISource interface
//
#include "ISource.hpp"
//
// XML types
//
#include "XML_Types.hpp"
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
} // namespace XMLLib