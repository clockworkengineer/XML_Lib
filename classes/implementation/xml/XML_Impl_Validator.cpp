//
// Class: XML_Impl_Validator
//
// Description: XML DTD parser code.
//
// Dependencies: C++20 - Language standard features used..
//

#include "XML.hpp"
#include "XML_Impl.hpp"
#include "DTD.hpp"
#include "DTD_Impl.hpp"

namespace XML_Lib {

/// <summary>
/// Parse XML DTD and return any XNode created for it.
/// </summary>
/// <param name="source">XML source stream.</param>
/// <returns>Pointer to DTD XNode.</returns>
XNode XML_Impl::parseDTD(ISource &source)
{
  if (hasDTD) { throw XML_Lib::SyntaxError(source.getPosition(), "More than one DOCTYPE declaration."); }
  auto xNode = XNode::make<XDTD>(*entityMapper);
  validator = std::make_unique<DTD>(xNode);
  validator->parse(source);
  hasDTD = true;
  return (xNode);
}

}// namespace XML_Lib
