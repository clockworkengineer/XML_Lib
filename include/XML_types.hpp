#pragma once
//
// C++ STL
//
#include <string>
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
} // namespace XMLLib
