//
// Class: DTD
//
// Description: Parse XML Data Type Declaration (DTD) portion of an XML file so
// that the XML maybe be validated against it and flagged as well-formed if it passes.
// For more information on the XML standard check out https://www.w3.org/TR/REC-xml/.
//
// Dependencies:   C20++ - Language standard features used.
//
// =================
// CLASS DEFINITIONS
// =================
#include "DTD.hpp"
#include "DTD_Impl.hpp"
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
    DTD::DTD(IXMLEntityMapper &entityMapper)
    {
        m_implementation = std::make_unique<DTD_Impl>(entityMapper);
    }
    DTD::~DTD()
    {
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    long DTD::getLineCount() const
    {
        return (m_implementation->getLineCount());
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    uint16_t DTD::getType() const
    {
        return (m_implementation->getType());
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    std::string &DTD::getRootName()
    {
        return (m_implementation->getRootName());
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    XMLExternalReference &DTD::getExternalReference()
    {
        return (m_implementation->getExternalReference());
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    bool DTD::isElementPresent(const std::string &elementName) const
    {
        return (m_implementation->isElementPresent(elementName));
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    long DTD::getElementCount() const
    {
        return (m_implementation->getElementCount());
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    DTDElement &DTD::getElement(const std::string &elementName)
    {
        return (m_implementation->getElement(elementName));
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    bool DTD::isEntityPresent(const std::string &entityName) const
    {
        return (m_implementation->isEntityPresent(entityName));
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    XMLEntityMapping &DTD::getEntity(const std::string &entityName)
    {
        return (m_implementation->getEntity(entityName));
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    XMLExternalReference &DTD::getNotation(const std::string &notationName)
    {
        return (m_implementation->getNotation(notationName));
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    DTDParsed &DTD::parsed()
    {
        return (m_implementation->parsed());
    }
    /// <summary>
    /// Parse DTD read from source stream.
    /// </summary>
    /// <param name="dtdSource">DTD source stream.</param>
    void DTD::parse(ISource &dtdSource)
    {
        m_implementation->parse(dtdSource);
    }
    /// <summary>
    /// Create DTD text from an DTD object.
    /// </summary>
    /// <param name="destination">DTD destination stream.</param>
    void DTD::stringify(IDestination &destination)
    {
        m_implementation->stringify(destination);
    }
} // namespace XMLLib
