//
// Class: DTD_Validator
//
// Description: Parse XML Data Type Declaration (DTD) portion of an XML file so
// that the XML maybe be validated against it and flagged as well-formed if it passes.
// For more information on the XML standard check out https://www.w3.org/TR/REC-xml/.
//
// Dependencies: C++20 - Language standard features used.
//

#include "DTD_Impl.hpp"

namespace XML_Lib {

/// <summary>
/// DTD_Validator constructor.
/// </summary>
DTD_Validator::DTD_Validator(XNode &xNode) : implementation(std::make_unique<DTD_Impl>(xNode)) {}

/// <summary>
/// DTD_Validator destructor.
/// </summary>
DTD_Validator::~DTD_Validator() = default;

/// <summary>
/// Parse DTD read from source stream.
/// </summary>
/// <param name="source">DTD source stream.</param>
void DTD_Validator::parse(ISource &source) { implementation->parse(source); }

/// <summary>
/// Create DTD text from an DTD_Validator object.
/// </summary>
/// <param name="destination">DTD destination stream.</param>
void DTD_Validator::stringify(IDestination &destination) { implementation->stringify(destination); }

/// <summary>
/// Validate XML against DTD.
/// </summary>
/// <param name="xNode">XML prolog XNode.</param>
void DTD_Validator::validate(const XNode &xNode) { implementation->validate(xNode); }

}// namespace XML_Lib
