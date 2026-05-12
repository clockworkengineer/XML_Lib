#pragma once

#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace XML_Lib {

// ========================
// XML forward declarations
// ========================
class IStringify;
class IParser;
class ISource;
class IDestination;
class IAction;
class XML_Impl;
struct Node;

/// @brief Top-level XML document class.
///
/// Owns the parsed document tree (prolog, declaration, root element, optional DTD).
/// Uses the PImpl pattern — construction is lightweight; the heavy implementation
/// lives in XML_Impl.
///
/// @note Copying and moving are disabled; hold by value or via `std::unique_ptr`.
class XML
{
public:
  /// @brief Serialisation output formats.
  enum class Format : uint8_t { utf8 = 0, utf8BOM, utf16BE, utf16LE, utf32BE, utf32LE };

  /// @brief Construct an XML document, optionally providing custom stringifier/parser.
  /// @param stringify Custom stringifier; pass `nullptr` to use the default.
  /// @param parser    Custom parser;      pass `nullptr` to use the default.
  explicit XML(IStringify *stringify = nullptr, IParser *parser = nullptr);

  /// @brief Construct and immediately parse @p xmlString.
  /// @param xmlString UTF-8 XML text to parse.
  explicit XML(const std::string_view &xmlString);

  XML(const XML &other) = delete;
  XML &operator=(const XML &other) = delete;

  /// @brief Assign (parse) a new XML string, replacing the current document.
  XML &operator=(const std::string_view &xmlString);

  XML(XML &&other) = delete;
  XML &operator=(XML &&other) = delete;

  ~XML();

  /// @brief Return a reference to the prolog node (the `<?xml …?>` processing instruction and surrounding whitespace/comments).
  [[nodiscard]] Node &prolog() const;

  /// @brief Return a reference to the XML declaration node.
  [[nodiscard]] Node &declaration() const;

  /// @brief Return a reference to the document root element node.
  [[nodiscard]] Node &root() const;

#if defined(XML_LIB_ENABLE_DTD)
  /// @brief Return a reference to the DTD node (only present when a `<!DOCTYPE …>` was parsed).
  [[nodiscard]] Node &dtd() const;

  /// @brief Validate the document against the embedded DTD.
  /// @throws DTD_Validator::Error if validation fails.
  void validate() const;
#endif

#if defined(XML_LIB_ENABLE_XSD)
  /// @brief Validate the document against an XSD schema.
  /// @param xsdSource Inline XSD text or a file path to an `.xsd` file.
  /// @throws XSD_Validator::Error if validation fails.
  void validate(const std::string_view &xsdSource) const;
#endif

#if defined(XML_LIB_ENABLE_XPATH)
  /// @brief Evaluate an XPath 1.0 expression and return matching nodes.
  /// @param expression XPath expression string.
  /// @return Pointers into the document tree — do not store beyond the XML object's lifetime.
  [[nodiscard]] std::vector<const Node *> xpath(std::string_view expression) const;
#endif

  /// @brief Return the library version string (e.g. `"1.2.0"`).
  [[nodiscard]] static std::string version();

  /// @brief Parse XML from an lvalue source stream.
  void parse(ISource &source) const;

  /// @brief Parse XML from an rvalue (temporary) source stream.
  void parse(ISource &&source) const;

  /// @brief Parse XML from a null-terminated C string (convenience overload).
  /// @param xmlString Null-terminated UTF-8 XML text.
  void parse(const char *xmlString) const;

  /// @brief Parse XML from a `string_view` (convenience overload).
  /// @param xmlString UTF-8 XML text view.
  void parse(const std::string_view &xmlString) const;

  /// @brief Parse XML from a file (convenience overload).
  /// @param filePath Path to the XML file to read and parse.
  void parse(const std::filesystem::path &filePath) const;

#if defined(XML_LIB_ENABLE_STRINGIFY)
  /// @brief Serialise the document tree to an lvalue destination.
  void stringify(IDestination &destination) const;

  /// @brief Serialise the document tree to an rvalue (temporary) destination.
  void stringify(IDestination &&destination) const;

  /// @brief Serialise the document tree and return it as a `std::string` (convenience overload).
  [[nodiscard]] std::string stringify() const;

  /// @brief Serialise the document tree directly to a file (convenience overload).
  /// @param filePath Destination file path.
  /// @param format   Output encoding; defaults to UTF-8 without BOM.
  void stringify(const std::filesystem::path &filePath, Format format = Format::utf8) const;
#endif

  /// @brief Traverse the document tree, calling the appropriate `IAction::on*` callback for each node.
  void traverse(IAction &action);

  /// @brief Traverse the document tree (const overload).
  void traverse(IAction &action) const;

  /// @brief Read the entire content of @p fileName into a `std::string`.
  [[nodiscard]] static std::string fromFile(const std::string_view &fileName);

  /// @brief Write @p xmlString to @p fileName using the specified encoding.
  static void toFile(const std::string_view &fileName, const std::string_view &xmlString, Format format = Format::utf8);

  /// @brief Detect the encoding format of an XML file by inspecting its BOM.
  [[nodiscard]] static Format getFileFormat(const std::string_view &fileName);

private:
  const std::unique_ptr<XML_Impl> implementation;
};
}// namespace XML_Lib
