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
#include "XML_Impl.hpp"
// ====================
// CLASS IMPLEMENTATION
// ====================
// =========
// NAMESPACE
// =========
namespace XML_Lib
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
    XML::XML() : m_implementation(std::make_unique<XML_Impl>())
    {
    }
    /// <summary>
    /// XML destructor.
    /// </summary>
    XML::~XML()
    {
    }
    /// <summary>
    /// Get XML_Lib version.
    /// </summary>
    std::string XML::version()
    {
        return (m_implementation->version());
    }
    /// <summary>
    /// Return reference to any internal DTD for parsed XML.
    /// </summary>
    XMLNodeDTD &XML::dtd()
    {
        return (m_implementation->dtd());
    }
    /// <summary>
    /// Return prolog node of parsed XML tree.
    /// </summary>
    XMLNodeProlog &XML::prolog()
    {
        return (m_implementation->prolog());
    }
    /// <summary>
    /// Return declaration node of parsed XML tree.
    /// </summary>
    XMLNodeDeclaration &XML::declaration()
    {
        return (m_implementation->declaration());
    }
    /// <summary>
    /// Return root node of parsed XML tree.
    /// </summary>
    XMLNodeElement &XML::root()
    {
        return (m_implementation->root());
    }
    /// <summary>
    /// Validate XML against any DTD provided to see whether it is valid. If an
    /// exception is thrown then there is a validation issue and the XML is not valid.
    /// </summary>
    void XML::validate()
    {
        m_implementation->validate();
    }
    /// <summary>
    /// Parse XML read from source stream into internal object generating an exception
    /// if a syntax error in the XML is found (not well formed).
    /// </summary>
    void XML::parse(ISource &source)
    {
        m_implementation->parse(source);
    }
    void XML::parse(ISource &&source)
    {
        m_implementation->parse(source);
    }
    /// <summary>
    /// Create XML text from an XML object.
    /// </summary>
    /// <param name="destination">XML destination stream.</param>
    void XML::stringify(IDestination &destination)
    {
        m_implementation->stringify(destination);
    }
    void XML::stringify(IDestination &&destination)
    {
        m_implementation->stringify(destination);
    }
} // namespace XML_Lib
