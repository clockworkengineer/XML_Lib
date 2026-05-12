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

class XML
{
public:
  // XML file formats
  enum class Format : uint8_t { utf8 = 0, utf8BOM, utf16BE, utf16LE, utf32BE, utf32LE };
  // Constructors/Destructors
  explicit XML(IStringify *stringify = nullptr, IParser *parser = nullptr);
  // Pass in default XML to parse
  explicit XML(const std::string_view &xmlString);
  XML(const XML &other) = delete;
  XML &operator=(const XML &other) = delete;
  XML &operator=(const std::string_view &xmlString);
  XML(XML &&other) = delete;
  XML &operator=(XML &&other) = delete;
  ~XML();
  // Get reference to prolog Node
  [[nodiscard]] Node &prolog() const;
  // Get reference to declaration Node
  [[nodiscard]] Node &declaration() const;
  // Get reference to root element Node
  [[nodiscard]] Node &root() const;
#if defined(XML_LIB_ENABLE_DTD)
  // Get reference to DTD_Validator Node
  [[nodiscard]] Node &dtd() const;
  // Validate XML against DTD (if present)
  void validate() const;
#endif
#if defined(XML_LIB_ENABLE_XSD)
  // Validate XML against an XSD schema supplied as a string or file path
  void validate(const std::string_view &xsdSource) const;
#endif
#if defined(XML_LIB_ENABLE_XPATH)
  // Evaluate an XPath 1.0 expression against the parsed document
  [[nodiscard]] std::vector<const Node *> xpath(std::string_view expression) const;
#endif
  // Return version string
  [[nodiscard]] static std::string version();
  // Parse XML source
  void parse(ISource &source) const;
  void parse(ISource &&source) const;
  // Parse XML from a plain string (convenience — no ISource needed)
  void parse(const char *xmlString) const;
  void parse(const std::string_view &xmlString) const;
  // Parse XML from a file path (convenience — no ISource needed)
  void parse(const std::filesystem::path &filePath) const;
#if defined(XML_LIB_ENABLE_STRINGIFY)
  // Stringify XML object to the destination text
  void stringify(IDestination &destination) const;
  void stringify(IDestination &&destination) const;
  // Stringify XML to a string (convenience — no IDestination needed)
  [[nodiscard]] std::string stringify() const;
  // Stringify XML to a file path (convenience — no IDestination needed)
  void stringify(const std::filesystem::path &filePath, Format format = Format::utf8) const;
#endif
  // Traverse the XML tree
  void traverse(IAction &action);
  void traverse(IAction &action) const;
  // Read/Write XML to/from the file
  [[nodiscard]] static std::string fromFile(const std::string_view &fileName);
  static void toFile(const std::string_view &fileName, const std::string_view &xmlString, Format format = Format::utf8);
  // Get an XML file format
  [[nodiscard]] static Format getFileFormat(const std::string_view &fileName);

private:
  const std::unique_ptr<XML_Impl> implementation;
};
}// namespace XML_Lib
