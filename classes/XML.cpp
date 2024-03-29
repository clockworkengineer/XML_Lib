//
// Class: XML
//
// Description: Parse XML into an internal XML Object (generating an exception
// if it is found not to be syntactically correct "well formed") so that it can
// be manipulated, interrogated and saved back to a text form (stringify). For more
// information on the XML standard check out https://www.w3.org/TR/REC-xml/.
//
// Dependencies: C++20 - Language standard features used..
//

#include "XML_Impl.hpp"

namespace XML_Lib {

/// <summary>
/// XML constructor.
/// </summary>
XML::XML() : implementation(std::make_unique<XML_Impl>()) {}

/// <summary>
/// XML destructor.
/// </summary>
XML::~XML() {}

/// <summary>
/// Get XML_Lib version.
/// </summary>
/// <returns>Library version string.</returns>
std::string XML::version() { return (implementation->version()); }

/// <summary>
/// Return reference to any internal DTD for parsed XML.
/// </summary>
/// <returns>Reference to DTD XNode.</returns>
XNode &XML::dtd() { return (implementation->dtd()); }

/// <summary>
/// Return prolog node of parsed XML tree.
/// </summary>
/// <returns>Reference to prolog XNode.</returns>
XNode &XML::prolog() { return (implementation->prolog()); }

/// <summary>
/// Return declaration node of parsed XML tree.
/// </summary>
/// <returns>Reference to declaration XNode.</returns>
XNode &XML::declaration() { return (implementation->declaration()); }

/// <summary>
/// Return root node of parsed XML tree.
/// </summary>
/// <returns>Reference to root element XNode.</returns>
XNode &XML::root() { return (implementation->root()); }

/// <summary>
/// Validate XML against any DTD provided to see whether it is valid. If an
/// exception is thrown then there is a validation issue and the XML is not valid.
/// </summary>
void XML::validate() { implementation->validate(); }

/// <summary>
/// Parse XML read from source stream into internal object generating an exception
/// if a syntax error in the XML is found (not well formed).
/// </summary>
void XML::parse(ISource &source) { implementation->parse(source); }
void XML::parse(ISource &&source) { implementation->parse(source); }

/// <summary>
/// Create XML text from an XML object.
/// </summary>
/// <param name="destination">XML destination stream.</param>
void XML::stringify(IDestination &destination) { implementation->stringify(destination); }
void XML::stringify(IDestination &&destination) { implementation->stringify(destination); }
}// namespace XML_Lib
