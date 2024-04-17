//
// Class: DTD_impl
//
// Description: DTD class implementation.
//
// Dependencies: C++20 - Language standard features used..
//

#include "DTD_Impl.hpp"

namespace XML_Lib {

/// <summary>
/// DTD_Impl constructor.
/// </summary>
DTD_Impl::DTD_Impl(XNode &xNode) : xDTD(XRef<XDTD>(xNode)) {}

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
void DTD_Impl::stringify(IDestination &destination) { destination.add(xDTD.unparsed()); }
}// namespace XML_Lib
