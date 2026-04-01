//
// Class: XSD_Validator
//
// Description: Parse XML Schema Definition (XSD) so that XML may be validated
// against it. For more information on the XSD standard check out
// https://www.w3.org/TR/xmlschema-1/.
//
// Dependencies: C++20 - Language standard features used.
//

#include "XSD_Impl.hpp"

namespace XML_Lib {

/// <summary>
/// XSD_Validator constructor.
/// </summary>
XSD_Validator::XSD_Validator(Node &xNode) : implementation(std::make_unique<XSD_Impl>(xNode)) {}

/// <summary>
/// XSD_Validator destructor.
/// </summary>
XSD_Validator::~XSD_Validator() = default;

/// <summary>
/// Parse XSD schema read from the source stream.
/// </summary>
/// <param name="source">XSD source stream.</param>
void XSD_Validator::parse(ISource &source) { implementation->parse(source); }

/// <summary>
/// Create XSD text from an XSD_Validator object.
/// </summary>
/// <param name="destination">XSD destination stream.</param>
void XSD_Validator::stringify(IDestination &destination) { implementation->stringify(destination); }

/// <summary>
/// Validate XML against XSD.
/// </summary>
/// <param name="xNode">XML root Node.</param>
void XSD_Validator::validate(const Node &xNode) { implementation->validate(xNode); }

}// namespace XML_Lib
