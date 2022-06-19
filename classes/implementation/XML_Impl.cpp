//
// Class: XML_Impl
//
// Description: XML class implementation api layer.
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
    ///  Get XMLLib version.
    /// </summary>
    std::string XML_Impl::version()
    {
        //return (std::to_string(XML_VERSION_MAJOR) + "." + std::to_string(XML_VERSION_MINOR));
        return (std::to_string(0) + "." + std::to_string(1));
    }
    /// <summary>
    /// Return XML DT node.
    /// </summary>
    XMLNodeDTD &XML_Impl::dtd()
    {
        // Slow need to speed up.
        for (auto &element : prolog().children)
        {
            if (element->getNodeType() == XMLNodeType::dtd)
            {
                return (XMLNodeRef<XMLNodeDTD>(*element));
            }
        }
        throw XMLLib::Error("No DTD found.");
    }
    /// <summary>
    /// Return XML prolog node.
    /// </summary>
    XMLNodeProlog &XML_Impl::prolog()
    {
        return (XMLNodeRef<XMLNodeProlog>(*m_prolog));
    }
    /// <summary>
    /// Return XML declaration node.
    /// </summary>
    XMLNodeDeclaration &XML_Impl::declaration()
    {
        return (XMLNodeRef<XMLNodeDeclaration>(*prolog().children[0]));
    }
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
        throw XMLLib::Error("No root element found.");
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
