//
// Class: XML
//
// Description: Parse XML into an internal XML Object (generating an exception
// if it is found not to be syntactically correct "well-formed") so that it can
// be manipulated, interrogated and saved back to a text form (stringify). For more
// information on the XML standard check out https://www.w3.org/TR/REC-xml/.
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
XML::XML(const std::string &xmlString) : XML() { parse(BufferSource{ xmlString }); }

/// <summary>
/// XML assignment (parse in a default XML string).
/// </summary>
XML &XML::operator=(const std::string &xmlString) { parse(BufferSource{ xmlString }); return(*this);}

/// <summary>
/// XML destructor.
/// </summary>
XML::~XML() = default;

/// <summary>
/// Get XML_Lib version.
/// </summary>
/// <returns>Library version string.</returns>
std::string XML::version() const { return implementation->version(); }

/// <summary>
/// Return reference to any internal DTD_Validator for parsed XML.
/// </summary>
/// <returns>Reference to DTD_Validator XNode.</returns>
XNode &XML::dtd() const { return implementation->dtd(); }

/// <summary>
/// Return prolog node of parsed XML tree.
/// </summary>
/// <returns>Reference to prolog XNode.</returns>
XNode &XML::prolog() const { return implementation->prolog(); }

/// <summary>
/// Return declaration node of parsed XML tree.
/// </summary>
/// <returns>Reference to declaration XNode.</returns>
XNode &XML::declaration() const { return implementation->declaration(); }

/// <summary>
/// Return root node of parsed XML tree.
/// </summary>
/// <returns>Reference to root element XNode.</returns>
XNode &XML::root() const { return implementation->root(); }

/// <summary>
/// Validate XML against any DTD_Validator provided to see whether it is valid. If an
/// exception is thrown then there is a validation issue and the XML is not valid.
/// </summary>
void XML::validate() const { implementation->validate(); }

/// <summary>
/// Parse XML read from source stream into internal object generating an exception
/// if a syntax error in the XML is found (not well formed).
/// </summary>
void XML::parse(ISource &source) const { implementation->parse(source); }
void XML::parse(ISource &&source) const { implementation->parse(source); }

/// <summary>
/// Create XML text from an XML object.
/// </summary>
/// <param name="destination">XML destination stream.</param>
void XML::stringify(IDestination &destination) const { implementation->stringify(destination); }
void XML::stringify(IDestination &&destination) const { implementation->stringify(destination); }

/// <summary>
/// Recursively traverse XNode structure calling IAction methods (read only)
///  or to change the XML tree node directly.
/// </summary>
/// <param name="action">Action methods to call during traversal.</param>
/// Traverse using non-const JSON so can change JSON tree
void XML::traverse(IAction &action) { implementation->traverse(action); }
// Traverse using const JSON so cannot change JSON tree
void XML::traverse(IAction &action) const { std::as_const(*implementation).traverse(action); }

/// <summary>
/// Open a XML file, read its contents into a string buffer and return
/// the buffer.
/// </summary>
/// <param name="fileName">XML file name</param>
/// <returns>XML string.</returns>
std::string XML::fromFile(const std::string &fileName) { return XML_Impl::fromFile(fileName); }

/// <summary>
/// Create an XML file and write XML string to it.
/// </summary>
/// <param name="fileName">XML file name</param>
/// <param name="xmlString">XML string</param>
/// <param name="format">XML file format</param>
void XML::toFile(const std::string &fileName, const std::string &xmlString, const Format format)
{
  XML_Impl::toFile(fileName, xmlString, format);
}
/// <summary>
/// Return format of XML file.
/// </summary>
/// <param name="fileName">XML file name</param>
/// <returns>XML file format.</returns>
XML::Format XML::getFileFormat(const std::string &fileName) { return XML_Impl::getFileFormat(fileName); }
}// namespace XML_Lib
