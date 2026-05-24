#pragma once
#include "common/XML_Error.hpp"
#include "XML_Converter.hpp"
#include "XML.hpp"

#include "ISource.hpp"

#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>

#include "common/XML_Utility.hpp"

namespace XML_Lib {

class BufferSource final : public ISource
{
public:
  // Bits per byte
  static constexpr int kBitsPerByte{ 8 };
  // BufferSource Error
#ifndef XML_LIB_NO_EXCEPTIONS
  XML_LIB_DEFINE_ERROR("BufferSource");
#endif
  // Constructors/Destructors
  static constexpr std::size_t kMaxSourceBytes{ XML_LIB_MAX_XML_SIZE };

  explicit BufferSource(const std::u16string_view &sourceBuffer, std::size_t maxSourceBytes = kMaxSourceBytes)// UTF16 source BE/LE
  {
    if (sourceBuffer.empty()) { XML_LIB_THROW(Error("Empty source buffer passed to be parsed.")); }
    if (static_cast<std::size_t>(sourceBuffer.size()) > maxSourceBytes / sizeof(char16_t)) {
      XML_LIB_THROW(Error("Source buffer exceeds maximum allowed size."));
    }
    std::u16string utf16xml{ sourceBuffer };
    if (utf16xml.starts_with(u"<?xml")) {
      std::transform(utf16xml.begin(), utf16xml.end(), utf16xml.begin(), [](const char16_t &ch) {
        return static_cast<char16_t>(static_cast<uint16_t>(ch) >> kBitsPerByte | static_cast<uint16_t>(ch) << kBitsPerByte);
      });
    }
    if (!utf16xml.empty() && utf16xml.front() == u'\uFEFF') {
      utf16xml.erase(utf16xml.begin());
    }
    buffer = utf16xml;
    convertCRLFToLF(buffer);
  }
/// @brief
/// Implementation of BufferSource.

  explicit BufferSource(const std::string_view &sourceBuffer, std::size_t maxSourceBytes = kMaxSourceBytes)
  {
    if (sourceBuffer.empty()) { XML_LIB_THROW(Error("Empty source buffer passed to be parsed.")); }
    if (sourceBuffer.size() > maxSourceBytes) {
      XML_LIB_THROW(Error("Source buffer exceeds maximum allowed size."));
    }
    buffer = toUtf16(std::string(sourceBuffer));
    if (!buffer.empty() && buffer.front() == u'\uFEFF') {
      buffer.erase(buffer.begin());
    }
    convertCRLFToLF(buffer);
  }
  BufferSource() = default;
  BufferSource(const BufferSource &other) = delete;
  BufferSource &operator=(const BufferSource &other) = delete;
  BufferSource(BufferSource &&other) = delete;
  BufferSource &operator=(BufferSource &&other) = delete;
  ~BufferSource() noexcept override = default;

  [[nodiscard]] Char current() const override
  {
    if (more()) { return buffer[bufferPosition]; }
    return static_cast<Char>(EOF);
  }
  void next() override
  {
    if (!more()) { XML_LIB_THROW(Error("Parse buffer empty before parse complete.")); }
    bufferPosition++;
    columnNo++;
    if (current() == kLineFeed) {
      lineNo++;
      columnNo = 1;
    }
  }
  [[nodiscard]] bool more() const override { return bufferPosition < static_cast<long>(buffer.size()); }
  void backup(const long length) override
  {
    bufferPosition -= length;
    if (bufferPosition < 0) { bufferPosition = 0; }
  }
  [[nodiscard]] long position() const override { return bufferPosition; }
  [[nodiscard]] std::string getRange(const long start, const long end) override
  {
    if (start < 0 || end < 0 || end < start) {
      XML_LIB_THROW(Error("Invalid range requested from BufferSource."));
    }
    const auto size = static_cast<std::size_t>(buffer.size());
    if (static_cast<std::size_t>(end) > size) {
      XML_LIB_THROW(Error("Requested range exceeds source buffer size."));
    }
    return toUtf8(buffer.substr(start, static_cast<std::size_t>(end) - start));
  }
  void reset() override
  {
    lineNo = 1;
    columnNo = 1;
    bufferPosition = 0;
  }

private:
/// @brief
/// Implementation of convertCRLFToLF.

  static void convertCRLFToLF(String &xmlString)
  {
    size_t pos = xmlString.find(u"\x0D\x0A");
    while (pos != std::string::npos) {
      xmlString.replace(pos, 2, u"\x0A");
      pos = xmlString.find(u"\x0D\x0A", pos + 1);
    }
    pos = xmlString.find(u"\x0D");
    while (pos != std::string::npos) {
      xmlString.replace(pos, 1, u"\x0A");
      pos = xmlString.find(u"\x0D", pos + 1);
    }
  }

  long bufferPosition = 0;
  String buffer;
};
}// namespace XML_Lib