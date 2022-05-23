#pragma once
//
// C++ STL
//
#include <string>
#include <utility>
// =========
// NAMESPACE
// =========
namespace XMLLib
{
    //
    // XML value
    //
    struct XMLValue
    {
    public:
        XMLValue(const std::string &unparsed, const std::string &parsed="") : unparsed(unparsed), parsed(parsed)
        {
        }
        [[nodiscard]] bool isEntityReference() const
        {
            return (unparsed[0] == '&' && unparsed[1] != '#' && unparsed.back() == ';');
        }
        [[nodiscard]] bool isCharacterReference() const
        {
            return (unparsed[0] == '&' && unparsed[1] == '#' && unparsed.back() == ';');
        }
        std::string unparsed;
        std::string parsed;
    };
    //
    // XML Attribute
    //
    struct XMLAttribute
    {
    public:
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
        XMLExternalReference(const std::string &type, const std::string &systemID="", const std::string &publicID="") : type(type), systemID(systemID), publicID(publicID)
        {
        }
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
        XMLEntityMapping(const std::string &internal, const XMLExternalReference &external={""}, std::string notation="") : internal(internal), external(external), notation(notation)
        {
        }
        std::string internal;
        XMLExternalReference external{""};
        std::string notation;
    };
} // namespace XMLLib
