#pragma once

#include "XML_Core.hpp"
#include "XML_Expected.hpp"
#include "XML_Sources.hpp"
#include <expected>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace XML_Lib {

/// @brief Forward-only streaming pull-parser cursor for processing XML documents in O(1) memory.
///
/// XMLReader reads tokens sequentially from an input character stream (ISource) without
/// constructing an in-memory DOM tree. It enables parsing multi-gigabyte XML files with
/// a constant, negligible memory footprint (< 10 MB).
class XMLReader
{
public:
  /// @brief XML node / event types encountered during pull parsing.
  enum class NodeType {
    None,                   ///< Initial state before read() is called.
    Declaration,            ///< XML declaration <?xml version="1.0" ... ?>
    ElementStart,           ///< Start element tag <tag attr="val">
    ElementEnd,             ///< End element tag </tag> or closing of self-closing tag
    Text,                   ///< Character text content inside an element
    CDATA,                  ///< CDATA section <![CDATA[ ... ]]>
    Comment,                ///< XML comment <!-- ... -->
    ProcessingInstruction,  ///< Processing instruction <?target data?>
    DTD,                    ///< Document type declaration <!DOCTYPE ...>
    Whitespace,             ///< Ignorable inter-element whitespace
    EndDocument             ///< End of document / stream reached
  };

  /// @brief Construct an XMLReader wrapping an existing character source.
  explicit XMLReader(ISource &source);

  /// @brief Construct an XMLReader parsing directly from an XML string buffer.
  explicit XMLReader(std::string_view xmlString);

  /// @brief Factory to create an XMLReader from a file path.
  static XMLReader fromFile(const std::filesystem::path &filePath);

  XMLReader(const XMLReader &) = delete;
  XMLReader &operator=(const XMLReader &) = delete;
  XMLReader(XMLReader &&) noexcept;
  XMLReader &operator=(XMLReader &&) noexcept;
  ~XMLReader();

  /// @brief Return the string representation of a NodeType using C++23 std::unreachable.
  [[nodiscard]] static constexpr std::string_view nodeTypeToString(NodeType type) noexcept
  {
    switch (type) {
    case NodeType::None: return "None";
    case NodeType::Declaration: return "Declaration";
    case NodeType::ElementStart: return "ElementStart";
    case NodeType::ElementEnd: return "ElementEnd";
    case NodeType::Text: return "Text";
    case NodeType::CDATA: return "CDATA";
    case NodeType::Comment: return "Comment";
    case NodeType::ProcessingInstruction: return "ProcessingInstruction";
    case NodeType::DTD: return "DTD";
    case NodeType::Whitespace: return "Whitespace";
    case NodeType::EndDocument: return "EndDocument";
    }
    std::unreachable();
  }

  /// @brief Advance the cursor to the next XML token/node in the stream.
  /// @return `true` if a new node was read; `false` if end of document was reached.
  bool read();

  /// @brief Advance the cursor returning std::expected (non-throwing C++23 API).
  std::expected<bool, XML_Error> readExpected() noexcept;

  /// @brief Return the type of the current node.
  [[nodiscard]] NodeType nodeType() const noexcept { return type; }

  /// @brief Return the name of the current node (element tag name, PI target, or DTD root).
  [[nodiscard]] std::string_view name() const noexcept { return currentName; }

  /// @brief Return the value/content of the current node (text, CDATA, comment, PI data).
  [[nodiscard]] std::string_view value() const noexcept { return currentValue; }

  /// @brief Return `true` if the current start element was an empty / self-closing tag (<tag/>).
  [[nodiscard]] bool isEmptyElement() const noexcept { return isSelfClosing; }

  /// @brief Return current element nesting depth (0 at root element).
  [[nodiscard]] int depth() const noexcept { return currentDepth; }

  /// @brief Return number of attributes on the current start element.
  [[nodiscard]] std::size_t attributeCount() const noexcept { return currentAttributes.size(); }

  /// @brief Return the value of an attribute by name, or empty string_view if not found.
  [[nodiscard]] std::string_view getAttribute(std::string_view attrName) const noexcept;

  /// @brief Find an attribute value by name returning std::optional (monadic C++23 API).
  [[nodiscard]] std::optional<std::string_view> findAttribute(std::string_view attrName) const noexcept;

  /// @brief Return `true` if an attribute with the given name exists on current element.
  [[nodiscard]] bool hasAttribute(std::string_view attrName) const noexcept;

  /// @brief Return the value of an attribute by zero-based index.
  [[nodiscard]] std::string_view getAttribute(std::size_t index) const;

  /// @brief Return the name of an attribute by zero-based index.
  [[nodiscard]] std::string_view getAttributeName(std::size_t index) const;

  /// @brief Return all attributes on the current start element as name/value pairs.
  [[nodiscard]] const std::vector<std::pair<std::string, std::string>> &attributes() const noexcept
  {
    return currentAttributes;
  }

  /// @brief Return current stream position as {line, column}.
  [[nodiscard]] std::pair<long, long> getPosition() const;

  /// @brief Configure whether pure whitespace between elements should be skipped automatically.
  void setSkipWhitespace(bool skip) noexcept { skipWhitespace = skip; }

  /// @brief Advance the cursor until the next ElementStart node is found.
  /// @return `true` if found, `false` if end of document reached.
  bool readToNextElement();

  /// @brief Read all text inside the current element until its matching ElementEnd.
  /// The cursor will be positioned on the ElementEnd node upon completion.
  std::string readElementText();

  /// @brief Skip the entire subtree of the current element and advance to its matching ElementEnd.
  void skip();

private:
  void parseElementStart();
  void parseCloseTag();
  void parseCommentOrPIOrCDATA();
  void parseTextContent();
  void parseDeclaration();

  std::unique_ptr<BufferSource> ownedBufferSource;
  std::unique_ptr<FileSource> ownedFileSource;
  ISource *sourceStream{ nullptr };

  NodeType type{ NodeType::None };
  std::string currentName;
  std::string currentValue;
  std::vector<std::pair<std::string, std::string>> currentAttributes;
  std::vector<std::string> openElements;

  int currentDepth{ 0 };
  bool isSelfClosing{ false };
  bool pendingEndElement{ false };
  bool atEnd{ false };
  bool skipWhitespace{ false };
};

} // namespace XML_Lib
