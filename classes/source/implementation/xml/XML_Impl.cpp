//
// Class: XML_Impl
//
// Description: XML class implementation api layer.
//
// Dependencies: C++20 - Language standard features used..
//

#include "XML_Impl.hpp"

namespace XML_Lib {

XML_Impl::XML_Impl(IStringify *stringify, IParser *parser)
{
  entityMapper = std::make_unique<XML_EntityMapper>();
  if (parser==nullptr) {
    xmlParser = std::make_unique<Default_Parser>(*entityMapper);
  } else {
    xmlParser.reset(parser);
  }
  if (stringify==nullptr) {
    xmlStringifier = std::make_unique<Default_Stringify>();
  } else {
    xmlStringifier.reset(stringify);
  }
}

XML_Impl::~XML_Impl() = default;

std::string XML_Impl::version()
{
  std::stringstream versionString;
  versionString << "XML_Lib Version " << XML_VERSION_MAJOR << "." << XML_VERSION_MINOR << "." << XML_VERSION_PATCH;
  return versionString.str();
}

Node &XML_Impl::dtd()
{
  if (xmlParser->canValidate()) {
    for (auto &child : prolog().getChildren()) {
      if (isA<DTD>(child)) { return child; }
    }
  }
  throw Error("No DTD found.");
}

Node &XML_Impl::prolog()
{
  if (!xmlRoot.isEmpty()) {
    return xmlRoot;
  }
  throw Error("No XML has been parsed.");
}

Node &XML_Impl::declaration() { return prolog().getChildren()[0]; }

Node &XML_Impl::root()
{
  for (auto &child : prolog().getChildren()) {
    if (isA<Root>(child) || isA<Self>(child)) { return child; }
  }
  throw Error("No root element found.");
}

void XML_Impl::validate() { xmlParser->validate(prolog()); }

void XML_Impl::parse(ISource &source) { xmlRoot = xmlParser->parse(source); }

void XML_Impl::stringify(IDestination &destination) { xmlStringifier->stringify(prolog(), destination, 0); }

void XML_Impl::traverse(IAction &action)
{
  if (xmlRoot.isEmpty()) { throw Error("No XML to traverse."); }
  traverseNodes(xmlRoot, action);
}
void XML_Impl::traverse(IAction &action) const
{
  if (xmlRoot.isEmpty()) { throw Error("No XML to traverse."); }
  traverseNodes(xmlRoot, action);
}
}// namespace XML_Lib
