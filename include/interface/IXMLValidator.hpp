#pragma once
//
// XML types
//
#include "XML_Types.hpp"
// =========
// NAMESPACE
// =========
namespace XMLLib
{
    // =======================
    // XML validator interface
    // =======================
    class IXMLValidator
    {
    public:
        virtual ~IXMLValidator() = default;
        virtual void validate(XMLNodeProlog &prolog) = 0;
    };

} // namespace XMLLib