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
// =================
// LIBRARY NAMESPACE
// =================
namespace XML_Lib {
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
/// DTD_Impl constructor.
/// </summary>
DTD_Impl::DTD_Impl(XNodeDTD &xNodeDTD) : m_xNodeDTD(xNodeDTD) {}
/// <summary>
/// DTD_Impl destructor.
/// </summary>
DTD_Impl::~DTD_Impl() {}
/// <summary>
/// Parse DTD read from source stream.
/// </summary>
/// <param name="source">DTD source stream.</param>
void DTD_Impl::parse(ISource &source) { parseDTD(source); }
/// <summary>
/// Create DTD text from an DTD object.
/// </summary>
/// <param name="destination">DTD destination stream.</param>
void DTD_Impl::stringify(IDestination &destination) { destination.add(m_xNodeDTD.unparsed()); }
}// namespace XML_Lib
