//
// Class: DTD_impl
//
// Description: DTD_Validator class implementation.
//
// Dependencies: C++20 - Language standard features used.
//

#include "DTD_Impl.hpp"

namespace XML_Lib {

/// <summary>
/// DTD_Impl constructor.
/// </summary>
DTD_Impl::DTD_Impl(XNode &xNode) : xDTD(XRef<DTD>(xNode)) {}

/// <summary>
/// DTD_Impl destructor.
/// </summary>
DTD_Impl::~DTD_Impl() = default;

/// <summary>
/// Parse DTD_Validator read from source stream.
/// </summary>
/// <param name="source">DTD_Validator source stream.</param>
void DTD_Impl::parse(ISource &source) { parseDTD(source); }

/// <summary>
/// Create DTD_Validator text from an DTD_Validator object.
/// </summary>
/// <param name="destination">DTD_Validator destination stream.</param>
void DTD_Impl::stringify(IDestination &destination) const { destination.add(xDTD.unparsed()); }
}// namespace XML_Lib
