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
        // Constructors/destructors
        IXMLValidator() = default;
        IXMLValidator(const IXMLValidator& other) = delete;
        IXMLValidator& operator=(const IXMLValidator& other) = delete;
        IXMLValidator(IXMLValidator&& other) = delete;
        IXMLValidator& operator=(IXMLValidator&& other) = delete;
        virtual ~IXMLValidator() = default;
        virtual void validate(const XMLNodeProlog &prolog) = 0;
    };

} // namespace XMLLib