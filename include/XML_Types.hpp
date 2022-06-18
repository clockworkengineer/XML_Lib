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
    // =========
    // XML value
    // =========
    struct XMLValue
    {
        explicit XMLValue(std::string unparsed) : unparsed(std::move(unparsed))
        {
        }
        XMLValue(std::string unparsed, std::string parsed) : unparsed(std::move(unparsed)), parsed(std::move(parsed))
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
        std::string unparsed{};
        std::string parsed{};
    };
    // =============
    // XML Attribute
    // =============
    struct XMLAttribute
    {
        using List = std::vector<XMLAttribute>;
        XMLAttribute(std::string name, XMLValue value) : name(std::move(name)), value(std::move(value))
        {
        }
        std::string name;
        XMLValue value;
    };
    // ======================
    // XML External reference
    // ======================
    struct XMLExternalReference
    {
        explicit XMLExternalReference(std::string type) : type(std::move(type))
        {
        }
        XMLExternalReference(std::string type, std::string systemID, std::string publicID) : type(std::move(type)), systemID(std::move(systemID)), publicID(std::move(publicID))
        {
        }
        std::string type{};
        std::string systemID{};
        std::string publicID{};
    };
    // ==================
    // XML Entity mapping
    // ==================
    struct XMLEntityMapping
    {
        explicit XMLEntityMapping(std::string internal) : internal(std::move(internal))
        {
        }
        XMLEntityMapping(std::string internal, XMLExternalReference external, std::string notation) : internal(std::move(internal)), external(std::move(external)), notation(std::move(notation))
        {
        }
        std::string internal{};
        XMLExternalReference external{""};
        std::string notation{};
    };
} // namespace XMLLib
