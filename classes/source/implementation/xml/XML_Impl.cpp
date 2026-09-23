//
// Class: XML_Impl
//
// Description: XML class implementation api layer.
//
// Dependencies: C++20 - Language standard features used..
//

#include "XML_Impl.hpp"
#include "XML_Factories.hpp"
#include <sstream>
#include <utility>
#if defined(XML_LIB_ENABLE_XSD)
#include "XSD_Validator.hpp"
#endif

namespace XML_Lib {

/// @brief
/// Return the first child of xNode satisfying predicate, or nullptr.

template<typename Predicate>
/// @brief
/// Find the first child matching the supplied predicate.

static Node *findFirstChild(Node &xNode, Predicate &&predicate)
{
  for (auto &child : xNode.getChildren()) {
    if (predicate(child)) { return &child; }
  }
  return nullptr;
}

/// @brief
/// Initialize XML_Impl with the provided stringifier and parser.

XML_Impl::XML_Impl(IStringify *stringify, IParser *parser)
{
  entityMapper = createDefaultEntityMapper();
  if (parser == nullptr) {
    xmlParser = createDefaultParser(*entityMapper);
  } else {
    xmlParser.reset(parser);
  }
#if defined(XML_LIB_ENABLE_STRINGIFY)
  if (stringify == nullptr) {
    xmlStringifier = createDefaultStringify();
  } else {
    xmlStringifier.reset(stringify);
  }
#else
  (void)stringify;
#endif
  validatorRegistry = createDefaultValidatorRegistry();
#if defined(XML_LIB_ENABLE_XPATH)
  xpathEngine = createDefaultXPathEngine();
#endif
}

/// @brief
/// Destroy XML_Impl and release any owned resources.

XML_Impl::~XML_Impl() = default;

/// @brief
/// Return the XML_Lib version string.

std::string XML_Impl::version()
{
  std::stringstream versionString;
  versionString << "XML_Lib Version " << XML_VERSION_MAJOR << "." << XML_VERSION_MINOR << "." << XML_VERSION_PATCH;
  return versionString.str();
}

#if defined(XML_LIB_ENABLE_DTD)
/// @brief
/// Return the DTD node used for document validation.

Node &XML_Impl::dtd()
{
  if (auto *validatingParser = dynamic_cast<IValidatingParser *>(xmlParser.get())) {
    if (validatingParser->canValidate()) {
      if (Node *found = findFirstChild(prolog(), [](const Node &n) { return isA<DTD>(n); })) {
        return *found;
      }
    }
  }
  XML_LIB_THROW(Error("No DTD found."));
}
#endif

/// @brief
/// Return the XML prolog node.

Node &XML_Impl::prolog()
{
  if (!xmlRoot.isEmpty()) { return xmlRoot; }
  XML_LIB_THROW(Error("No XML has been parsed."));
}

/// @brief
/// Return the XML declaration node.

Node &XML_Impl::declaration() { return prolog().getChildren()[0]; }

/// @brief
/// Return the document root node.

Node &XML_Impl::root()
{
  if (Node *found = findFirstChild(prolog(), [](const Node &n) { return isA<Root>(n) || isA<Self>(n); })) {
    return *found;
  }
  XML_LIB_THROW(Error("No root element found."));
}

#if defined(XML_LIB_ENABLE_DTD)
/// @brief
/// Validate the parsed XML against the attached DTD.

void XML_Impl::validate()
{
  if (auto *validatingParser = dynamic_cast<IValidatingParser *>(xmlParser.get())) {
    validatingParser->validate(prolog());
  } else {
    XML_LIB_THROW(Error("Parser does not support validation."));
  }
}
#endif

void XML_Impl::registerValidator(const std::string_view &schemaType, std::unique_ptr<IValidator> validator)
{
  validatorRegistry->registerValidator(schemaType, std::move(validator));
}

void XML_Impl::validate(const std::string_view &schemaType, const std::string_view &schemaSource)
{
  if (auto *registeredValidator = validatorRegistry->getValidator(schemaType)) {
    BufferSource source(schemaSource);
    registeredValidator->parse(source);
    registeredValidator->validate(root());
    return;
  }
#if defined(XML_LIB_ENABLE_XSD)
  if (schemaType == "xsd" || schemaType == "XSD") {
    XSD_Validator xsdValidator(root());
    BufferSource source(schemaSource);
    xsdValidator.parse(source);
    xsdValidator.validate(root());
    return;
  }
#endif
  XML_LIB_THROW(Error("No validator registered for schema type '" + std::string(schemaType) + "'."));
}

#if defined(XML_LIB_ENABLE_XSD)
/// @brief
/// Validate the parsed XML against an XSD schema string.

void XML_Impl::validate(const std::string_view &xsdSource)
{
  validate("XSD", xsdSource);
}
#endif

#if defined(XML_LIB_ENABLE_XPATH)
void XML_Impl::setXPathEngine(std::unique_ptr<IXPathEngine> engine)
{
  xpathEngine = std::move(engine);
}

/// @brief
/// Evaluate an XPath expression against the current document.

std::vector<const Node *> XML_Impl::xpath(const std::string_view expression)
{
  if (!xpathEngine) {
    xpathEngine = createDefaultXPathEngine();
  }
  return xpathEngine->evaluate(root(), expression);
}
#endif

/// @brief
/// Implementation of XML_Impl::parse.

void XML_Impl::parse(ISource &source, const ParseOptions &options)
{
  XML_Arena::ScopedCurrentArena scopedCurrentArena(arena);
  XML_Arena::ScopedDefaultResource scopedDefaultResource(arena);
  xmlRoot = xmlParser->parse(source, options);
}
#if defined(XML_LIB_ENABLE_STRINGIFY)
/// @brief
/// Serialize the current XML document to the destination.

void XML_Impl::stringify(IDestination &destination) { xmlStringifier->stringify(prolog(), destination, 0); }
#endif

/// @brief
/// Traverse the XML tree and invoke the supplied action.

void XML_Impl::traverse(IAction &action)
{
  std::as_const(*this).traverse(action);
}

/// @brief
/// Traverse the XML tree with a const action handler.

void XML_Impl::traverse(IAction &action) const
{
  if (xmlRoot.isEmpty()) { XML_LIB_THROW(Error("No XML to traverse.")); }
  traverseNodes(xmlRoot, action);
}
}// namespace XML_Lib