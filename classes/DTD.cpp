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
/// DTD constructor.
/// </summary>
DTD::DTD(XNodeDTD &xNodeDTD) : m_implementation(std::make_unique<DTD_Impl>(xNodeDTD)) {}
/// <summary>
/// DTD destructor.
/// </summary>
DTD::~DTD() {}
/// <summary>
/// Parse DTD read from source stream.
/// </summary>
/// <param name="source">DTD source stream.</param>
void DTD::parse(ISource &source) { m_implementation->parse(source); }
/// <summary>
/// Create DTD text from an DTD object.
/// </summary>
/// <param name="destination">DTD destination stream.</param>
void DTD::stringify(IDestination &destination) { m_implementation->stringify(destination); }
}// namespace XML_Lib
