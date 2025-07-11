#pragma once

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
  // Get reference to DTD_Validator Node
  [[nodiscard]] Node &dtd() const;
  // Get reference to prolog Node
  [[nodiscard]] Node &prolog() const;
  // Get reference to declaration Node
  [[nodiscard]] Node &declaration() const;
  // Get reference to root element Node
  [[nodiscard]] Node &root() const;
  // Validate XML
  void validate() const;
  // Return version string
  [[nodiscard]] static std::string version();
  // Parse XML source
  void parse(ISource &source) const;
  void parse(ISource &&source) const;
  // Stringify XML object to the destination text
  void stringify(IDestination &destination) const;
  void stringify(IDestination &&destination) const;
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
