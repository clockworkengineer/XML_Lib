#pragma once
//
// C++ STL
//
#include <string>
#include <utility>
#include <vector>
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
        XMLValue(const std::string &unparsed, const std::string &parsed = "") : unparsed(unparsed), parsed(parsed)
        {
        }
        [[nodiscard]] bool isReference() const
        {
            return (unparsed[0] == '&' && unparsed.back() == ';');
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
        XMLAttribute(const std::string &name, const XMLValue &value) : name(name), value(value)
        {
        }
        std::string name;
        XMLValue value;
    };
    using XMLAttributeList = std::vector<XMLAttribute>;
    //
    // XML External reference
    //
    struct XMLExternalReference
    {
        XMLExternalReference(const std::string &type, const std::string &systemID = "", const std::string &publicID = "") : type(type), systemID(systemID), publicID(publicID)
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
        XMLEntityMapping(const std::string &internal, const XMLExternalReference &external = {""}, std::string notation = "") : internal(internal), external(external), notation(notation)
        {
        }
        std::string internal;
        XMLExternalReference external{""};
        std::string notation;
    };
} // namespace XMLLib
