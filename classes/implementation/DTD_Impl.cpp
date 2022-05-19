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
    DTD_Impl::DTD_Impl(IXMLEntityMapper &entityMapper)
    {
        m_parsed = std::make_unique<DTDParsed>(entityMapper);
    }
    DTD_Impl::~DTD_Impl()
    {
    }
    /// <summary>
    /// Parse DTD read from source stream.
    /// </summary>
    /// <param name="dtdSource">DTD source stream.</param>
    void DTD_Impl::parse(ISource &dtdSource)
    {
        parseDTD(dtdSource);
        m_parsed->setLineCount(static_cast<long>(std::count(m_unparsed.begin(), m_unparsed.end(), kLineFeed))+1);
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
