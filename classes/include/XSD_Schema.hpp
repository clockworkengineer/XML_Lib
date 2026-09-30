#pragma once

#if defined(XML_LIB_ENABLE_XSD)

#include <filesystem>
#include <memory>
#include <string_view>

namespace XML_Lib {

class ISource;
struct Node;
struct XSD_SchemaDefinition;

/// @brief Pre-compiled, immutable, thread-safe W3C XML Schema (XSD) definition.
///
/// Parses and compiles an XSD schema once into memory. Can be reused to validate
/// millions of XML documents concurrently without re-parsing the schema XML or
/// re-constructing validation tables.
class XSD_Schema
{
public:
  /// @brief Compile an XSD schema from an ISource character stream.
  explicit XSD_Schema(ISource &source);

  /// @brief Compile an XSD schema from an inline XML string or file path.
  explicit XSD_Schema(std::string_view schemaSource);

  /// @brief Compile an XSD schema from an existing filesystem path.
  static XSD_Schema fromFile(const std::filesystem::path &filePath);

  XSD_Schema(const XSD_Schema &other) = default;
  XSD_Schema &operator=(const XSD_Schema &other) = default;
  XSD_Schema(XSD_Schema &&other) noexcept = default;
  XSD_Schema &operator=(XSD_Schema &&other) noexcept = default;
  ~XSD_Schema();

  /// @brief Validate an XML document or node tree against this compiled schema.
  /// @throws XSD_Validator::Error if validation fails.
  void validate(const Node &xNode) const;

  /// @brief Internal compiled schema definition accessor.
  [[nodiscard]] const std::shared_ptr<const XSD_SchemaDefinition> &definition() const noexcept { return schemaDef; }

private:
  std::shared_ptr<const XSD_SchemaDefinition> schemaDef;
};

} // namespace XML_Lib

#endif // XML_LIB_ENABLE_XSD
