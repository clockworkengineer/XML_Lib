//
// Class: DTD_impl
//
// Description: DTD class implementation.
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
    DTD_Impl::DTD_Impl(IXMLEntityMapper &entityMapper)  : m_entityMapper(entityMapper)
    {
    }
    DTD_Impl::~DTD_Impl()
    {
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    long DTD_Impl::getLineCount() const
    {
        long lineNumber = 1;
        for (auto ch : m_unparsed)
        {
            if (ch == kLineFeed)
            {
                lineNumber++;
            }
        }
        return (lineNumber);
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    uint16_t DTD_Impl::getType() const
    {
        return (m_type);
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    std::string &DTD_Impl::getRootName()
    {
        return (m_name);
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    XMLExternalReference &DTD_Impl::getExternalReference()
    {
        return (m_external);
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    bool DTD_Impl::isElementPresent(const std::string &elementName) const
    {
        return (m_elements.find(elementName) != m_elements.end());
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    DTDElement &DTD_Impl::getElement(const std::string &elementName)
    {
        return (m_elements[elementName]);
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    bool DTD_Impl::isEntityPresent(const std::string &entityName) const
    {
        return (m_entityMapper.isPresent(entityName));
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    XMLEntityMapping &DTD_Impl::getEntity(const std::string &entityName)
    {
        return (m_entityMapper.get(entityName));
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    XMLExternalReference &DTD_Impl::getNotation(const std::string &notationName)
    {
        return (m_notations[notationName]);
    }
    /// <summary>
    /// Parse DTD read from source stream.
    /// </summary>
    /// <param name="dtdSource">DTD source stream.</param>
    void DTD_Impl::parse(ISource &dtdSource)
    {
        parseDTD(dtdSource);
    }
    /// <summary>
    /// Create DTD text from an DTD object.
    /// </summary>
    /// <param name="destination">DTD destination stream.</param>
    void DTD_Impl::stringify(IDestination &destination)
    {
        destination.add(m_unparsed);
    }
} // namespace XMLLib
