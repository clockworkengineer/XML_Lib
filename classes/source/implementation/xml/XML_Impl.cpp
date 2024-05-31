//
// Class: XML_Impl
//
// Description: XML class implementation api layer.
//
// Dependencies: C++20 - Language standard features used..
//

#include "XML_Impl.hpp"
#include "XML_Parser.hpp"
#include "XML_Stringify.hpp"

namespace XML_Lib {

XML_Impl::XML_Impl()
{
  entityMapper = std::make_unique<XML_EntityMapper>();
  parser = std::make_unique<XML_Parser>(*entityMapper);
  stringifier = std::make_unique<XML_Stringify>();
}

XML_Impl::~XML_Impl() {}

std::string XML_Impl::version()
{
  std::stringstream versionString;
  versionString << "XML_Lib Version " << XML_VERSION_MAJOR << "." << XML_VERSION_MINOR << "." << XML_VERSION_PATCH;
  return versionString.str();
}

XNode &XML_Impl::dtd()
{
  if (parser->canValidate()) {
    for (auto &element : prolog().getChildren()) {
      if (element.isDTD()) { return element; }
    }
  }
  throw Error("No DTD found.");
}

XNode &XML_Impl::prolog()
{
  if (!xmlRoot.isEmpty()) {
    return xmlRoot;
  }
  throw Error("No XML has been parsed.");
}

XNode &XML_Impl::declaration() { return prolog().getChildren()[0]; }

XNode &XML_Impl::root()
{
  for (auto &element : prolog().getChildren()) {
    if (element.isRoot() || element.isSelf()) { return element; }
  }
  throw Error("No root element found.");
}

void XML_Impl::validate() { parser->validate(prolog()); }

void XML_Impl::parse(ISource &source) { xmlRoot = parser->parse(source); }

void XML_Impl::stringify(IDestination &destination) { stringifier->stringify(prolog(), destination); }

void XML_Impl::traverse(IAction &action)
{
  if (xmlRoot.isEmpty()) { throw Error("No XML to traverse."); }
  traverseXNodes(xmlRoot, action);
}
void XML_Impl::traverse(IAction &action) const
{
  if (xmlRoot.isEmpty()) { throw Error("No XML to traverse."); }
  traverseXNodes(xmlRoot, action);
}
}// namespace XML_Lib
