//
// Class: XML_Impl
//
// Description: XML class implementation api layer.
//
// Dependencies: C++20 - Language standard features used..
//

#include "XML_Impl.hpp"
#include <sstream>
#if defined(XML_LIB_ENABLE_XSD)
#include "XSD_Validator.hpp"
#endif
#if defined(XML_LIB_ENABLE_XPATH)
#include "XPath.hpp"
#endif

namespace XML_Lib {

/// <summary>
/// Return the first child of xNode satisfying predicate, or nullptr.
/// </summary>
template<typename Predicate>
/// <summary>
/// Find the first child matching the supplied predicate.
/// </summary>
static Node *findFirstChild(Node &xNode, Predicate &&predicate)
{
  for (auto &child : xNode.getChildren()) {
    if (predicate(child)) { return &child; }
  }
  return nullptr;
}

/// <summary>
/// Initialize XML_Impl with the provided stringifier and parser.
/// </summary>
XML_Impl::XML_Impl(IStringify *stringify, IParser *parser)
{
  entityMapper = std::make_unique<XML_EntityMapper>();
  if (parser == nullptr) {
    xmlParser = std::make_unique<Default_Parser>(*entityMapper);
  } else {
    xmlParser.reset(parser);
  }
#if defined(XML_LIB_ENABLE_STRINGIFY)
  if (stringify == nullptr) {
    xmlStringifier = std::make_unique<Default_Stringify>();
  } else {
    xmlStringifier.reset(stringify);
  }
#else
  (void)stringify;
#endif
}

/// <summary>
/// Destroy XML_Impl and release any owned resources.
/// </summary>
XML_Impl::~XML_Impl() = default;

/// <summary>
/// Return the XML_Lib version string.
/// </summary>
std::string XML_Impl::version()
{
  std::stringstream versionString;
  versionString << "XML_Lib Version " << XML_VERSION_MAJOR << "." << XML_VERSION_MINOR << "." << XML_VERSION_PATCH;
  return versionString.str();
}

#if defined(XML_LIB_ENABLE_DTD)
/// <summary>
/// Return the DTD node used for document validation.
/// </summary>
Node &XML_Impl::dtd()
{
  if (xmlParser->canValidate()) {
    if (Node *found = findFirstChild(prolog(), [](const Node &n) { return isA<DTD>(n); })) {
      return *found;
    }
  }
  XML_LIB_THROW(Error("No DTD found."));
}
#endif

/// <summary>
/// Return the XML prolog node.
/// </summary>
Node &XML_Impl::prolog()
{
  if (!xmlRoot.isEmpty()) { return xmlRoot; }
  XML_LIB_THROW(Error("No XML has been parsed."));
}

/// <summary>
/// Return the XML declaration node.
/// </summary>
Node &XML_Impl::declaration() { return prolog().getChildren()[0]; }

/// <summary>
/// Return the document root node.
/// </summary>
Node &XML_Impl::root()
{
  if (Node *found = findFirstChild(prolog(), [](const Node &n) { return isA<Root>(n) || isA<Self>(n); })) {
    return *found;
  }
  XML_LIB_THROW(Error("No root element found."));
}

#if defined(XML_LIB_ENABLE_DTD)
/// <summary>
/// Validate the parsed XML against the attached DTD.
/// </summary>
void XML_Impl::validate() { xmlParser->validate(prolog()); }
#endif

#if defined(XML_LIB_ENABLE_XSD)
/// <summary>
/// Validate the parsed XML against an XSD schema string.
/// </summary>
void XML_Impl::validate(const std::string_view &xsdSource)
{
  XSD_Validator xsdValidator(root());
  BufferSource source(xsdSource);
  xsdValidator.parse(source);
  xsdValidator.validate(root());
}
#endif

#if defined(XML_LIB_ENABLE_XPATH)
/// <summary>
/// Evaluate an XPath expression against the current document.
/// </summary>
std::vector<const Node *> XML_Impl::xpath(const std::string_view expression)
{
  XPath xp(root());
  return xp.evaluate(expression);
}
#endif

/// <summary>
/// Implementation of XML_Impl::parse.
/// </summary>
void XML_Impl::parse(ISource &source, const ParseOptions &options)
{
  XML_Arena::ScopedCurrentArena scopedCurrentArena(arena);
  XML_Arena::ScopedDefaultResource scopedDefaultResource(arena);
  xmlRoot = xmlParser->parse(source, options);
}
#if defined(XML_LIB_ENABLE_STRINGIFY)
/// <summary>
/// Serialize the current XML document to the destination.
/// </summary>
void XML_Impl::stringify(IDestination &destination) { xmlStringifier->stringify(prolog(), destination, 0); }
#endif

/// <summary>
/// Traverse the XML tree and invoke the supplied action.
/// </summary>
void XML_Impl::traverse(IAction &action)
{
  if (xmlRoot.isEmpty()) { XML_LIB_THROW(Error("No XML to traverse.")); }
  traverseNodes(xmlRoot, action);
}

/// <summary>
/// Traverse the XML tree with a const action handler.
/// </summary>
void XML_Impl::traverse(IAction &action) const
{
  if (xmlRoot.isEmpty()) { XML_LIB_THROW(Error("No XML to traverse.")); }
  traverseNodes(xmlRoot, action);
}
}// namespace XML_Lib