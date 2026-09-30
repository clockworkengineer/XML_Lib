//
// Class: Converter
//
// Description: Convert characters to/from UTF8/UTF16 for POSIX/Standard platforms.
//
// Dependencies: C++20 - Language standard features used.
//

#include "XML_Converter.hpp"
#include "common/XML_Error.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>

namespace XML_Lib {

/// @brief
/// Convert a UTF-16 string or view to UTF-8.
/// Completely re-entrant, thread-safe, fast-paths pure ASCII runs, and validates surrogate code points.

std::string toUtf8(std::u16string_view utf16)
{
  const std::size_t len = utf16.size();
  std::size_t i = 0;
  while (i < len && utf16[i] <= 0x7F) {
    ++i;
  }
  if (i == len) {
    std::string utf8(len, '\0');
    for (std::size_t j = 0; j < len; ++j) {
      utf8[j] = static_cast<char>(utf16[j]);
    }
    return utf8;
  }

  std::string utf8;
  utf8.reserve(len * 3 / 2);
  for (std::size_t j = 0; j < i; ++j) {
    utf8.push_back(static_cast<char>(utf16[j]));
  }
  for (; i < len; ++i) {
    char32_t cp = utf16[i];
    // Check for surrogate pairs (0xD800 - 0xDFFF)
    if (cp >= 0xD800 && cp <= 0xDBFF) {
      if (i + 1 < len) {
        const char32_t low = utf16[i + 1];
        if (low >= 0xDC00 && low <= 0xDFFF) {
          cp = 0x10000 + (((cp - 0xD800) << 10) | (low - 0xDC00));
          ++i;
        } else {
          XML_LIB_THROW(std::range_error("Unpaired UTF-16 high surrogate encountered."));
        }
      } else {
        XML_LIB_THROW(std::range_error("Unpaired UTF-16 high surrogate at end of stream."));
      }
    } else if (cp >= 0xDC00 && cp <= 0xDFFF) {
      XML_LIB_THROW(std::range_error("Unpaired UTF-16 low surrogate encountered."));
    }

    // Encode code point into UTF-8
    if (cp <= 0x7F) {
      utf8.push_back(static_cast<char>(cp));
    } else if (cp <= 0x7FF) {
      utf8.push_back(static_cast<char>(0xC0 | (cp >> 6)));
      utf8.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    } else if (cp <= 0xFFFF) {
      utf8.push_back(static_cast<char>(0xE0 | (cp >> 12)));
      utf8.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
      utf8.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    } else if (cp <= 0x10FFFF) {
      utf8.push_back(static_cast<char>(0xF0 | (cp >> 18)));
      utf8.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
      utf8.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
      utf8.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    } else {
      XML_LIB_THROW(std::range_error("Code point exceeds 0x10FFFF."));
    }
  }
  return utf8;
}

/// @brief
/// Convert a UTF-8 string or view to UTF-16.
/// Fast-paths pure ASCII and enforces strict RFC 3629 / W3C XML Unicode validation rules (rejects overlong sequences,
/// surrogate code points, out-of-range scalars > 0x10FFFF, and invalid continuation bytes).

std::u16string toUtf16(std::string_view utf8)
{
  const std::size_t len = utf8.size();
  std::size_t i = 0;
  while (i < len && static_cast<uint8_t>(utf8[i]) <= 0x7F) {
    ++i;
  }
  if (i == len) {
    std::u16string utf16(len, u'\0');
    for (std::size_t j = 0; j < len; ++j) {
      utf16[j] = static_cast<char16_t>(static_cast<uint8_t>(utf8[j]));
    }
    return utf16;
  }

  std::u16string utf16;
  utf16.reserve(len);
  for (std::size_t j = 0; j < i; ++j) {
    utf16.push_back(static_cast<char16_t>(static_cast<uint8_t>(utf8[j])));
  }

  for (; i < len;) {
    const uint8_t b0 = static_cast<uint8_t>(utf8[i]);

    if (b0 <= 0x7F) {
      utf16.push_back(static_cast<char16_t>(b0));
      ++i;
    } else if (b0 >= 0xC2 && b0 <= 0xDF) {
      // 2-byte sequence (U+0080 - U+07FF)
      if (i + 1 >= len) {
        XML_LIB_THROW(std::range_error("Incomplete 2-byte UTF-8 sequence."));
      }
      const uint8_t b1 = static_cast<uint8_t>(utf8[i + 1]);
      if ((b1 & 0xC0) != 0x80) {
        XML_LIB_THROW(std::range_error("Invalid continuation byte in 2-byte UTF-8 sequence."));
      }
      const char32_t cp = ((b0 & 0x1F) << 6) | (b1 & 0x3F);
      utf16.push_back(static_cast<char16_t>(cp));
      i += 2;
    } else if (b0 >= 0xE0 && b0 <= 0xEF) {
      // 3-byte sequence (U+0800 - U+FFFF)
      if (i + 2 >= len) {
        XML_LIB_THROW(std::range_error("Incomplete 3-byte UTF-8 sequence."));
      }
      const uint8_t b1 = static_cast<uint8_t>(utf8[i + 1]);
      const uint8_t b2 = static_cast<uint8_t>(utf8[i + 2]);

      // Overlong check for E0: b1 must be in [0xA0, 0xBF]
      if (b0 == 0xE0 && (b1 < 0xA0 || b1 > 0xBF)) {
        XML_LIB_THROW(std::range_error("Overlong 3-byte UTF-8 sequence."));
      }
      // Surrogate check for ED: b1 must be in [0x80, 0x9F] (rejects 0xD800-0xDFFF)
      if (b0 == 0xED && (b1 < 0x80 || b1 > 0x9F)) {
        XML_LIB_THROW(std::range_error("UTF-8 encoding of surrogate code point is illegal."));
      }
      if ((b1 & 0xC0) != 0x80) {
        XML_LIB_THROW(std::range_error("Invalid continuation byte 1 in 3-byte UTF-8 sequence."));
      }
      if ((b2 & 0xC0) != 0x80) {
        XML_LIB_THROW(std::range_error("Invalid continuation byte 2 in 3-byte UTF-8 sequence."));
      }

      const char32_t cp = ((b0 & 0x0F) << 12) | ((b1 & 0x3F) << 6) | (b2 & 0x3F);
      utf16.push_back(static_cast<char16_t>(cp));
      i += 3;
    } else if (b0 >= 0xF0 && b0 <= 0xF4) {
      // 4-byte sequence (U+10000 - U+10FFFF)
      if (i + 3 >= len) {
        XML_LIB_THROW(std::range_error("Incomplete 4-byte UTF-8 sequence."));
      }
      const uint8_t b1 = static_cast<uint8_t>(utf8[i + 1]);
      const uint8_t b2 = static_cast<uint8_t>(utf8[i + 2]);
      const uint8_t b3 = static_cast<uint8_t>(utf8[i + 3]);

      // Overlong check for F0: b1 must be in [0x90, 0xBF]
      if (b0 == 0xF0 && (b1 < 0x90 || b1 > 0xBF)) {
        XML_LIB_THROW(std::range_error("Overlong 4-byte UTF-8 sequence."));
      }
      // Out-of-range check for F4: b1 must be in [0x80, 0x8F] (rejects > 0x10FFFF)
      if (b0 == 0xF4 && (b1 < 0x80 || b1 > 0x8F)) {
        XML_LIB_THROW(std::range_error("UTF-8 code point exceeds Unicode maximum 0x10FFFF."));
      }
      if ((b1 & 0xC0) != 0x80) {
        XML_LIB_THROW(std::range_error("Invalid continuation byte 1 in 4-byte UTF-8 sequence."));
      }
      if ((b2 & 0xC0) != 0x80 || (b3 & 0xC0) != 0x80) {
        XML_LIB_THROW(std::range_error("Invalid continuation byte in 4-byte UTF-8 sequence."));
      }

      char32_t cp = ((b0 & 0x07) << 18) | ((b1 & 0x3F) << 12) | ((b2 & 0x3F) << 6) | (b3 & 0x3F);
      cp -= 0x10000;
      utf16.push_back(static_cast<char16_t>(0xD800 + (cp >> 10)));
      utf16.push_back(static_cast<char16_t>(0xDC00 + (cp & 0x3FF)));
      i += 4;
    } else {
      XML_LIB_THROW(std::range_error("Invalid leading byte in UTF-8 sequence."));
    }
  }

  return utf16;
}

}// namespace XML_Lib
