#pragma once
// =========
// NAMESPACE
// =========
namespace XML_Lib
{
    // ===================================================
    // Forward declarations for interfaces/classes/structs
    // ===================================================
    struct XMLNodeProlog;
    // =======================
    // XML validator interface
    // =======================
    class IXMLValidator
    {
    public:
        // ========================
        // Constructors/destructors
        // ========================
        IXMLValidator() = default;
        IXMLValidator(const IXMLValidator& other) = delete;
        IXMLValidator& operator=(const IXMLValidator& other) = delete;
        IXMLValidator(IXMLValidator&& other) = delete;
        IXMLValidator& operator=(IXMLValidator&& other) = delete;
        virtual ~IXMLValidator() = default;
        // ========================
        // Validate XML against DTD
        // ========================
        virtual void validate(const XMLNodeProlog &prolog) = 0;
    };

} // namespace XML_Lib