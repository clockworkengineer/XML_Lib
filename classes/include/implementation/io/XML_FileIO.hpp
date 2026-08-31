#pragma once

#include "XML.hpp"
#include <filesystem>
#include <string>
#include <string_view>

namespace XML_Lib {

/// @brief Dedicated service class for XML file I/O and format detection (SRP compliant).
class XML_FileIO
{
public:
  /// @brief Read the entire content of @p filePath into a `std::string`.
  [[nodiscard]] static std::string fromFile(const std::filesystem::path &filePath);

  /// @brief Write @p xmlString to @p filePath using the specified encoding format.
  static void toFile(const std::filesystem::path &filePath, const std::string_view &xmlString, XML::Format format = XML::Format::utf8);

  /// @brief Detect the encoding format of an XML file by inspecting its BOM.
  [[nodiscard]] static XML::Format getFileFormat(const std::string_view &fileName);
};

} // namespace XML_Lib
