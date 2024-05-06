//
// Class: XML_Impl
//
// Description: XML class implementation api layer.
//
// Dependencies: C++20 - Language standard features used..
//

#include "XML_Impl.hpp"
#include "XML_Parser.hpp"

namespace XML_Lib {

/// <summary>
/// XML constructor.
/// </summary>
XML_Impl::XML_Impl()
{
  entityMapper = std::make_unique<XML_EntityMapper>();
  parser = std::make_unique<XML_Parser>(*entityMapper);
  stringifier = std::make_unique<XML_Stringify>();
}

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
  versionString << "XML_Lib Version " << XML_VERSION_MAJOR << "." << XML_VERSION_MINOR << "." << XML_VERSION_PATCH;
  return (versionString.str());
}

/// <summary>
/// Return XML DTD XNode.
/// </summary>
/// <returns>Reference to DTD XNode.</returns>
XNode &XML_Impl::dtd()
{
  if (parser->canValidate()) {
    for (auto &element : prolog().getChildren()) {
      if (element.isDTD()) { return (element); }
    }
  }
  throw Error("No DTD found.");
}

/// <summary>
/// Return XML prolog XNode.
/// </summary>
/// <returns>Reference to prolog XNode.</returns>
XNode &XML_Impl::prolog()
{
  if (!xmlRoot.isEmpty()) {
    return (xmlRoot);
  } else {
    throw Error("No XML has been parsed.");
  }
}

/// <summary>
/// Return XML declaration XNode.
/// </summary>
/// <returns>Reference to declaration XNode.</returns>
XNode &XML_Impl::declaration() { return (prolog().getChildren()[0]); }

/// <summary>
/// Return XML root element XNode.
/// </summary>
/// <returns>Reference to root element XNode.</returns>
XNode &XML_Impl::root()
{
  for (auto &element : prolog().getChildren()) {
    if (element.isRoot() || element.isSelf()) { return (element); }
  }
  throw Error("No root element found.");
}

/// <summary>
/// Validate XML against any DTD provided to see whether it is valid. If an
/// exception is thrown then there is a validation issue and the XML is not valid.
/// </summary>
void XML_Impl::validate()
{

  parser->validate(prolog());
}

/// <summary>
/// Parse XML read from source stream into internal object generating an exception
/// if a syntax error in the XML is found (not well formed).
/// </summary>
void XML_Impl::parse(ISource &source) { xmlRoot = parser->parse(source); }
/// <summary>
/// Create XML text on destination stream from an XML object.
/// </summary>
/// <param name="destination">XML destination stream.</param>
void XML_Impl::stringify(IDestination &destination) { stringifier->stringify(prolog(), destination); }

}// namespace XML_Lib
