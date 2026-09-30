#pragma once

#include "XML_Core.hpp"
#include "XML_Destinations.hpp"
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace XML_Lib {

/// @brief High-performance streaming XML serializer emitting directly to an IDestination.
///
/// XMLWriter maintains a constant, negligible memory footprint by streaming XML tokens
/// directly to an output buffer or file, keeping track of open tags to guarantee well-formedness
/// and proper closing.
class XMLWriter
{
public:
  /// @brief Construct an XMLWriter writing to an existing destination stream.
  explicit XMLWriter(IDestination &destination);

  /// @brief Construct an XMLWriter writing to an internal in-memory buffer.
  XMLWriter();

  /// @brief Factory to create an XMLWriter writing directly to a file.
  static XMLWriter toFile(const std::filesystem::path &filePath);

  XMLWriter(const XMLWriter &) = delete;
  XMLWriter &operator=(const XMLWriter &) = delete;
  XMLWriter(XMLWriter &&) noexcept;
  XMLWriter &operator=(XMLWriter &&) noexcept;
  ~XMLWriter();

  /// @brief Enable or disable pretty-printing indentation.
  void setIndent(bool enable, int spaces = 2) noexcept;

  /// @brief Configure whether the XML declaration should be omitted.
  void setOmitXmlDeclaration(bool omit) noexcept { omitDeclaration = omit; }

  /// @brief Write the XML declaration (<?xml version="..." encoding="..."?>).
  void writeStartDocument(std::string_view version = "1.0",
                          std::string_view encoding = "UTF-8",
                          bool standalone = false);

  /// @brief Close all open elements and finish the XML document.
  void writeEndDocument();

  /// @brief Write a start element tag (<name>).
  void writeStartElement(std::string_view name);

  /// @brief Close the current active element tag.
  void writeEndElement();

  /// @brief Write a self-closing empty element (<name/>).
  void writeEmptyElement(std::string_view name);

  /// @brief Convenience method to write a start element, text content, and end element.
  void writeElement(std::string_view name, std::string_view textContent);

  /// @brief Write an attribute onto the currently open start tag.
  /// @throws std::runtime_error if called outside a start tag.
  void writeAttribute(std::string_view name, std::string_view value);

  /// @brief Write text content, automatically escaping XML special characters (&, <, >, ", ').
  void writeCharacters(std::string_view text);

  /// @brief Write an XML comment (<!-- comment -->).
  void writeComment(std::string_view comment);

  /// @brief Write an unparsed CDATA section (<![CDATA[ cdata ]]>).
  void writeCDATA(std::string_view cdata);

  /// @brief Write a processing instruction (<?target data?>).
  void writeProcessingInstruction(std::string_view target, std::string_view data = "");

  /// @brief Write raw XML markup directly to the destination without escaping.
  void writeRaw(std::string_view rawMarkup);

  /// @brief Flush any pending tag closures.
  void flush();

  /// @brief Return the accumulated XML string if constructed with the default internal buffer.
  [[nodiscard]] std::string result() const;

private:
  void closePendingStartTag(bool emptyElement = false);
  void writeIndentation();

  std::unique_ptr<BufferDestination> ownedBufferDest;
  std::unique_ptr<FileDestination> ownedFileDest;
  IDestination *dest{ nullptr };

  struct ElementEntry
  {
    std::string name;
    bool hasChildElements{ false };
  };
  std::vector<ElementEntry> openElements;
  bool inStartTag{ false };
  bool indentEnabled{ false };
  int indentSpaces{ 2 };
  int currentIndentLevel{ 0 };
  bool omitDeclaration{ false };
  bool documentStarted{ false };
  bool lastWasStartElement{ false };
};

} // namespace XML_Lib
