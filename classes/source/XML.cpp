//
// Class: XML
//
// Description: Parse XML into an internal XML Object (generating an exception
// if it is found not to be syntactically correct "well-formed") so that it can
// be manipulated, interrogated and saved back to a text form (stringify). For more
// information on the XML standard, check out https://www.w3.org/TR/REC-xml/.
//
// Dependencies: C++20 - Language standard features used.
//

#include "XML_Impl.hpp"
#include "implementation/io/XML_FileIO.hpp"

namespace XML_Lib {

/// @brief
/// XML constructor.

XML::XML(IStringify *stringify, IParser *parser) : implementation(std::make_unique<XML_Impl>(stringify, parser)) {}

/// @brief
/// XML constructor (parse in a default XML string).

XML::XML(const std::string_view &xmlString) : XML() { parse(BufferSource{ xmlString }); }

/// @brief
/// XML assignment (parse in a default XML string).

XML &XML::operator=(const std::string_view &xmlString)
{
  parse(BufferSource{ xmlString });
  return *this;
}

/// @brief
/// XML destructor.

XML::~XML() noexcept = default;

/// @brief
/// Get XML_Lib version.

/// @return Library version string.
std::string XML::version() { return XML_Impl::version(); }

#if defined(XML_LIB_ENABLE_DTD)
/// @brief
/// Return reference to any internal DTD_Validator for parsed XML.

/// @return Reference to DTD_Validator Node.
Node &XML::dtd() const { return implementation->dtd(); }
#endif

/// @brief
/// Return prolog node of the parsed XML tree.

/// @return Reference to prolog Node.
Node &XML::prolog() const { return implementation->prolog(); }

/// @brief
/// Return declaration node of the parsed XML tree.

/// @return Reference to declaration Node.
Node &XML::declaration() const { return implementation->declaration(); }

/// @brief
/// Return root node of parsed XML tree.

/// @return Reference to root element Node.
Node &XML::root() const { return implementation->root(); }

#if defined(XML_LIB_ENABLE_DTD)
/// @brief
/// Validate XML against any DTD_Validator provided to see whether it is valid. If an
/// exception is thrown, then there is a validation issue and the XML is not valid.

void XML::validate() const { implementation->validate(); }
#endif

#if defined(XML_LIB_ENABLE_XSD)
/// @brief
/// Validate XML against an XSD schema supplied as a UTF-8 XML string.

/// @param xsdSource XSD schema XML string.
void XML::validate(const std::string_view &xsdSource) const { implementation->validate(xsdSource); }
#endif

#if defined(XML_LIB_ENABLE_XPATH)
/// @brief
/// Evaluate an XPath 1.0 expression against the parsed document.

/// @param expression XPath expression string.
/// @return Node pointers matching the expression (into the internal node tree).
std::vector<const Node *> XML::xpath(const std::string_view expression) const
{
  return implementation->xpath(expression);
}
#endif

/// @brief
/// Parse XML read from source stream into internal object generating an exception
/// if a syntax error in the XML is found (not well-formed).

void XML::parse(ISource &source, const ParseOptions &options) const { implementation->parse(source, options); }
/// @brief
/// Parse XML from an rvalue source.

void XML::parse(ISource &&source, const ParseOptions &options) const { implementation->parse(source, options); }

/// @brief
/// Convenience overload: parse XML directly from a string without needing a BufferSource.

void XML::parse(const char *xmlString, const ParseOptions &options) const
{
  if (!xmlString) { XML_LIB_THROW(SyntaxError("Null XML string passed to parse().")); }
  const std::size_t length = std::strlen(xmlString);
  if (length > options.maxXmlSize) {
    XML_LIB_THROW(SyntaxError("XML input exceeds maximum allowed size."));
  }
  BufferSource source{ std::string_view{ xmlString }, options.maxXmlSize };
  implementation->parse(source, options);
}
/// @brief
/// Parse XML from a string view.

void XML::parse(const std::string_view &xmlString, const ParseOptions &options) const
{
  if (xmlString.size() > options.maxXmlSize) {
    XML_LIB_THROW(SyntaxError("XML input exceeds maximum allowed size."));
  }
  BufferSource source{ xmlString, options.maxXmlSize };
  implementation->parse(source, options);
}

/// @brief
/// Convenience overload: parse XML directly from a file path without needing a FileSource.

void XML::parse(const std::filesystem::path &filePath, const ParseOptions &options) const
{
  const auto fileSize = std::filesystem::file_size(filePath);
  if (static_cast<std::size_t>(fileSize) > options.maxXmlSize) {
    XML_LIB_THROW(SyntaxError("XML input exceeds maximum allowed size."));
  }
  const std::string xmlString = XML_Impl::fromFile(filePath);
  if (xmlString.size() > options.maxXmlSize) {
    XML_LIB_THROW(SyntaxError("XML input exceeds maximum allowed size."));
  }
  BufferSource source{ xmlString, options.maxXmlSize };
  implementation->parse(source, options);
}

/// @brief
/// Create XML text from an XML object.

#if defined(XML_LIB_ENABLE_STRINGIFY)
/// @brief
/// Serialize XML to the provided destination.

void XML::stringify(IDestination &destination) const { implementation->stringify(destination); }
/// @brief
/// Serialize XML to a temporary destination.

void XML::stringify(IDestination &&destination) const { implementation->stringify(destination); }

/// @brief
/// Convenience overload: stringify XML to a string without needing a BufferDestination.

std::string XML::stringify() const
{
  BufferDestination destination;
  implementation->stringify(destination);
  return destination.toString();
}

/// @brief
/// Convenience overload: stringify XML directly to a file path without needing a FileDestination.

void XML::stringify(const std::filesystem::path &filePath, const Format format) const
{
  BufferDestination buffer;
  implementation->stringify(buffer);
  XML_Impl::toFile(filePath, buffer.toString(), format);
}
#endif

/// @brief
/// Recursively traverse Node structure calling IAction methods (read-only)
///  or to change the XML tree node directly.

/// @param action Action methods to call during traversal.
/// Traverse using non-const JSON so can change JSON tree
void XML::traverse(IAction &action) { implementation->traverse(action); }
// Traverse using const JSON so cannot change JSON tree
void XML::traverse(IAction &action) const { std::as_const(*implementation).traverse(action); }

/// @brief
/// Open an XML file, read its contents into a string buffer and return
/// the buffer.

/// @param fileName XML file name
/// @return XML string.
std::string XML::fromFile(const std::filesystem::path &filePath) { return XML_FileIO::fromFile(filePath); }

void XML::toFile(const std::filesystem::path &filePath, const std::string_view &xmlString, const Format format)
{
  XML_FileIO::toFile(filePath, xmlString, format);
}

XML::Format XML::getFileFormat(const std::string_view &fileName) { return XML_FileIO::getFileFormat(fileName); }
} // namespace XML_Lib
