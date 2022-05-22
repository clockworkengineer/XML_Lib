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
        XMLValue(const std::string &unparsedValue, const std::string &parsedValue) : unparsed(unparsedValue), parsed(parsedValue)
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
        XMLAttribute(const std::string &attributeName, const XMLValue &attributeValue) : name(attributeName), value(attributeValue)
        {
        }
        const std::string name;
        const XMLValue value;
    };
    //
    // XML External reference
    //
    struct XMLExternalReference
    {
        // XMLExternalReference() = default;
        // XMLExternalReference(const std::string &refType, const std::string &refSystemID, const std::string &refPublicID) : type(refType), systemID(refSystemID), publicID(refPublicID)
        // {
        // }
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
} // namespace XMLLib
