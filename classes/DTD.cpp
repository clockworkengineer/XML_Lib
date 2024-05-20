//
// Class: DTD
//
// Description: Parse XML Data Type Declaration (DTD) portion of an XML file so
// that the XML maybe be validated against it and flagged as well-formed if it passes.
// For more information on the XML standard check out https://www.w3.org/TR/REC-xml/.
//
// Dependencies: C++20 - Language standard features used..
//

#include "DTD_Impl.hpp"

namespace XML_Lib {

/// <summary>
/// DTD constructor.
/// </summary>
DTD::DTD(XNode &xNode) : implementation(std::make_unique<DTD_Impl>(xNode)) {}

/// <summary>
/// DTD destructor.
/// </summary>
DTD::~DTD() {}

/// <summary>
/// Parse DTD read from source stream.
/// </summary>
/// <param name="source">DTD source stream.</param>
void DTD::parse(ISource &source) { implementation->parse(source); }

/// <summary>
/// Create DTD text from an DTD object.
/// </summary>
/// <param name="destination">DTD destination stream.</param>
void DTD::stringify(IDestination &destination) { implementation->stringify(destination); }

/// <summary>
/// Validate XML against DTD.
/// </summary>
/// <param name="xNode">XML prolog XNode.</param>
void DTD::validate(const XNode &xNode) { implementation->validate(xNode); }

}// namespace XML_Lib
