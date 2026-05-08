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

XML_Impl::~XML_Impl() = default;

std::string XML_Impl::version()
{
  std::stringstream versionString;
  versionString << "XML_Lib Version " << XML_VERSION_MAJOR << "." << XML_VERSION_MINOR << "." << XML_VERSION_PATCH;
  return versionString.str();
}

#if defined(XML_LIB_ENABLE_DTD)
Node &XML_Impl::dtd()
{
  if (xmlParser->canValidate()) {
    for (auto &child : prolog().getChildren()) {
      if (isA<DTD>(child)) { return child; }
    }
  }
  XML_LIB_THROW(Error("No DTD found."));
}
#endif

Node &XML_Impl::prolog()
{
  if (!xmlRoot.isEmpty()) { return xmlRoot; }
  XML_LIB_THROW(Error("No XML has been parsed."));
}

Node &XML_Impl::declaration() { return prolog().getChildren()[0]; }

Node &XML_Impl::root()
{
  for (auto &child : prolog().getChildren()) {
    if (isA<Root>(child) || isA<Self>(child)) { return child; }
  }
  XML_LIB_THROW(Error("No root element found."));
}

#if defined(XML_LIB_ENABLE_DTD)
void XML_Impl::validate() { xmlParser->validate(prolog()); }
#endif

#if defined(XML_LIB_ENABLE_XSD)
void XML_Impl::validate(const std::string_view &xsdSource)
{
  XSD_Validator xsdValidator(root());
  BufferSource source(xsdSource);
  xsdValidator.parse(source);
  xsdValidator.validate(root());
}
#endif

#if defined(XML_LIB_ENABLE_XPATH)
std::vector<const Node *> XML_Impl::xpath(const std::string_view expression)
{
  XPath xp(root());
  return xp.evaluate(expression);
}
#endif

void XML_Impl::parse(ISource &source)
{
  XML_Arena::ScopedCurrentArena scopedCurrentArena(arena);
  XML_Arena::ScopedDefaultResource scopedDefaultResource(arena);
  xmlRoot = xmlParser->parse(source);
}
#if defined(XML_LIB_ENABLE_STRINGIFY)
void XML_Impl::stringify(IDestination &destination) { xmlStringifier->stringify(prolog(), destination, 0); }
#endif

void XML_Impl::traverse(IAction &action)
{
  if (xmlRoot.isEmpty()) { XML_LIB_THROW(Error("No XML to traverse.")); }
  traverseNodes(xmlRoot, action);
}
void XML_Impl::traverse(IAction &action) const
{
  if (xmlRoot.isEmpty()) { XML_LIB_THROW(Error("No XML to traverse.")); }
  traverseNodes(xmlRoot, action);
}
}// namespace XML_Lib
