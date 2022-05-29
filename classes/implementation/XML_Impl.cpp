//
// Class: XML_Impl
//
// Description: XML class implementation.
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
    XML_Impl::XML_Impl()
    {
        m_entityMapper = std::make_unique<XML_EntityMapper>();
    }
    /// <summary>
    /// XML destructor.
    /// </summary>
    XML_Impl::~XML_Impl()
    {
    }
    /// <summary>
    /// Return XML DTD.
    /// </summary>
    DTD &XML_Impl::dtd()
    {
        return (*m_dtd);
    }
    /// <summary>
    /// Return XML prolog node.
    /// </summary>
    XMLNodeElement &XML_Impl::prolog()
    {
        return (XMLNodeRef<XMLNodeElement>(*m_prolog));
    };
    /// <summary>
    /// Return XML root element node.
    /// </summary>
    XMLNodeElement &XML_Impl::root()
    {
        // Slow need to speed up.
        for (auto &element : prolog().children)
        {
            if ((element->getNodeType() == XMLNodeType::root) ||
                (element->getNodeType() == XMLNodeType::self))
            {
                return (XMLNodeRef<XMLNodeElement>(*element));
            }
        }
        throw std::runtime_error("Error: No root element found.");
    }
    /// <summary>
    /// Parse XML read from source stream into internal object generating an exception
    /// if a syntax error in the XML is found (not well formed).
    /// </summary>
    void XML_Impl::parse(ISource &source)
    {
        parseXML(source);
    }
    /// <summary>
    /// Validate XML against any DTD provided to see whether it is valid. If an
    /// exception is thrown then there is a validation issue and the XML is not valid.
    /// </summary>
    void XML_Impl::validate()
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
    void XML_Impl::stringify(IDestination &destination)
    {
        stringifyXML(destination);
    }
} // namespace XMLLib
