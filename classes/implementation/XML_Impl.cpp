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
/// XML constructor.
/// </summary>
XML_Impl::XML_Impl() { m_entityMapper = std::make_unique<XML_EntityMapper>(); }
/// <summary>
/// XML destructor.
/// </summary>
XML_Impl::~XML_Impl() {}
/// <summary>
///  Get XML_Lib version.
/// </summary>
std::string XML_Impl::version()
{
  std::stringstream versionString;
  versionString << "XML_Lib Version  " << XML_VERSION_MAJOR << "." << XML_VERSION_MINOR << "." << XML_VERSION_PATCH;
  return (versionString.str());
}
/// <summary>
/// Return XML DT node.
/// </summary>
XNode &XML_Impl::dtd()
{
  // Slow need to speed up.
  for (auto &element : prolog().children) {
    if (element->getNodeType() == XNodeType::dtd) { return (*element); }
  }
  throw XML_Lib::Error("No DTD found.");
}
/// <summary>
/// Return XML prolog node.
/// </summary>
XNode &XML_Impl::prolog() { return (*m_prolog); }
/// <summary>
/// Return XML declaration node.
/// </summary>
XNode &XML_Impl::declaration() { return (*prolog().children[0]); }
/// <summary>
/// Return XML root element node.
/// </summary>
XNode &XML_Impl::root()
{
  // Slow need to speed up.
  for (auto &element : prolog().children) {
    if ((element->getNodeType() == XNodeType::root) || (element->getNodeType() == XNodeType::self)) {
      return (*element);
    }
  }
  throw XML_Lib::Error("No root element found.");
}
/// <summary>
/// Parse XML read from source stream into internal object generating an exception
/// if a syntax error in the XML is found (not well formed).
/// </summary>
void XML_Impl::parse(ISource &source) { parseXML(source); }
/// <summary>
/// Validate XML against any DTD provided to see whether it is valid. If an
/// exception is thrown then there is a validation issue and the XML is not valid.
/// </summary>
void XML_Impl::validate()
{
  if (m_validator.get() != nullptr) { m_validator->validate(prolog()); }
}
/// <summary>
/// Create XML text from an XML object.
/// </summary>
/// <param name="destination">XML destination stream.</param>
void XML_Impl::stringify(IDestination &destination) { stringifyXML(destination); }
}// namespace XML_Lib
