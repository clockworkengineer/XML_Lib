//
// Class: XML_Impl
//
// Description: XML class implementation api layer.
//
// Dependencies:   C++20 - Language standard features used.
//

#include "XML_Impl.hpp"

namespace XML_Lib {

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
/// <returns>Library version string.</returns>
std::string XML_Impl::version()
{
  std::stringstream versionString;
  versionString << "XML_Lib Version  " << XML_VERSION_MAJOR << "." << XML_VERSION_MINOR << "." << XML_VERSION_PATCH;
  return (versionString.str());
}

/// <summary>
/// Return XML DTD XNode.
/// </summary>
/// <returns>Reference to DTD XNode.</returns>
XNode &XML_Impl::dtd()
{
  for (auto &element : prolog().getChildren()) {
    if (element->getType() == XNode::Type::dtd) { return (*element); }
  }
  throw XML_Lib::Error("No DTD found.");
}

/// <summary>
/// Return XML prolog XNode.
/// </summary>
/// <returns>Reference to prolog XNode.</returns>
XNode &XML_Impl::prolog() { return (*m_prolog); }

/// <summary>
/// Return XML declaration XNode.
/// </summary>
/// <returns>Reference to declaration XNode.</returns>
XNode &XML_Impl::declaration() { return (*prolog().getChildren()[0]); }

/// <summary>
/// Return XML root element XNode.
/// </summary>
/// <returns>Reference to root element XNode.</returns>
XNode &XML_Impl::root()
{
  for (auto &element : prolog().getChildren()) {
    if ((element->getType() == XNode::Type::root) || (element->getType() == XNode::Type::self)) { return (*element); }
  }
  throw XML_Lib::Error("No root element found.");
}

/// <summary>
/// Parse XML read from source stream into internal object generating an exception
/// if a syntax error in the XML is found (not well formed).
/// </summary>
void XML_Impl::parse(ISource &source) { m_prolog = parseXML(source); }

/// <summary>
/// Validate XML against any DTD provided to see whether it is valid. If an
/// exception is thrown then there is a validation issue and the XML is not valid.
/// </summary>
void XML_Impl::validate()
{
  if (m_validator.get() != nullptr) { m_validator->validate(prolog()); }
}

/// <summary>
/// Create XML text on destination stream from an XML object.
/// </summary>
/// <param name="destination">XML destination stream.</param>
void XML_Impl::stringify(IDestination &destination) { stringifyXML(destination); }
}// namespace XML_Lib
