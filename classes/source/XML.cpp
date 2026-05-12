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

namespace XML_Lib {

/// <summary>
/// XML constructor.
/// </summary>
XML::XML(IStringify *stringify, IParser *parser) : implementation(std::make_unique<XML_Impl>(stringify, parser)) {}

/// <summary>
/// XML constructor (parse in a default XML string).
/// </summary>
XML::XML(const std::string_view &xmlString) : XML() { parse(BufferSource{ xmlString }); }

/// <summary>
/// XML assignment (parse in a default XML string).
/// </summary>
XML &XML::operator=(const std::string_view &xmlString)
{
  parse(BufferSource{ xmlString });
  return *this;
}

/// <summary>
/// XML destructor.
/// </summary>
XML::~XML() = default;

/// <summary>
/// Get XML_Lib version.
/// </summary>
/// <returns>Library version string.</returns>
std::string XML::version() { return XML_Impl::version(); }

#if defined(XML_LIB_ENABLE_DTD)
/// <summary>
/// Return reference to any internal DTD_Validator for parsed XML.
/// </summary>
/// <returns>Reference to DTD_Validator Node.</returns>
Node &XML::dtd() const { return implementation->dtd(); }
#endif

/// <summary>
/// Return prolog node of the parsed XML tree.
/// </summary>
/// <returns>Reference to prolog Node.</returns>
Node &XML::prolog() const { return implementation->prolog(); }

/// <summary>
/// Return declaration node of the parsed XML tree.
/// </summary>
/// <returns>Reference to declaration Node.</returns>
Node &XML::declaration() const { return implementation->declaration(); }

/// <summary>
/// Return root node of parsed XML tree.
/// </summary>
/// <returns>Reference to root element Node.</returns>
Node &XML::root() const { return implementation->root(); }

#if defined(XML_LIB_ENABLE_DTD)
/// <summary>
/// Validate XML against any DTD_Validator provided to see whether it is valid. If an
/// exception is thrown, then there is a validation issue and the XML is not valid.
/// </summary>
void XML::validate() const { implementation->validate(); }
#endif

#if defined(XML_LIB_ENABLE_XSD)
/// <summary>
/// Validate XML against an XSD schema supplied as a UTF-8 XML string.
/// </summary>
/// <param name="xsdSource">XSD schema XML string.</param>
void XML::validate(const std::string_view &xsdSource) const { implementation->validate(xsdSource); }
#endif

#if defined(XML_LIB_ENABLE_XPATH)
/// <summary>
/// Evaluate an XPath 1.0 expression against the parsed document.
/// </summary>
/// <param name="expression">XPath expression string.</param>
/// <returns>Node pointers matching the expression (into the internal node tree).</returns>
std::vector<const Node *> XML::xpath(const std::string_view expression) const
{
  return implementation->xpath(expression);
}
#endif

/// <summary>
/// Parse XML read from source stream into internal object generating an exception
/// if a syntax error in the XML is found (not well-formed).
/// </summary>
void XML::parse(ISource &source, const ParseOptions &options) const { implementation->parse(source, options); }
void XML::parse(ISource &&source, const ParseOptions &options) const { implementation->parse(source, options); }

/// <summary>
/// Convenience overload: parse XML directly from a string without needing a BufferSource.
/// </summary>
void XML::parse(const char *xmlString, const ParseOptions &options) const
{
  BufferSource source{ std::string_view{ xmlString } };
  implementation->parse(source, options);
}
void XML::parse(const std::string_view &xmlString, const ParseOptions &options) const
{
  BufferSource source{ xmlString };
  implementation->parse(source, options);
}

/// <summary>
/// Convenience overload: parse XML directly from a file path without needing a FileSource.
/// </summary>
void XML::parse(const std::filesystem::path &filePath, const ParseOptions &options) const
{
  FileSource source{ filePath.string() };
  implementation->parse(source, options);
}

/// <summary>
/// Create XML text from an XML object.
/// </summary>
#if defined(XML_LIB_ENABLE_STRINGIFY)
void XML::stringify(IDestination &destination) const { implementation->stringify(destination); }
void XML::stringify(IDestination &&destination) const { implementation->stringify(destination); }

/// <summary>
/// Convenience overload: stringify XML to a string without needing a BufferDestination.
/// </summary>
std::string XML::stringify() const
{
  BufferDestination destination;
  implementation->stringify(destination);
  return destination.toString();
}

/// <summary>
/// Convenience overload: stringify XML directly to a file path without needing a FileDestination.
/// </summary>
void XML::stringify(const std::filesystem::path &filePath, const Format format) const
{
  BufferDestination buffer;
  implementation->stringify(buffer);
  XML_Impl::toFile(filePath.string(), buffer.toString(), format);
}
#endif

/// <summary>
/// Recursively traverse Node structure calling IAction methods (read-only)
///  or to change the XML tree node directly.
/// </summary>
/// <param name="action">Action methods to call during traversal.</param>
/// Traverse using non-const JSON so can change JSON tree
void XML::traverse(IAction &action) { implementation->traverse(action); }
// Traverse using const JSON so cannot change JSON tree
void XML::traverse(IAction &action) const { std::as_const(*implementation).traverse(action); }

/// <summary>
/// Open an XML file, read its contents into a string buffer and return
/// the buffer.
/// </summary>
/// <param name="fileName">XML file name</param>
/// <returns>XML string.</returns>
std::string XML::fromFile(const std::string_view &fileName) { return XML_Impl::fromFile(fileName); }

/// <summary>
/// Create an XML file and write XML string to it.
/// </summary>
/// <param name="fileName">XML file name</param>
/// <param name="xmlString">XML string</param>
/// <param name="format">XML file format</param>
void XML::toFile(const std::string_view &fileName, const std::string_view &xmlString, const Format format)
{
  XML_Impl::toFile(fileName, xmlString, format);
}
/// <summary>
/// Return format of the XML file.
/// </summary>
/// <param name="fileName">XML file name</param>
/// <returns>XML file format.</returns>
XML::Format XML::getFileFormat(const std::string_view &fileName) { return XML_Impl::getFileFormat(fileName); }
}// namespace XML_Lib
