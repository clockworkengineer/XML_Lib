#pragma once

#include "XML_Types.hpp"
#include <algorithm>
#include <cstdint>
#include <string>

namespace XML_Lib {

/// @brief Utility component to normalize encodings, BOM markers, and line endings in input streams.
class StreamNormalizer
{
public:
  // Bits per byte
  static constexpr int kBitsPerByte{ 8 };

  /// @brief Convert all CRLF line endings in @p xmlString to LF.
  static void convertCRLFToLF(String &xmlString)
  {
    std::size_t pos = xmlString.find(u"\x0D\x0A");
    while (pos != std::u16string::npos) {
      xmlString.replace(pos, 2, u"\x0A");
      pos = xmlString.find(u"\x0D\x0A", pos + 1);
    }
  }

  /// @brief Strip UTF-16 Byte Order Mark (BOM) if present at the start of @p xmlString.
  static void stripBOM(String &xmlString)
  {
    if (!xmlString.empty() && xmlString.front() == u'\uFEFF') {
      xmlString.erase(xmlString.begin());
    }
  }

  /// @brief Byte-swap UTF-16 string if big-endian encoded `<?xml` prefix is detected.
  static void swapUTF16ByteOrderIfNeeded(std::u16string &utf16xml)
  {
    if (utf16xml.starts_with(u"<?xml")) {
      std::transform(utf16xml.begin(), utf16xml.end(), utf16xml.begin(), [](const char16_t &ch) {
        return static_cast<char16_t>(
          static_cast<uint16_t>(ch) >> kBitsPerByte | static_cast<uint16_t>(ch) << kBitsPerByte);
      });
    }
  }
};

}// namespace XML_Lib
