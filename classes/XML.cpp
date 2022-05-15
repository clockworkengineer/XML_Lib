//
// Class: XML
//
// Description: Parse XML into an internal XML Object (generating an exception
// if it is found not to be syntactically correct "well formed") so that it can
// be manipulated, interrogated and saved back to a text form (stringify). For more
// information on the XML standard check out https://www.w3.org/TR/REC-xml/.
//
// Dependencies:   C20++ - Language standard features used.
//
// =================
// CLASS DEFINITIONS
// =================
#include "XML.hpp"
#include "DTD.hpp"
#include "XML_EntityMapper.hpp"
#include "IXMLValidator.hpp"
// ====================
// CLASS IMPLEMENTATION
// ====================
// =========
// NAMESPACE
// =========
namespace XMLLib
{
    // ===========================
    // PRIVATE TYPES AND CONSTANTS
    // ===========================
    // ==========================
    // PUBLIC TYPES AND CONSTANTS
    // ==========================
    // ========================
    // PRIVATE STATIC VARIABLES
    // ========================
    // =======================
    // PUBLIC STATIC VARIABLES
    // =======================
    // ===============
    // PRIVATE METHODS
    // ===============
    // ==============
    // PUBLIC METHODS
    // ==============
    /// <summary>
    /// XML constructor.
    /// </summary>
    XML::XML()
    {
        m_prolog = std::make_unique<XMLNodeElement>();
        m_entityMapper = std::make_unique<XML_EntityMapper>();
    }
    /// <summary>
    /// XML destructor.
    /// </summary>
    XML::~XML()
    {
    }
    /// <summary>
    /// Parse XML read from source stream into internal object generating an exception
    /// if a syntax error in the XML is found (not well formed).
    /// </summary>
    void XML::parse(ISource &source)
    {
        parseXML(source);
    }
    /// <summary>
    /// Validate XML against any DTD provided to see whether it is valid. If an
    /// exception is thrown then there is a validation issue and the XML is not valid.
    /// </summary>
    void XML::validate()
    {
        if (m_validator.get() != nullptr)
        {
            m_validator->validate(prolog());
        }
    }
    /// <summary>
    /// Create XML text from an XML object.
    /// </summary>
    /// <param name="destination">XML destination stream.</param>
    void XML::stringify(IDestination &destination)
    {
        stringifyXML(destination);
    }
} // namespace XMLLib
